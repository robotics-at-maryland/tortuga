/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/RotationalControllerBase.h
 */

// Project Includes
#include "control/include/TranslationalControllerBase.h"

namespace ram {
namespace control {
    
TranslationalControllerBase::TranslationalControllerBase(
    core::ConfigNode config) :
    // m_currentVelocity(math::Vector2::ZERO),
    // m_currentPosition(math::Vector2::ZERO),
    // m_positionThreshold(0.1),
    // m_velocityThreshold(0.1),
    m_controlMode(ControlMode::OPEN_LOOP)
{
    init(config);
}
    
math::Vector3 TranslationalControllerBase::translationalUpdate(
    double timestep,
    math::Vector3 linearAcceleration,
    math::Quaternion orientation,
    math::Vector2 position,
    math::Vector2 velocity,
    controltest::DesiredStatePtr desiredState)
{
    return math::Vector3::ZERO;
}

void TranslationalControllerBase::setControlMode(ControlMode::ModeType mode)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_controlMode = mode;
}

void TranslationalControllerBase::init(core::ConfigNode config)
{
    // m_positionThreshold = config["positionThreshold"].asDouble(0.1);
    // m_velocityThreshold = config["velocityThreshold"].asDouble(0.1);
}
    
} // namespace control
} // namespace ram
