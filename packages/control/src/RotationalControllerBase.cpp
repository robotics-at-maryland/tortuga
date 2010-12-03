/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/RotationalControllerBase.h
 */

// Project Includes
#include "control/include/RotationalControllerBase.h"
#include "control/include/ControllerBase.h"

namespace ram {
namespace control {

RotationalControllerBase::RotationalControllerBase(core::ConfigNode config)
{
    init(config);
}
   
math::Vector3 RotationalControllerBase::rotationalUpdate(
    double timestep,
    estimation::IStateEstimatorPtr estimator,
    control::DesiredStatePtr desiredState)
{
    return math::Vector3::ZERO;
}

void RotationalControllerBase::init(core::ConfigNode config)
{
}
        
} // namespace control
} // namespace ram
