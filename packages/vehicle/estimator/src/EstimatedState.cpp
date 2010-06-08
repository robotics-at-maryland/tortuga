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
#include "math/include/Events.h"
#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace estimator {

EstimatedState::EstimatedState(core::ConfigNode config, core::EventHubPtr eventHub) :
    core::EventPublisher(eventHub, "EstimatedState"),
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
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estimatedPosition = position;
    }    
    publishPositionUpdate(position);
}

void EstimatedState::setEstimatedVelocity(math::Vector2 velocity)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estimatedVelocity = velocity;
    }
    publishVelocityUpdate(velocity);
}

void EstimatedState::setEstimatedLinearAcceleration(
    math::Vector3 linearAcceleration)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estimatedLinearAcceleration = linearAcceleration;
    }    
    publishLinearAccelerationUpdate(linearAcceleration);
}

void EstimatedState::setEstimatedAngularRate(
    math::Vector3 angularRate)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estimatedAngularRate = angularRate;
    }
    publishAngularRateUpdate(angularRate);
}

void EstimatedState::setEstimatedOrientation(
    math::Quaternion orientation)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estimatedOrientation = orientation;
    }
    publishOrientationUpdate(orientation);
}

void EstimatedState::setEstimatedDepth(double depth)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estimatedDepth = depth;
    }
    publishDepthUpdate(depth);
}

void EstimatedState::setEstimatedDepthDot(double depthDot)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estimatedDepthDot = depthDot;
    }
    publishDepthDotUpdate(depthDot);
}

void EstimatedState::addObstacle(std::string name, ObstaclePtr obstacle)
{
    std::cout << "EstimatedState::addObstacle - NOT YET IMPLEMENTED" << std::endl;
}

math::Vector2 EstimatedState::getObstaclePosition(std::string name)
{
    return math::Vector2::ZERO;
}

double EstimatedState::getObstacleDepth(std::string name)
{
    return 0;
}


void EstimatedState::publishPositionUpdate(const math::Vector2& position)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = position;
    publish(estimator::IStateEstimator::ESTIMATED_VELOCITY_UPDATE, event);
}
void EstimatedState::publishVelocityUpdate(const math::Vector2& velocity)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = velocity;
    publish(estimator::IStateEstimator::ESTIMATED_VELOCITY_UPDATE, event);
}
void EstimatedState::publishLinearAccelerationUpdate(const math::Vector3& linearAcceleration)
{
    math::Vector3EventPtr event(new math::Vector3Event());
    event->vector3 = linearAcceleration;
    publish(estimator::IStateEstimator::ESTIMATED_VELOCITY_UPDATE, event);
}
void EstimatedState::publishAngularRateUpdate(const math::Vector3& angularRate)
{
    math::Vector3EventPtr event(new math::Vector3Event());
    event->vector3 = angularRate;
    publish(estimator::IStateEstimator::ESTIMATED_VELOCITY_UPDATE, event);
}
void EstimatedState::publishOrientationUpdate(const math::Quaternion& orientation)
{
    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = orientation;
    publish(estimator::IStateEstimator::ESTIMATED_ORIENTATION_UPDATE, event);
}
void EstimatedState::publishDepthUpdate(const double& depth)
{
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depth;
    publish(estimator::IStateEstimator::ESTIMATED_DEPTH_UPDATE, event);
}
void EstimatedState::publishDepthDotUpdate(const double& depthDot)
{
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depthDot;
    publish(estimator::IStateEstimator::ESTIMATED_DEPTH_UPDATE, event);
}

} // namespace estimator
} // namespace ram
