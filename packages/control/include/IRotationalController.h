/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/IRotationalController.h
 */

#ifndef RAM_CONTROL_IROTATIONALCONTROLLER_08_31_2008
#define RAM_CONTROL_IROTATIONALCONTROLLER_08_31_2008

// Project Includes
#include "math/include/Quaternion.h"
#include "math/include/Vector3.h"

#include "control/include/DesiredState.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

/** Defines the interface for controler which controls orientation */
class RAM_EXPORT IRotationalController
{
public:
    virtual ~IRotationalController() {}
    
    // /** Yaws the desired vehicle state by the desired number of degrees */
    // virtual void yawVehicle(double degrees) = 0;

    // /** Pitches the desired vehicle state by the desired number of degrees */
    // virtual void pitchVehicle(double degrees) = 0;

    // /** Rolls the desired vehicle state by the desired number of degrees */
    // virtual void rollVehicle(double degrees) = 0;

    // /** Gets the current desired orientation */
    // virtual math::Quaternion getDesiredOrientation() = 0;
    
    // /** Sets the current desired orientation */
    // virtual void setDesiredOrientation(math::Quaternion) = 0;
    
    // /** Returns true if the vehicle is at the desired orientation */
    // virtual bool atOrientation() = 0;

    // /** Loads current orientation into desired (fixes offset in roll and pitch)
    //  *
    //  *  The desired state quaternion will be "level" in horizontal plane, this
    //  *  will reverse slight offsets in roll and pitch.
    //  *
    //  *  @warning
    //  *      The vehicle should be upright when using this function, otherwise
    //  *      the interpretation of yaw and upright will be nonsensical.
    //  */
    // virtual void holdCurrentHeading() = 0;
};

/** Provides an interface for a implementation of a Rotational Controller */
class RAM_EXPORT IRotationalControllerImp : public IRotationalController
{
  public:
    virtual ~IRotationalControllerImp() {}

    /** Gets the needed vehicle torque based on current vehicle state */
    virtual math::Vector3 rotationalUpdate(double timestep,
                                           math::Quaternion orientation,
                                           math::Vector3 angularRate,
                                           controltest::DesiredStatePtr desiredState) = 0;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_IROTATIONALCONTROLLER_08_31_2008
