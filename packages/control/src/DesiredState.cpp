/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/TrackingTranslationalController.h
 */

#include <iostream>
#include "control/include/Helpers.h"
#include "control/include/DesiredState.h"
#include "vehicle/include/Common.h"
#include "vehicle/include/IVehicle.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram{
namespace controltest{

DesiredStatePtr DesiredState::m_instance = DesiredStatePtr();

DesiredStatePtr DesiredState::getInstance() 
{ 
    return m_instance; 
}

math::Vector2 DesiredState::getDesiredVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredVelocity;
}

math::Vector2 DesiredState::getDesiredPosition()
{   
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredPosition;
}  

math::Quaternion DesiredState::getDesiredOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredOrientation;
}

math::Vector3 DesiredState::getDesiredAngularRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredAngularRate;
}

double DesiredState::getDesiredDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredDepth;
}

void DesiredState::setDesiredVelocity(math::Vector2 velocity)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredVelocity = velocity;
}

void DesiredState::setDesiredPosition(math::Vector2 position)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredPosition = position;
}

void DesiredState::setDesiredDepth(double depth)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredDepth = depth;
}

void DesiredState::setDesiredOrientation(math::Quaternion orientation)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredOrientation = orientation;
}


void DesiredState::setDesiredAngularRate(math::Vector3 angularRate)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredAngularRate = angularRate;
}

void DesiredState::setDesiredYaw(double degrees){}
void DesiredState::setDesiredPitch(double degrees){}
void DesiredState::setDesiredRoll(double degrees){}

DesiredState::DesiredState() :
    m_desiredVelocity(math::Vector2::ZERO),
    m_desiredPosition(math::Vector2::ZERO),
    m_desiredDepth(0),
    m_desiredOrientation(math::Quaternion::ZERO),
    m_desiredAngularRate(math::Vector3::ZERO) {}

DesiredState::~DesiredState() {}

} //namespace control
} //namespace ram
