/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/core/include/GILock.h
 */

#ifndef RAM_CORE_GILOCK_H_12_14_2010
#define RAM_CORE_GILOCK_H_12_14_2010

// Library Includes
#include <boost/python.hpp>

namespace ram {
namespace core {

class GILock
{
public:
    GILock();

    /* Locks the python interpreter for use by this thread. This function
       should always be matched with a call to unlock.

       It is not safe to call this multiple times. lock() should only be called
       once per lock. Multiple locks can exist at the same time.
    */
    void lock();

    /* Unlocks the python interpreter for use by this thread. This function
       is only safe to call after lock() has been called.
    */
    void unlock();

private:
    PyGILState_STATE m_state;
#ifndef NDEBUG
    bool m_locked;
#endif // NDEBUG
};

class ScopedGILock
{
public:
    ScopedGILock();

    ~ScopedGILock();

private:
    GILock m_lock;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_GILOCK_H_12_14_2010
