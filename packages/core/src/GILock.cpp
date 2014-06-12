/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/core/src/GILock.cpp
 */

// STD Includes
#include <assert.h>

// Project Includes
#include "core/include/GILock.h"

namespace ram {
namespace core {

GILock::GILock()
#ifndef NDEBUG
    : m_locked(false)
#endif // NDEBUG
{
}

void GILock::lock()
{
    assert( ! m_locked && "Cannot lock: Lock is already active" );
    m_state = PyGILState_Ensure();
#ifndef NDEBUG
    m_locked = true;
#endif // NDEBUG
}

void GILock::unlock()
{
    assert( m_locked && "Cannot unlock: Lock is not active" );
    PyGILState_Release(m_state);
#ifndef NDEBUG
    m_locked = false;
#endif // NDEBUG
}

ScopedGILock::ScopedGILock()
{
    m_lock.lock();
}

ScopedGILock::~ScopedGILock()
{
    m_lock.unlock();
}

} // namespace core
} // namespace ram
