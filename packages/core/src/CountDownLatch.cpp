/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/CountDownLatch.cpp
 */

// STD Includes
#include <cassert>

// Project Includes
#include "core/include/CountDownLatch.h"

namespace ram {
namespace core {

CountDownLatch::CountDownLatch(int count) :
    m_count(count)
{
}

void CountDownLatch::await()
{
    boost::mutex::scoped_lock lock(m_mutex);

    // Return immediately if count is zero
    if (0 != m_count)
    {
        m_countAtZero.wait(lock);
    }
}
    
bool CountDownLatch::await(boost::xtime timeout)
{
    boost::mutex::scoped_lock lock(m_mutex);

    // Return immediately if count is zero
    if (m_count == 0)
        return true;
    
    return m_countAtZero.timed_wait(lock, timeout);
}

void CountDownLatch::countDown()
{
    boost::mutex::scoped_lock lock(m_mutex);

    // Only decrement if at won
    if (m_count > 0)
        m_count--;

    // Release all waiting threads
    m_countAtZero.notify_all();
}

int CountDownLatch::getCount()
{
    boost::mutex::scoped_lock lock(m_mutex);
    return m_count;
}
    
void CountDownLatch::resetCount(int count)
{
    boost::mutex::scoped_lock lock(m_mutex);

    assert(m_count == 0 && "Can't reset a count not at zero");
    m_count = count;
}

} // namespace core
} // namespace ram
