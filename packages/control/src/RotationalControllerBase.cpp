/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/RotationalControllerBase.h
 */

// Project includes
#include <iostream>
#include "control/include/RotationalControllerBase.h"
#include "control/include/ControllerBase.h"

namespace ram {
namespace control {

RotationalControllerBase::RotationalControllerBase(core::ConfigNode config) :
    m_orientationThreshold(0.001)
{
    init(config);
}
   
math::Vector3 RotationalControllerBase::rotationalUpdate(
    double timestep,
    math::Quaternion orientation,
    math::Vector3 angularRate,
    controltest::DesiredStatePtr desiredState)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_currentOrientation = orientation;

    return math::Vector3::ZERO;
}

void RotationalControllerBase::init(core::ConfigNode config)
{
    m_orientationThreshold =
        config["orientationThreshold"].asDouble(ORIENTATION_THRESHOLD);
}
        
} // namespace control
} // namespace ram
