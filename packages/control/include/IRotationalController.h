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
