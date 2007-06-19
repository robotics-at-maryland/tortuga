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

namespace ram {
namespace core {

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
    TimeVal current;
    TimeVal last;
    TimeVal diff;
    TimeVal sleepTime;
    
    while (1)
    {   
        // Set current time to now
        current.now();

        // Grab our running state
        bool in_background = false;
        int interval = 10;

        {
            boost::mutex::scoped_lock lock(m_stateMutex);
            in_background = m_backgrounded;
            interval = m_interval;
        }

        // Trun the ms interval into TimeVal
        TimeVal updateInterval(0, interval * USEC_PER_MILLISEC);
        
        if (in_background)
        {
            // If we are on the first interation, we just say we had the
            // perfect time since the last iteration, other wise we
            // compute it
            if (last.get_double() == 0)
                diff.add(m_interval * USEC_PER_MILLISEC);
            else    
                diff = (current - last);

            // Now run our timed function
            update(diff.get_double());

            // Now compute how long to sleep untill the next iteration
            // (timeOfDay - current) == time to run update()
            sleepTime = updateInterval - (TimeVal::timeOfDay() - current);

            // Ensure that we don't go negative (ie the update took to
            // longer that the its updateInterval)
            // if (sleepTime.get_double() < 0)
                sleepTime = updateInterval;

            // Save beginning time of this loop for timestep calculation
            last = current;

            // Finally sleep
            std::cout << "Sleeping for " << sleepTime.get_double()
                      << " seconds and " << sleepTime.microseconds() << " us" <<std::endl;
            // boost::xtime xt;
            // xt.sec = 0;
            // xt.nsec = 1000 * m_interval;
            // boost::thread::sleep(xt);
            usleep(sleepTime.microseconds());
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
