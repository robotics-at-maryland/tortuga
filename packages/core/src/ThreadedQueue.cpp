/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/ThreadedQueue.cpp
 */

// Project Includes
#include "core/include/ThreadedQueue.h"

namespace ram {
namespace core {
namespace details {
    
boost::xtime add_xtime(const boost::xtime& a, const boost::xtime& b)
{
    boost::xtime result = {a.sec + b.sec, a.nsec + b.nsec};
    
    // Handle the overflow of nanosecond
    if(result.nsec >= 1000000000)
    {
        result.nsec %= 1000000000;
        result.sec++;
    }

    return result;
}
    
} // namespace details
} // namespace core
} // namespace ram
