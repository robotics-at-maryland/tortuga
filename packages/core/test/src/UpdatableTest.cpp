/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Original Author: Paul Bridger <paulbridger.net>
 * Brought in by: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/UpdatableTest.h
 */

// UNIX Includes

// STD Includes
#include <iostream>
#include <cassert>

// Project Includes
#include "core/include/Updatable.h"
#include "core/include/TimeVal.h"

static const int INTERVAL = 100; // 10 Hz

using ram::core::TimeVal;

/** Prints the time between each run and the delta it is off by */
class Printer : public ram::core::Updatable
{
    TimeVal m_lastTime;
    int m_updateInterval;
    int m_count;
    
public:
    Printer(int interval) :
        ram::core::Updatable(),
        m_updateInterval(interval),
        count(0)
    {
        m_lastTime.now();
    }
    
    virtual void update(double timestep)
    {
        TimeVal current = TimeVal::timeOfDay();
        TimeVal delta = current - m_lastTime;
        std::cout << "Step: " << timestep << " Calc Delt: "
                  << timestep - (double)m_updateInterval/1000
                  << " Act Delt: " << delta.get_double() << std::endl;
        count++;
        m_lastTime = current;
    }

    int count;
};

int main()
{
    // Create and start are printer
    Printer* up = new Printer(INTERVAL);
    up->background(INTERVAL);

    // Print for 3 seconds
    sleep(3);

    // Stop printer and wait for it to shutdown
    up->unbackground();
    sleep(1);

    // Verify shutdown
    assert(false == up->backgrounded());

    std::cout << "Update rate (updates/sec): " << up->count / (double)3
              << " Expected: " << 1000 / (double)INTERVAL << std::endl;
    
    return 0;
}
