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

namespace ram {
namespace core {

/** Reprsents and object which can be updated, asyncronously or sequentially.
 *
 *  All you have to do to use it is subclass and implement the update() method,
 *  will be called the given interval in a background thread.
 */
class Updatable : public IUpdatable, boost::noncopyable
{
public:
    Updatable();
    virtual ~Updatable();

    
    /** Updates the Object.
     *
     *  This is called in the background once the object has been backgrounded.
     *
     *  @param timestep  The time since the last update.
     */
    virtual void update(double timestep) = 0;

    /**  Starts automatic background update.
     *
     * This runs a background thread and calls update in a loop at the given
     * interval. If the thread is already running it will just change its update
     * interval.
     *
     * @interval   The time between calls in milliseconds, a negative number
     *             means the object runs at its own pace.  This can be full out
     *             or waiting on incoming events.
     */
    virtual void background(int interval = -1);

    /** Stops background update.
     *
     * @join  If true the function won't return until the background thread has
     *        stopped and been joined.
     */
    virtual void unbackground(bool join = false);

    /** Returns true if the thread is running in the background false if not.
     */
    virtual bool backgrounded();

protected:
    /** Gets copies of the internal state */
    void getState(bool& backgrounded, int& interval);
    
    /** The function which runs the update function in a loop */
    virtual void loop();

    /** This function executes the wait for next frame */
    virtual void waitForUpdate(long microseconds);
    
private:
    /** Joins and delete's the background thread */
    void cleanUpBackgroundThread();
    
    /** The current backgrond thread */
    boost::thread* m_backgroundThread;
    
    /** Guard the interval and background */
    boost::mutex m_upStateMutex;

    /** Whether or not the thread is running in the background */
    bool m_backgrounded;

    /** Number of milliseconds between updates */
    int m_interval;

    CountDownLatch m_threadStopped;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_UPDATABLE_06_11_2006
