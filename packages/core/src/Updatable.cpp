/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/Updatable.cpp
 */

// UNIX Includes
#include <unistd.h>

// Library Includes
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "core/include/TimeVal.h"
#include "core/include/Updatable.h"

#include <iostream>

const static long USEC_PER_MILLISEC = 1000;
const static long NSEC_PER_MILLISEC = 1000000;
const static long NSEC_PER_USEC = 1000;

// How close do we try to get to actual sleep time (in usec)
const static long SLEPP_THRESHOLD = 500;

namespace ram {
namespace core {


typedef int64_t Usec;

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

struct timeval *elapse_time(struct timeval *tv, unsigned msec, unsigned jitter) {
    assert(tv);

    gettimeofday(tv, NULL);

    if (msec)
        timeval_add(tv, (Usec) msec*1000);
        
    return tv;
}

    
Updatable::Updatable() :
    m_backgroundThread(0),
    m_backgrounded(0),
    m_interval(100),
    m_threadStopped(1)
{
}

Updatable::~Updatable()
{
    // Join and delete background thread if its still running
    cleanUpBackgroundThread();
}
    
void Updatable::background(int interval)
{
    bool startThread = false;

    {
        boost::mutex::scoped_lock lock(m_stateMutex);

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
        // wait for it to stop before starting a new one
        cleanUpBackgroundThread();

        // Reset the latch count
        if (0 == m_threadStopped.getCount())
            m_threadStopped.resetCount(1);
        
        // Create out background thread
        m_backgroundThread = new boost::thread(boost::bind(&Updatable::loop,
                                                           this));
    }
}

void Updatable::unbackground(bool join)
{
    {
        boost::mutex::scoped_lock lock(m_stateMutex);

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
    boost::mutex::scoped_lock lock(m_stateMutex);
    return m_backgrounded;
}

void Updatable::getState(bool& backgrounded, int& interval)
{
    boost::mutex::scoped_lock lock(m_stateMutex);
    backgrounded = m_backgrounded;
    interval = m_interval;
}
    
void Updatable::loop()
{
    struct timeval last;
    struct timeval start;
    Usec sleep_time;
    Usec offset;

    // This call determines the time it takes for a gettimeofday call
    // This makes the calls more accurate
    gettimeofday(&last, NULL);
    gettimeofday(&start, NULL);
    offset = timeval_diff(&start, &last);

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
        
        if (in_background)
        {
            // On the first loop through, set the step to ideal
            double diff = interval *(double)1000;
            if (0 != last.tv_usec)
                diff = (timeval_diff(&start, &last) + offset);

            // Record time for next run 
            last = start;
            
            // Call our update function
            update(diff / (double)1000000);

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
                    waitForUpdate(sleep_time);
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
    struct timespec sleep = {0, 0};
    struct timespec act_sleep = {0, 0};
    
    sleep.tv_nsec = microseconds * NSEC_PER_USEC;        
    nanosleep(&sleep, &act_sleep);  
}
    
void Updatable::cleanUpBackgroundThread()
{
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
    
} // namespace core     
} // namespace ram
