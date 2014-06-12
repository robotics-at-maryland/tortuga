/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/DepthControllerBase.h
 */

// STD Includes

// Library Includes

// Project Includes
#include "control/include/DepthControllerBase.h"

namespace ram {
namespace control {

DepthControllerBase::DepthControllerBase(core::ConfigNode config)
{
    init(config);
}

math::Vector3 DepthControllerBase::depthUpdate(double timestep,
                                 estimation::IStateEstimatorPtr estimator,
                                 control::DesiredStatePtr desiredState)
{
    return math::Vector3::ZERO;
}

void DepthControllerBase::init(core::ConfigNode config)
{
}
    
} // namespace control
} // namespace ram
