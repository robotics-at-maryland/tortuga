/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Chris Giles <cgiles17@umd.edu>
 * All rights reserved.
 *
 * Author: Chris Giles <cgiles17@umd.edu>
 * File:  packages/core/include/Scheduler.h
 */

#include <boost/cstdint.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <iostream>

#include "core/include/Scheduler.h"

namespace ram {
namespace core {

Scheduler::Scheduler() : m_mainThread(0), m_running(false)
{
}

Scheduler::~Scheduler()
{
    stop();
}

void Scheduler::addRunnable(Runnable *r)
{
    m_runnables[r->getThread()].push_back(r);
}

void Scheduler::removeRunnable(Runnable *r)
{
}

void Scheduler::run()
{
    if (m_running)
        return;

    m_running = true;
    m_mainThread = new boost::thread(boost::bind(&Scheduler::process, this));
}

void Scheduler::stop()
{
    m_running = false;
    m_mainThread->join();
    delete m_mainThread;
    m_mainThread = 0;
}

void Scheduler::process()
{
    while (m_running)
    {
        std::cout << "RUNNING\n";
    }
}

}
}