/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Chris Giles <cgiles17@umd.edu>
 * All rights reserved.
 *
 * Author: Chris Giles <cgiles17@umd.edu>
 * File:  packages/core/include/Runnable.h
 */

#ifndef RAM_CORE_RUNNABLE
#define RAM_CORE_RUNNABLE

#include <string>

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

enum ThreadPriority
{
    LowPriority = 0,
    MediumPriority = 1,
    HighPriority = 2,
    MaxPriority = 3
};

class RAM_EXPORT Runnable
{
public:

    virtual void run() = 0;

    inline ThreadPriority getPriority()
    {
        return priority;
    }

    inline double getFrequency()
    {
        return frequency;
    }

    inline int getAffinity()
    {
        return affinity;
    }

    inline std::string getThread()
    {
        return thread;
    }

protected:

    ThreadPriority priority;
    double frequency;
    int affinity;
    std::string thread;

};

}
}

#endif