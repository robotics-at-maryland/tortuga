/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/estimation/src/Obstacle.cpp
 */

// STD Includes
#include <stdexcept>

// Project Includes
#include "estimation/include/Obstacle.h"

#define THROW_ERROR( obj ) \
    throw std::runtime_error(# obj " is not a valid attribute for this obstacle")

namespace ram {
namespace estimation {

Obstacle::Obstacle()
    : m_location(math::Vector3::ZERO),
      m_locationCovariance(math::Matrix3::IDENTITY),
      m_attackOrientation(math::Quaternion::IDENTITY)
{
}

Obstacle::~Obstacle()
{
}

math::Vector3 Obstacle::getLocation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_location;
}

math::Matrix3 Obstacle::getLocationCovariance()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_locationCovariance;
}

math::Quaternion Obstacle::getAttackOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_attackOrientation;
}

 
void Obstacle::setLocation(math::Vector3 location)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_location = location;
}

void Obstacle::setLocationCovariance(math::Matrix3 covariance)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_locationCovariance = covariance;
}

void Obstacle::setAttackOrientation(math::Quaternion orientation)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_attackOrientation = orientation;
}

} // namespace estimation
} // namespace ram
