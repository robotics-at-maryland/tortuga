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
    m_backgrounded(0),
    m_interval(100)
{
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
        // Create out background thread
        boost::thread thread(boost::bind(&Updatable::loop, this));
    }
}

void Updatable::unbackground()
{
    boost::mutex::scoped_lock lock(m_stateMutex);

    // The run loop check this value to determine whether it should keep
    // running, next loop through it will stop.
    m_backgrounded = false;
}

bool Updatable::backgrounded()
{
    boost::mutex::scoped_lock lock(m_stateMutex);
    return m_backgrounded;
}

void Updatable::loop()
{
    struct timeval last;
    struct timeval start;
    Usec sleep_time;
    Usec offset;
    struct timespec sleep = {0, 0};
    struct timespec act_sleep = {0, 0};


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

        {
            boost::mutex::scoped_lock lock(m_stateMutex);
            in_background = m_backgrounded;
            interval = m_interval;
        }
        
        if (in_background)
        {
            // On the first loop through, set the step to ideal
            double diff = interval *(double)1000;
            if (0 != last.tv_usec)
                diff = (timeval_diff(&start, &last) + offset);

            // Call our update function
            update(diff / (double)1000000);
            
            // Determine next time to awake
            struct timeval next = start;
            timeval_add(&next, (Usec)interval * USEC_PER_MILLISEC);
            
            // Sleep for the rest for the remainder of our time
            sleep_time = -age(&next);

            // Handle overrun
            if (sleep_time < 0)
                sleep.tv_nsec = interval * NSEC_PER_MILLISEC;
            else
                sleep.tv_nsec = sleep_time * NSEC_PER_USEC;
            
            last = start;
            nanosleep(&sleep, &act_sleep);
        }
        // Time to quit
        else
        {
            break;
        }
    }
}
    
} // namespace core
} // namespace ram
