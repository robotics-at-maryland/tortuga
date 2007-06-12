/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Updatable.h
 */

#ifndef RAM_VEHICLE_UPDATABLE_06_11_2006
#define RAM_VEHICLE_UPDATABLE_06_11_2006

/** Reprsents and object which can be updated, asyncronously or sequentially.
 */
class IUpdatable
{
public:
    /** Updates the Object.
     *
     *  @param timestep  The time since the last update.
     */
    void update(double timestep) = 0;

    /**  Start automatic background update.
     *
     * This would utilize threads and locking to keep everything up to date
     */
    void startContinuousUpdate() = 0;

    /**  Stops what ever current background update system there is.
     */
    void stop_continuous_update() = 0;
};

#endif // RAM_VEHICLE_UPDATABLE_06_11_2006
