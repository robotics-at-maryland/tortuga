/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/Updatable.cpp
 */

// Library Includes
#include <boost/cstdint.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "core/include/Feature.h"
#include "core/include/TimeVal.h"
#include "core/include/Events.h"
#include "core/include/Updatable.h"

// System Includes
#ifdef RAM_POSIX
#include <unistd.h>
#include <pthread.h>
#include <sys/resource.h>

#ifdef RAM_LINUX
    // Only Linux support thread affinity on POSIX platforms
    #include <sys/syscall.h>
    #include <unistd.h>
    #define gettid(NOT_USED) ((pid_t)syscall(SYS_gettid))
#elif defined(RAM_DARWIN)
    #include <sys/types.h>
    #include <sys/sysctl.h>
#endif // RAM_LINUX

#elif defined(RAM_WINDOWS) 
    #include <windows.h> // For Sleep()
#else 
    #error "Unsupported platform"
#endif // RAM_POSIX

#include <iostream>

const static long USEC_PER_MILLISEC = 1000;
const static long NSEC_PER_MILLISEC = 1000000;
const static long NSEC_PER_USEC = 1000;

// How close do we try to get to actual sleep time (in usec)
const static long SLEEP_THRESHOLD = 500;

static int HIGH_PRIORITY_VALUE = 0;
static int NORMAL_PRIORITY_VALUE = 0;
static int LOW_PRIORITY_VALUE = 0;
static int RT_HIGH_PRIORITY_VALUE = 0;
static int RT_NORMAL_PRIORITY_VALUE = 0;
static int RT_LOW_PRIORITY_VALUE = 0;
static size_t CPU_COUNT = 0;

namespace ram {
namespace core {


typedef boost::int64_t Usec;

// Adapted from the avahi library, under LPGL
int timeval_compare(const struct timeval *a, const struct timeval *b) {
    assert(a);
    assert(b);

    if (a->tv_sec < b->tv_sec)
        return -1;

    if (a->tv_sec > b->tv_sec)
        return 1;

    if (a->tv_usec < b->tv_usec)
        return -1;

    if (a->tv_usec > b->tv_usec)
        return 1;

    return 0;
}

Usec timeval_diff(const struct timeval *a, const struct timeval *b) {
    assert(a);
    assert(b);

    if (timeval_compare(a, b) < 0)
        return - timeval_diff(b, a);

    return ((Usec) a->tv_sec - b->tv_sec)*1000000 + a->tv_usec - b->tv_usec;
}

struct timeval* timeval_add(struct timeval *a, Usec usec) {
    Usec u;
    assert(a);

    u = usec + a->tv_usec;

    if (u < 0) {
        a->tv_usec = (long) (1000000 + (u % 1000000));
        a->tv_sec += (long) (-1 + (u / 1000000));
    } else {
        a->tv_usec = (long) (u % 1000000);
        a->tv_sec += (long) (u / 1000000);
    }

    return a;
}

Usec age(const struct timeval *a) {
    struct timeval now;
    
    assert(a);

    gettimeofday(&now, NULL);

    return timeval_diff(&now, a);
}

struct timeval *elapse_time(struct timeval *tv, unsigned msec) {
    assert(tv);

    gettimeofday(tv, NULL);

    if (msec)
        timeval_add(tv, (Usec) msec*1000);
        
