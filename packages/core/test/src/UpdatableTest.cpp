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
#include <cstdio>
#include <cmath>

// Project Includes
#include "core/include/Updatable.h"
#include "core/include/TimeVal.h"

static const int INTERVAL = 250; // 100 Hz

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

    virtual ~Printer() {};
    
    virtual void update(double timestep)
    {
        TimeVal current = TimeVal::timeOfDay();
        TimeVal delta = current - m_lastTime;
        double current_error = fabs(delta.get_double() -
                                    (double)m_updateInterval/(double)1000) * 1000;
        printf("Step(ms): %-5.2f DStep: %d AStep: %-5.2f CError: %-5.2f"
               " AError: %-5.2f\n", timestep * 1000, m_updateInterval,
               delta.get_double() * 1000,
               (timestep - (double)m_updateInterval/1000) * 1000,
               current_error);

        count++;
        error += current_error;
        m_lastTime = current;
    }

    int count;
    double error;
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
              << " Expected: " << 1000 / (double)INTERVAL
              << " Average Error: " << up->error / up->count << std::endl;
    return 0;
}
