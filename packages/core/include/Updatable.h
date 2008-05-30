/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/Updatable.h
 */

#ifndef RAM_CORE_UPDATABLE_06_11_2006
#define RAM_CORE_UPDATABLE_06_11_2006

// Library Includes
#include <boost/utility.hpp>
#include <boost/thread/mutex.hpp>

// Forward declare boost::thread
namespace boost { class thread; }

// Project Includes
#include "core/include/IUpdatable.h"
#include "core/include/CountDownLatch.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

/** Reprsents and object which can be updated, asyncronously or sequentially.
 *
 *  All you have to do to use it is subclass and implement the update() method,
 *  will be called the given interval in a background thread.
 */
class RAM_EXPORT Updatable : public IUpdatable, boost::noncopyable
{
public:
    Updatable();
    virtual ~Updatable();

    virtual void setPriority(Priority priority);

    virtual Priority getPriority();

    virtual void setAffinity(size_t core);

    virtual int getAffinity();
    
    virtual void update(double timestep) = 0;

    virtual void background(int interval = -1);

    virtual void unbackground(bool join = false);

    virtual bool backgrounded();

protected:
    /** Gets copies of the internal state */
    void getState(bool& backgrounded, int& interval);
    
    /** The function which runs the update function in a loop */
    virtual void loop();

    /** This function executes the wait for next frame */
    virtual void waitForUpdate(long microseconds);
    
private:
    /** Simple message to talk to background thread */
    enum StateChange {
        PRIORITY = 1,
        AFFINITY = 2
    };
    
    /** Joins and delete's the background thread */
    void cleanUpBackgroundThread();

    /** Determines constants relating to thead, based on OS and machine
     *
     *  Determines the proper thread priorities, and CPU count.
     */
    void initThreadingSettings();

    /** Sets the priority of the running thread */
    void setThreadPriority();

    /** Sets the affinity of the running thread */
    void setThreadAffinity();
    
    /** Guard the interval and background */
    boost::mutex m_upStateMutex;

    /** Whether or not the thread is running in the background */
    bool m_backgrounded;

    /** Number of milliseconds between updates */
    int m_interval;

    /** The priority of the running background thread */
    Priority m_priority;

    /** The core which background thread will run on */
    int m_affinity;

    /** If the above settings have been changed */
    int m_settingChange;
    
    /** Syncronizes access background tread, and latch */
    boost::mutex m_threadStateMutex;
    
    /** The current backgrond thread */
    boost::thread* m_backgroundThread;
    
    CountDownLatch m_threadStopped;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_UPDATABLE_06_11_2006
