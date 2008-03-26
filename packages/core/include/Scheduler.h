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

    void addRunnable(Runnable *r);
    void removeRunnable(Runnable *r);

    void run();
    void stop();

    inline bool getRunning() const
    {
        return running;
    }


protected:

    std::map<std::string, std::vector<Runnable*> >  m_runnables;

    boost::thread* m_mainThread;

    bool m_running;

    void process();

};

}
}

#endif