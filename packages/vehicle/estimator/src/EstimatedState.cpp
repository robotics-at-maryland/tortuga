/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/src/EstimatedState.cpp
 */


// Library Includes

// Package Includes
#include "vehicle/estimator/include/EstimatedState.h"

namespace ram {
namespace estimator {

EstimatedState::EstimatedState() :
    estimatedPosition(math::Vector2::ZERO),
    estimatedVelocity(math::Vector2::ZERO),
    estimatedLinearAcceleration(math::Vector3::ZERO),
    estimatedAngularRate(math::Vector3::ZERO),
    estimatedOrientation(math::Quaternion::IDENTITY),
    estimatedDepth(0),
    estimatedDepthDot(0)
{
    
}

math::Vector2 EstimatedState::getEstimatedPosition()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estimatedPosition;
}

math::Vector2 EstimatedState::getEstimatedVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estimatedVelocity;
}

math::Vector3 EstimatedState::getEstimatedLinearAcceleration()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estimatedLinearAcceleration;
}

math::Vector3 EstimatedState::getEstimatedAngularRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estimatedAngularRate;
}

math::Quaternion EstimatedState::getEstimatedOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estimatedOrientation;
}

double EstimatedState::getEstimatedDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estimatedDepth;
}

double EstimatedState::getEstimatedDepthDot()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estimatedDepthDot;
}

void EstimatedState::setEstimatedPosition(math::Vector2 position)
{

}

void EstimatedState::setEstimatedVelocity(math::Vector2 velocity)
{

}

void EstimatedState::setEstimatedLinearAcceleration(
    math::Vector3 linearAcceleration)
{

}

void EstimatedState::setEstimatedAngularRate(
    math::Vector3 angularRate)
{

}

void EstimatedState::setEstimatedOrientation(
    math::Quaternion orientation)
{

}

void EstimatedState::setEstimatedDepth(double depth)
{

}

void EstimatedState::setEstimatedDepthDot(double depthDot)
{

}

void EstimatedState::addObstacle(std::string name, ObstaclePtr obstacle)
{

}

math::Vector2 EstimatedState::getObstaclePosition(std::string name)
{
    return math::Vector2::ZERO;
}

double EstimatedState::getObstacleDepth(std::string name)
{
    return 0;
}

} // namespace estimator
} // namespace ram
