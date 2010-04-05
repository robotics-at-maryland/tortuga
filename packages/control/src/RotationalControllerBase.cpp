/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/RotationalControllerBase.h
 */

// Project includes
#include "control/include/RotationalControllerBase.h"
#include "control/include/ControllerBase.h"

namespace ram {
namespace control {

RotationalControllerBase::RotationalControllerBase(core::ConfigNode config) :
        m_orientationThreshold(0.001)
{
    init(config);
}
    
void RotationalControllerBase::yawVehicle(double degrees)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredOrientation = ControllerBase::yawVehicleHelper(
        m_desiredOrientation, degrees);
}

void RotationalControllerBase::pitchVehicle(double degrees)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredOrientation = ControllerBase::pitchVehicleHelper(
        m_desiredOrientation, degrees);
}

void RotationalControllerBase::rollVehicle(double degrees)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredOrientation = ControllerBase::rollVehicleHelper(
        m_desiredOrientation, degrees);
}

math::Quaternion RotationalControllerBase::getDesiredOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredOrientation;
}
    
void RotationalControllerBase::setDesiredOrientation(
    math::Quaternion orientation)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredOrientation = orientation;
}
    
bool RotationalControllerBase::atOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);

    // Find the quaternion error between the two
    math::Quaternion error =
        m_desiredOrientation.errorQuaternion(m_currentOrientation);
  
    // This does the sqrt of the sum of the squares for the first three elements
    math::Vector3 tmp(error.ptr());
    if (tmp.length() > m_orientationThreshold)
        return false;
    else
        return true;
}

void RotationalControllerBase::holdCurrentHeading()
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredOrientation = ControllerBase::holdCurrentHeadingHelper(
        m_currentOrientation);
}

math::Vector3 RotationalControllerBase::rotationalUpdate(
    double timestep,
    math::Quaternion orientation,
    math::Vector3 angularRate)
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
