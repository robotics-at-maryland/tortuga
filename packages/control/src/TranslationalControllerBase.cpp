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
    m_desiredSpeed(0),
    m_desiredSidewaysSpeed(0)
{
    init(config);
}

void TranslationalControllerBase::setVelocity(math::Vector2 velocity)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredVelocity = velocity;
}

math::Vector2 TranslationalControllerBase::getVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredVelocity;
}
    
void TranslationalControllerBase::setSpeed(double speed)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredSpeed = speed;
}

void TranslationalControllerBase::setSidewaysSpeed(double speed)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredSidewaysSpeed = speed;
}

double TranslationalControllerBase::getSpeed()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredSpeed;
}

double TranslationalControllerBase::getSidewaysSpeed()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredSidewaysSpeed;
}

math::Vector3 TranslationalControllerBase::translationalUpdate(
    double timestep,
    math::Vector3 linearAcceleration,
    math::Quaternion orientation,
    math::Vector2 position,
    math::Vector2 velocity)
{
    return math::Vector3::ZERO;
}
    
void TranslationalControllerBase::init(core::ConfigNode config)
{
}
    
} // namespace control
} // namespace ram
