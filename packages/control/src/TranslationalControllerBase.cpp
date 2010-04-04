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
    m_desiredSidewaysSpeed(0),
    m_desiredVelocity(math::Vector2::ZERO),
    m_desiredPosition(math::Vector2::ZERO),
    m_currentVelocity(math::Vector2::ZERO),
    m_currentPosition(math::Vector2::ZERO),
    m_positionThreshold(0),
    m_velocityThreshold(0),
    m_controlMode(ControlMode::OPEN_LOOP)
{
    init(config);
}


void TranslationalControllerBase::setDesiredVelocity(math::Vector2 velocity)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredVelocity = velocity;
    m_controlMode = ControlMode::VELOCITY;
}

void TranslationalControllerBase::setDesiredPosition(math::Vector2 position)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredPosition = position;
    m_controlMode = ControlMode::POSITION;
}

void TranslationalControllerBase::
setDesiredPositionAndVelocity( math::Vector2 position,
                               math::Vector2 velocity)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredPosition = position;
    m_desiredVelocity = velocity;
    m_controlMode = ControlMode::POSITIONANDVELOCITY;
}

math::Vector2 TranslationalControllerBase::getDesiredVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredVelocity;
}

math::Vector2 TranslationalControllerBase::getDesiredPosition()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredPosition;
}


void TranslationalControllerBase::setVelocity(math::Vector2 velocity)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredVelocity = velocity;
    m_controlMode = ControlMode::VELOCITY;
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

    if (m_controlMode != ControlMode::OPEN_LOOP)
    {
        m_desiredSidewaysSpeed = 0;
        m_controlMode = ControlMode::OPEN_LOOP;
    }
}

void TranslationalControllerBase::setSidewaysSpeed(double speed)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredSidewaysSpeed = speed;
    
    if (m_controlMode != ControlMode::OPEN_LOOP)
    {
        m_desiredSpeed = 0;
        m_controlMode = ControlMode::OPEN_LOOP;
    }
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

void TranslationalControllerBase::holdCurrentPosition()
{
    setDesiredPosition(m_currentPosition);
}

TranslationalControllerBase::ControlMode::ModeType
TranslationalControllerBase::getMode()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_controlMode;
}

bool TranslationalControllerBase::atPosition()
{
    math::Vector2 currentPosition, desiredPosition;
    {
        core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
        currentPosition = m_currentPosition;
        desiredPosition = m_desiredPosition;
    }
    math::Vector2 diff = currentPosition - desiredPosition;
    double error[2] = {fabs(diff[1]), fabs(diff[2])};
    return error[0] <= m_positionThreshold && error[1] <= m_positionThreshold;
}

bool TranslationalControllerBase::atVelocity()
{
    math::Vector2 currentVelocity, desiredVelocity;
    {
        core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
        currentVelocity = m_currentVelocity;
        desiredVelocity = m_desiredVelocity;
    }
    math::Vector2 diff = currentVelocity - desiredVelocity;
    double error[2] = {fabs(diff[1]), fabs(diff[2])};
    return error[0] <= m_velocityThreshold && error[1] <= m_velocityThreshold;
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
