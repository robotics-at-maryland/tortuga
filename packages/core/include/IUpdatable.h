/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/IUpdatable.h
 */

#ifndef RAM_CORE_IUPDATABLE_06_22_2006
#define RAM_CORE_IUPDATABLE_06_22_2006

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

/** Represents an object which can be updated, asyncronously or sequentially.
 *
 *  All you have to do to use it is subclass and implement the update() method,
 *  will be called at the given interval in a background thread.
 */
class RAM_EXPORT IUpdatable
{
public:
    enum Priority
    {
        RT_HIGH_PRIORITY,
        RT_NORMAL_PRIORITY,
        RT_LOW_PRIORITY,
        HIGH_PRIORITY,
        NORMAL_PRIORITY,
        LOW_PRIORITY
    };
    
    virtual ~IUpdatable() {};

    /** Changes the priority of the background thread
     *
     *  Higher priority threads will be run in whenever they need to, even if
     *  there are other lower priority threads which can run. RT threads are
     *  real time threads.  Even the lowest priority real time threads has
     *  priority over any non-realtime thread.
     *
     *  @priority
     *      A value of the Priority enum
     */
    virtual void setPriority(Priority priority) = 0;

    /** Returns the current priority of the background thread */
    virtual Priority getPriority() = 0;

    /** Set which CPU core you wish the background thread to run
     *
     *  By default it will run on any core.
     *
     *  @core
     *      The CPU core you wish to run on, starts at 0.
     */
    virtual void setAffinity(size_t core) = 0;

    /** Gets the current core the background thread runs on
     *
     *  @return -1, if no affinity has been set.
     */
    virtual int getAffinity() = 0;
    
    /** Updates the Object.
     *
     *  This is called in the background once the object has been backgrounded.
     *  Do not call unbackground(true) while in this method.  You will get a
     *  deadlock.  This is because unbackground true waits for the background
     *  thread to quit, and you in update you *are* the background thread. If
     *  you wish to stop the background thread, from the background thread, use
     *  unbackground(false).*
     *
     *  @param timestep
     *      The time since the last update.
     */
    virtual void update(double timestep) = 0;

    /** Starts automatic background update.
     *
     *  This runs a background thread and calls update in a loop at the given
     *  interval. If the thread is already running it will just change its update
     *  interval.
     *
     * @param interval
     *    The time between calls in milliseconds, a negative number
     *    means the object runs at its own pace.  This can be full out
     *    or waiting on incoming events.
     */
    virtual void background(int interval = -1) = 0;

    /** Stops background update.
     *
     * @param join
     *    If true the function won't return until the background thread has
     *    stopped and been joined.
     */
    virtual void unbackground(bool join = false) = 0;

    /** Returns true if the thread is running in the background false if not.
     */
    virtual bool backgrounded() = 0;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_IUPDATABLE_06_22_2006
