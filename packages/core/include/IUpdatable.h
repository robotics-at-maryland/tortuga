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

/** Reprsents and object which can be updated, asyncronously or sequentially.
 *
 *  All you have to do to use it is subclass and implement the update() method,
 *  will be called the given interval in a background thread.
 */
class RAM_EXPORT IUpdatable
{
public:
    virtual ~IUpdatable() {};

    
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
    virtual void background(int interval = -1) = 0;

    /** Stops background update.
     *
     * @join  If true the function won't return until the background thread has
     *        stopped and been joined.
     */
    virtual void unbackground(bool join = false) = 0;

    /** Returns true if the thread is running in the background false if not.
     */
    virtual bool backgrounded() = 0;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_IUPDATABLE_06_22_2006
