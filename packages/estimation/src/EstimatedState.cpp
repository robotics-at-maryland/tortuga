/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/EstimatedState.cpp
 */


// Library Includes

// Package Includes
#include "estimation/include/EstimatedState.h"
#include "estimation/include/Events.h"
#include "math/include/Events.h"
#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace estimation {

EstimatedState::EstimatedState(core::ConfigNode config, core::EventHubPtr eventHub) :
    core::EventPublisher(eventHub, "EstimatedState"),
    estPosition(math::Vector2::ZERO),
    estVelocity(math::Vector2::ZERO),
    estLinearAccel(math::Vector3::ZERO),
    estAngularRate(math::Vector3::ZERO),
    estOrientation(math::Quaternion::IDENTITY),
    estDepth(0),
    estDepthDot(0)
{
    
}

math::Vector2 EstimatedState::getEstPosition()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estPosition;
}

math::Vector2 EstimatedState::getEstVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estVelocity;
}

math::Vector3 EstimatedState::getEstLinearAccel()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estLinearAccel;
}

math::Vector3 EstimatedState::getEstAngularRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estAngularRate;
}

math::Quaternion EstimatedState::getEstOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estOrientation;
}

double EstimatedState::getEstDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estDepth;
}

double EstimatedState::getEstDepthDot()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estDepthDot;
}

math::Vector3 EstimatedState::getEstThrusterForces()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estThrusterForces;
}

math::Vector3 EstimatedState::getEstThrusterTorques()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return estThrusterTorques;
}

void EstimatedState::setEstPosition(math::Vector2 position)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estPosition = position;
    }    
    publishPositionUpdate(position);
}

void EstimatedState::setEstVelocity(math::Vector2 velocity)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estVelocity = velocity;
    }
    publishVelocityUpdate(velocity);
}

void EstimatedState::setEstLinearAccel(
    math::Vector3 linearAccel)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estLinearAccel = linearAccel;
    }    
    publishLinearAccelUpdate(linearAccel);
}

void EstimatedState::setEstAngularRate(
    math::Vector3 angularRate)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estAngularRate = angularRate;
    }
    publishAngularRateUpdate(angularRate);
}

void EstimatedState::setEstOrientation(
    math::Quaternion orientation)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estOrientation = orientation;
    }
    publishOrientationUpdate(orientation);
}

void EstimatedState::setEstDepth(double depth)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estDepth = depth;
    }
    publishDepthUpdate(depth);
}

void EstimatedState::setEstDepthDot(double depthDot)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estDepthDot = depthDot;
    }
    publishDepthDotUpdate(depthDot);
}

void EstimatedState::setEstThrust(math::Vector3 forces,
                                  math::Vector3 torques)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        estThrusterForces = forces;
        estThrusterTorques = torques;
    }
    publishThrustUpdate(forces, torques);
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
    publish(estimation::IStateEstimator::ESTIMATED_VELOCITY_UPDATE, event);
}
void EstimatedState::publishVelocityUpdate(const math::Vector2& velocity)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = velocity;
    publish(estimation::IStateEstimator::ESTIMATED_VELOCITY_UPDATE, event);
}
void EstimatedState::publishLinearAccelUpdate(const math::Vector3& linearAccel)
{
    math::Vector3EventPtr event(new math::Vector3Event());
    event->vector3 = linearAccel;
    publish(estimation::IStateEstimator::ESTIMATED_VELOCITY_UPDATE, event);
}
void EstimatedState::publishAngularRateUpdate(const math::Vector3& angularRate)
{
    math::Vector3EventPtr event(new math::Vector3Event());
    event->vector3 = angularRate;
    publish(estimation::IStateEstimator::ESTIMATED_VELOCITY_UPDATE, event);
}
void EstimatedState::publishOrientationUpdate(const math::Quaternion& orientation)
{
    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = orientation;
    publish(estimation::IStateEstimator::ESTIMATED_ORIENTATION_UPDATE, event);
}
void EstimatedState::publishDepthUpdate(const double& depth)
{
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depth;
    publish(estimation::IStateEstimator::ESTIMATED_DEPTH_UPDATE, event);
}
void EstimatedState::publishDepthDotUpdate(const double& depthDot)
{
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depthDot;
    publish(estimation::IStateEstimator::ESTIMATED_DEPTH_UPDATE, event);
}
void EstimatedState::publishThrustUpdate(const math::Vector3& forces,
                                         const math::Vector3& torques)
{
    estimation::ThrustUpdateEventPtr event(new estimation::ThrustUpdateEvent());
    event->forces = forces;
    event->torques = torques;
    publish(estimation::IStateEstimator::ESTIMATED_THRUST_UPDATE, event);
}

} // namespace estimation
} // namespace ram
