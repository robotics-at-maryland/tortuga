/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Chris Giles <cgiles17@umd.edu>
 * All rights reserved.
 *
 * Author: Chris Giles <cgiles17@umd.edu>
 * File:  packages/core/include/Scheduler.h
 */

#ifndef RAM_CORE_SCHEDULER
#define RAM_CORE_SCHEDULER

#include <vector>
#include <map>
#include <boost/thread.hpp>

#include "core/include/Runnable.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

class RAM_EXPORT Scheduler
{
public:

    Scheduler();

    ~Scheduler();

protected:

};

}
}

#endif