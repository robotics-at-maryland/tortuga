/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/StateEstimatorBase.cpp
 */

// STD Includes
#include <iostream>

// Library Includes

// Project Includes
#include "estimation/include/StateEstimatorBase.h"

namespace ram {
namespace estimation {

StateEstimatorBase::StateEstimatorBase(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    IStateEstimator(config["name"].asString(), eventHub),
    m_estimatedState(EstimatedStatePtr(new EstimatedState(
                                         config["EstimatedState"],
                                         eventHub)))
{


}

StateEstimatorBase::StateEstimatorBase(core::ConfigNode config,
                                       core::SubsystemList deps) :
    IStateEstimator(config["name"].asString(),
        core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    m_estimatedState(EstimatedStatePtr(
                       new EstimatedState(
                           config["EstimatedState"],
                           core::Subsystem::getSubsystemOfType<core::EventHub>(deps))))
{


}

math::Vector2 StateEstimatorBase::getEstimatedPosition()
{
    return m_estimatedState->getEstimatedPosition();
}

math::Vector2 StateEstimatorBase::getEstimatedVelocity()
{
    return m_estimatedState->getEstimatedVelocity();
}

math::Vector3 StateEstimatorBase::getEstimatedLinearAcceleration()
{
    return m_estimatedState->getEstimatedLinearAccel();
}

math::Vector3 StateEstimatorBase::getEstimatedAngularRate()
{
    return m_estimatedState->getEstimatedAngularRate();
}

math::Quaternion StateEstimatorBase::getEstimatedOrientation()
{
    return m_estimatedState->getEstimatedOrientation();
}

double StateEstimatorBase::getEstimatedDepth()
{
    return m_estimatedState->getEstimatedDepth();
}

double StateEstimatorBase::getEstimatedDepthRate()
{
    return m_estimatedState->getEstimatedDepthRate();
}

double StateEstimatorBase::getEstimatedBottomRange()
{
    return m_estimatedState->getEstimatedBottomRange();
}

void StateEstimatorBase::addObstacle(Obstacle::ObstacleType name,
                                     ObstaclePtr obstacle)
{
    m_estimatedState->addObstacle(name,obstacle);
}

math::Vector3 StateEstimatorBase::getObstacleLocation(
    Obstacle::ObstacleType name)
{
    return m_estimatedState->getObstacleLocation(name);
}

math::Matrix3 StateEstimatorBase::getObstacleLocationCovariance(
    Obstacle::ObstacleType name)
{
    return m_estimatedState->getObstacleLocationCovariance(name);
}

math::Quaternion StateEstimatorBase::getObstacleAttackOrientation(
    Obstacle::ObstacleType name)
{
    return m_estimatedState->getObstacleAttackOrientation(name);
}

double StateEstimatorBase::getEstimatedMass()
{
    return m_estimatedState->getEstimatedMass();
}
ObstaclePtr StateEstimatorBase::getObstacle(Obstacle::ObstacleType name)
{
    return m_estimatedState->getObstacle(name);
}

math::Vector3 StateEstimatorBase::getEstimatedThrusterForces()
{
    return m_estimatedState->getEstimatedThrusterForces();
}

math::Vector3 StateEstimatorBase::getEstimatedThrusterTorques()
{
    return m_estimatedState->getEstimatedThrusterForces();
}

} // namespace estimation
} // namespace ram