    return tv;
}

    
Updatable::Updatable(EventPublisher *publisher) :
    m_backgrounded(0),
    m_interval(100),
    m_priority(NORMAL_PRIORITY),
    m_priorityValue(NORMAL_PRIORITY_VALUE),
    m_affinity(-1),
    m_settingChange(0),
    m_backgroundThread(0),
    m_threadStopped(1),
    m_publisher(publisher),
    m_profileCount(0)
{
    initThreadingSettings();
}

Updatable::~Updatable()
{
    // Join and delete background thread if its still running
    cleanUpBackgroundThread();
}

void Updatable::setPriority(Priority priority)
{
    // Translate prioirty value
    int priorityValue = 0;
    
    switch (priority)
    {
        case HIGH_PRIORITY:
        {
            priorityValue = HIGH_PRIORITY_VALUE;
            break;
        };
        
        case NORMAL_PRIORITY:
        {
            priorityValue = NORMAL_PRIORITY_VALUE;
            break;
        };
        
        case LOW_PRIORITY:
        {
            priorityValue = LOW_PRIORITY_VALUE;
            break;
        };
        
        case RT_HIGH_PRIORITY:
        {
            priorityValue = RT_HIGH_PRIORITY_VALUE;
            break;
        };
        
        case RT_NORMAL_PRIORITY:
        {
            priorityValue = RT_NORMAL_PRIORITY_VALUE;
            break;
        };
        
        case RT_LOW_PRIORITY:
        {
            priorityValue = RT_LOW_PRIORITY_VALUE;
            break;
        };
        
        default:
            assert(false && "Invalid thread priority");
    }

    // Make sure we only do real time threads on Linux
    #ifndef RAM_LINUX
    assert(priorityValue <= LOW_PRIORITY &&
           "Can't have real time threads on non-linux platforms");
    #endif
    
    // Set value if needed
    boost::mutex::scoped_lock lock(m_upStateMutex);

    
    if (priority != m_priority)
    {
        m_priority = priority;
        m_priorityValue = priorityValue;
        // Set priority change flag
        m_settingChange |= PRIORITY;
    }
}

Updatable::Priority Updatable::getPriority()
{
    boost::mutex::scoped_lock lock(m_upStateMutex);
    return m_priority;
}

void Updatable::setAffinity(size_t core)
{
    boost::mutex::scoped_lock lock(m_upStateMutex);
    assert(CPU_COUNT != 1 && "Can't set affinity on single core system");
    assert(core < CPU_COUNT && "Core too large");
    m_affinity = (int)core;
    m_settingChange |= AFFINITY;
}

int Updatable::getAffinity()
{
    boost::mutex::scoped_lock lock(m_upStateMutex);
    return m_affinity;
}
     
void Updatable::background(int interval)
{
    bool startThread = false;

    {
        boost::mutex::scoped_lock lock(m_upStateMutex);

        // Set state
        m_interval = interval;

        // Only start up the background thread if we aren't already
        // running
        if (!m_backgrounded)
        {
            startThread = true;
            m_backgrounded = true;
        }
    }

    if (startThread)
    {
        // Join and delete background thread if it exists, if it does exist
        // wait for it to stop before starting a new one (It really shouldn't)
        cleanUpBackgroundThread();

        {
            boost::mutex::scoped_lock lock(m_threadStateMutex);
            
            // Reset the latch count
            if (0 == m_threadStopped.getCount())
                m_threadStopped.resetCount(1);
        
            // Create out background thread
            m_backgroundThread = new boost::thread(boost::bind(&Updatable::loop,
                                                               this));
        }
    }
}

void Updatable::unbackground(bool join)
{
    {
        boost::mutex::scoped_lock lock(m_upStateMutex);

        // Leave early if its already backgrounded and we aren't joining
        if (!m_backgrounded && !join)
            return;
        
        // The run loop check this value to determine whether it should keep
        // running, next loop through it will stop.
        m_backgrounded = false;
    }

    // Wait for background thread to stop runnig and the delete it
    if (join)
        cleanUpBackgroundThread();
}

bool Updatable::backgrounded()
{
    boost::mutex::scoped_lock lock(m_upStateMutex);
    return m_backgrounded;
}

void Updatable::getState(bool& backgrounded, int& interval)
{
    boost::mutex::scoped_lock lock(m_upStateMutex);
    backgrounded = m_backgrounded;
    interval = m_interval;
}
    
void Updatable::loop()
{
    struct timeval last;
    struct timeval start;
    struct timeval profile; // last time a profile update happened
    Usec sleep_time;
    Usec offset;

    // This call determines the time it takes for a gettimeofday call
    // This makes the calls more accurate
    gettimeofday(&last, NULL);
    gettimeofday(&start, NULL);
    offset = timeval_diff(&start, &last);

    // Get time of day for profiling
    gettimeofday(&profile, NULL);
    m_profileCount = 0;

    // Zero last so we can check
    last.tv_usec = 0;
    
    while (1)
    {
        // Grab current time
        gettimeofday(&start, NULL);

        // Grab our running state
        bool in_background = false;
        int interval = 10;
        getState(in_background, interval);

        // Change thread state if needed
        {
            boost::mutex::scoped_lock lock(m_upStateMutex);
            if (m_settingChange & PRIORITY)
                setThreadPriority();

            if (m_settingChange & AFFINITY)
                setThreadAffinity();

            m_settingChange = 0;
        }
        
        if (in_background)
        {
            // On the first loop through, set the step to ideal
            double diff = (double)(interval *(double)1000);
            if (0 != last.tv_usec)
                diff = (double)(timeval_diff(&start, &last) + offset);

            // Record time for next run 
            last = start;
            
            // Call our update function
            update(diff / (double)1000000);
            m_profileCount += 1;

            Usec profile_time = timeval_diff(&last, &profile);
            // If 1 second has passed since the last profile, publish and reset
            if (profile_time > 1000000)
            {
                if (m_publisher) {
                    IntEventPtr event(new IntEvent());
                    event->data = m_profileCount;
                    m_publisher->publish(IUpdatable::PROFILE, event);
                }
                profile = last;
                m_profileCount = 0;
            }

            // Only sleep if we aren't running all out
            if (interval > 0)
            {
                // Determine next time to awake
                struct timeval next = start;
                timeval_add(&next, (Usec)interval * USEC_PER_MILLISEC);
            
                // Sleep for the rest for the remainder of our time
                sleep_time = -age(&next);

                // Handle overrun
                if (sleep_time < 0)
                    sleep_time = interval * USEC_PER_MILLISEC;

                // If the wait ends early keep waiting
                while(sleep_time > SLEEP_THRESHOLD)
                {
                    waitForUpdate((long)sleep_time);
                    sleep_time = -age(&next);
                }
            }
        }
        // Time to quit
        else
        {
            break;
        }
    }

    // Release an threads waiting on
    m_threadStopped.countDown();
}

void Updatable::waitForUpdate(long microseconds)
{
#ifdef RAM_POSIX
    struct timespec sleep = {0, 0};
    struct timespec act_sleep = {0, 0};
    
    sleep.tv_nsec = microseconds * NSEC_PER_USEC;        
    nanosleep(&sleep, &act_sleep);  
#else
    Sleep(microseconds / USEC_PER_MILLISEC);
#endif
}
    
void Updatable::cleanUpBackgroundThread()
{
    boost::mutex::scoped_lock lock(m_threadStateMutex);

    if (0 != m_backgroundThread)
    {
        // Wait for the background thread to stop
        if (m_threadStopped.getCount() != 0)
        {
            m_threadStopped.await();
            m_backgroundThread->join();
        }

        delete m_backgroundThread;
        m_backgroundThread = 0;
    }
}

void Updatable::initThreadingSettings()
{
    static bool init = false;
    
    if (!init)
    {
#ifdef RAM_POSIX
        
#ifdef RAM_LINUX
        RT_HIGH_PRIORITY_VALUE = sched_get_priority_max(SCHED_FIFO);
        RT_LOW_PRIORITY_VALUE = sched_get_priority_min(SCHED_FIFO);

        // Check default affinity set to determine CPU count
        cpu_set_t defaultSet;
        sched_getaffinity(0, sizeof(defaultSet), &defaultSet);

        CPU_COUNT = 0;
        while (CPU_ISSET((int)CPU_COUNT, &defaultSet))
            CPU_COUNT++;
        assert(CPU_COUNT != 0 && "Getting CPU count failed");

        // Assume these are standard accross all systems
        HIGH_PRIORITY_VALUE = -20;
        NORMAL_PRIORITY_VALUE = 0;
        LOW_PRIORITY_VALUE = 10;
        
        // We can test these values well, so lets assert to make sure they
        // make sense
        assert(RT_HIGH_PRIORITY_VALUE > RT_LOW_PRIORITY_VALUE &&
               "Cannot determine proper thread prorities");
        
        RT_NORMAL_PRIORITY_VALUE = RT_LOW_PRIORITY_VALUE +
            ((RT_HIGH_PRIORITY_VALUE - RT_LOW_PRIORITY_VALUE) / 2);

        // Check to make sure these values all make sense
        assert(RT_HIGH_PRIORITY_VALUE > RT_NORMAL_PRIORITY_VALUE &&
               "Cannot determine proper thread prorities");
        assert(RT_NORMAL_PRIORITY_VALUE > RT_LOW_PRIORITY_VALUE &&
               "Cannot determine proper thread prorities");        
#elif defined(RAM_DARWIN)
	//        RT_HIGH_PRIORITY_VALUE = PTHREAD_MAX_PRIORITY;
	//        RT_LOW_PRIORITY_VALUE = PTHREAD_MIN_PRIORITY;

        size_t size = sizeof(CPU_COUNT) ;
        int ret = sysctlbyname("hw.ncpu", &CPU_COUNT, &size, NULL, 0);
        assert(ret == 0 && "Getting CPU count failed");
#else
        #error "Unsupported platform"
#endif

#elif defined(RAM_WINDOWS)
        // Not yet implemented
#else
        #error "Unsupported platform"
#endif
        init = true;
    }
}

void Updatable::setThreadPriority()
{
    switch (m_priority)
    {
        case HIGH_PRIORITY:
        case NORMAL_PRIORITY:
        case LOW_PRIORITY:
        {
#ifdef RAM_POSIX
            int which = 0;
            int who = 0;
#ifdef RAM_DARWIN
            which = PRIO_DARWIN_THREAD;
#else
            who = gettid();
#endif 
            if(setpriority(which, who, m_priorityValue))
                perror("ERROR setpriority");

#elif defined(RAM_WINDOWS)
            // Not yet implemented
#else
    #error "Unsupported platform"
#endif
            break;
        };
        
        case RT_HIGH_PRIORITY:
        case RT_NORMAL_PRIORITY:
        case RT_LOW_PRIORITY:
        {
#ifdef RAM_LINUX
/* Use the following            
    struct sched_param param;  // scheduling priority
    int policy = SCHED_RR;     // scheduling policy
    
    // Get the current thread id
    
    pthread_t thread_id = pthread_self();
    
    // To set the scheduling priority of the thread
    param.sched_priority = 90;
    pthread_setschedparam(thread_id, policy, &param);
*/
            
#else
	  assert(false && "Unsupported platform");
#endif
            break;
        };
        
        default:
            assert(false && "Invalid thread priority");
    }
    

}

void Updatable::setThreadAffinity()
{
#ifdef RAM_LINUX
    // Create a mask which runs us on the proper CPU
    cpu_set_t cpuMask;
    CPU_ZERO(&cpuMask);
    CPU_SET(m_affinity, &cpuMask);
    
    if(sched_setaffinity(0, sizeof(cpuMask), &cpuMask))
        perror("ERROR sched_setaffinity");

#elif defined(RAM_DARWIN)
    // Not supported    
#elif defined(RAM_WINDOWS)
    // Not yet implemented
#else
    #error "Unsupported platform"
#endif
}
   
} // namespace core     
} // namespace ram
