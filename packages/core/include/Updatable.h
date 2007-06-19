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
#include <boost/thread/mutex.hpp>

namespace ram {
namespace core {

/** Reprsents and object which can be updated, asyncronously or sequentially.
 *
 *  All you have to do to use it is subclass and implement the update() method,
 *  will be called the given interval in a background thread.
 */
class Updatable
{
public:
    /** Updates the Object.
     *
     *  @param timestep  The time since the last update.
     */
    virtual void update(double timestep) = 0;

    /**  Starts automatic background update.
     *
     * This runs a background thread and calls update in a loop at the given
     * interval.
     *
     * @interval   The time between calls in milliseconds.
     */
    void background(int interval);

    /** Stops background update.
     */
    void unbackground();

    /** Returns true if the thread is running in the background false if not.
     */
    bool backgrounded();

private:
    /** The function which runs the update function in a loop */
    void loop();

    /** Guard the interval and background */
    boost::mutex m_stateMutex;
    bool m_backgrounded;
    int m_interval;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_UPDATABLE_06_11_2006
