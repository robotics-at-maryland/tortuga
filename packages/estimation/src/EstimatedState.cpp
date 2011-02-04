/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/EstimatedState.cpp
 */

// STD Includes
#include <iostream>

// Library Includes

// Package Includes
#include "estimation/include/EstimatedState.h"
#include "math/include/Events.h"
#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace estimation {

EstimatedState::EstimatedState(core::ConfigNode config, core::EventHubPtr eventHub) :
    core::EventPublisher(eventHub, "EstimatedState"),
    m_estPosition(math::Vector2::ZERO),
    m_estVelocity(math::Vector2::ZERO),
    m_estLinearAccel(math::Vector3::ZERO),
    m_estAngularRate(math::Vector3::ZERO),
    m_estOrientation(math::Quaternion::IDENTITY),
    m_estDepth(0),
    m_estDepthRate(0),
    m_estThrusterForces(math::Vector3::ZERO), 
    m_estThrusterTorques(math::Vector3::ZERO),
    m_estMass(0)
{
    
}

math::Vector2 EstimatedState::getEstimatedPosition()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_estPosition;
}

math::Vector2 EstimatedState::getEstimatedVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_estVelocity;
}

math::Vector3 EstimatedState::getEstimatedLinearAccel()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_estLinearAccel;
}

math::Vector3 EstimatedState::getEstimatedAngularRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_estAngularRate;
}

math::Quaternion EstimatedState::getEstimatedOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_estOrientation;
}

double EstimatedState::getEstimatedDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_estDepth;
}

double EstimatedState::getEstimatedDepthRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_estDepthRate;
}

math::Vector3 EstimatedState::getEstimatedThrusterForces()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_estThrusterForces;
}

math::Vector3 EstimatedState::getEstimatedThrusterTorques()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_estThrusterTorques;
}

double EstimatedState::getEstimatedMass()
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    return m_estMass;
}

void EstimatedState::setEstimatedPosition(math::Vector2 position)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_estPosition = position;
    }    
    publishPositionUpdate(position);
}

void EstimatedState::setEstimatedVelocity(math::Vector2 velocity)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_estVelocity = velocity;
    }
    publishVelocityUpdate(velocity);
}

void EstimatedState::setEstimatedLinearAccel(
    math::Vector3 linearAccel)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_estLinearAccel = linearAccel;
    }    
    publishLinearAccelUpdate(linearAccel);
}

void EstimatedState::setEstimatedAngularRate(
    math::Vector3 angularRate)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_estAngularRate = angularRate;
    }
    publishAngularRateUpdate(angularRate);
}

void EstimatedState::setEstimatedOrientation(
    math::Quaternion orientation)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_estOrientation = orientation;
    }
    publishOrientationUpdate(orientation);
}

void EstimatedState::setEstimatedDepth(double depth)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_estDepth = depth;
    }
    publishDepthUpdate(depth);
}

void EstimatedState::setEstimatedDepthRate(double depthRate)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_estDepthRate = depthRate;
    }
    publishDepthRateUpdate(depthRate);
}

void EstimatedState::setEstimatedThrust(math::Vector3 forces,
                                        math::Vector3 torques)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_estThrusterForces = forces;
    m_estThrusterTorques = torques;
}

void EstimatedState::setEstimatedMass(double mass)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_estMass = mass;
}

void EstimatedState::addObstacle(std::string name, ObstaclePtr obstacle)
{
    throw std::runtime_error("EstimatedState::addObstacle - NOT YET IMPLEMENTED");
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
void EstimatedState::publishDepthRateUpdate(const double& depthRate)
{
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depthRate;
    publish(estimation::IStateEstimator::ESTIMATED_DEPTH_UPDATE, event);
}
void EstimatedState::publishThrustUpdate(const math::Vector3& forces,
                                         const math::Vector3& torques)
{
    math::Vector3EventPtr fEvent(new math::Vector3Event());
    fEvent->vector3 = forces;
    publish(estimation::IStateEstimator::ESTIMATED_FORCES_UPDATE, fEvent);

    math::Vector3EventPtr tEvent(new math::Vector3Event());
    tEvent->vector3 = torques;
    publish(estimation::IStateEstimator::ESTIMATED_TORQUES_UPDATE, tEvent);
}

} // namespace estimation
} // namespace ram
