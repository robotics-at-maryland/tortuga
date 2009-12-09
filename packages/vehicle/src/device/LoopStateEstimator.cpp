/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/LoopStateEstimator.cpp
 */

// Project Includes
#include "vehicle/include/device/LoopStateEstimator.h"

namespace ram {
namespace vehicle {
namespace device {

LoopStateEstimator::LoopStateEstimator(core::ConfigNode config,
                                       core::EventHubPtr eventHub,
                                       IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IStateEstimator(eventHub, config["name"].asString()),
    m_orientation(math::Quaternion::IDENTITY),
    m_velocity(math::Vector2::ZERO),
    m_position(math::Vector2::ZERO),
    m_depth(0)
{
}
    
LoopStateEstimator::~LoopStateEstimator()
{
}

void LoopStateEstimator::orientationUpdate(math::Quaternion orientation,
					   double timeStamp)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_orientation = orientation;
}

void LoopStateEstimator::velocityUpdate(math::Vector2 velocity,
					double timeStamp)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_velocity = velocity;
}

void LoopStateEstimator::positionUpdate(math::Vector2 position,
					double timeStamp)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_position = position;
}
    
void LoopStateEstimator::depthUpdate(double depth,
                                     double timeStamp)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_depth = depth;
}
    
math::Quaternion LoopStateEstimator::getOrientation(std::string obj)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_orientation;
}

math::Vector2 LoopStateEstimator::getVelocity(std::string obj)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_velocity;
}

math::Vector2 LoopStateEstimator::getPosition(std::string obj)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_position;
}
    
double LoopStateEstimator::getDepth(std::string obj)
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_depth;
}

bool LoopStateEstimator::hasObject(std::string obj)
{
    return obj == "vehicle";
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
