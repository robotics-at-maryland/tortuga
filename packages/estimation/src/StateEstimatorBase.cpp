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
    estimatedState(EstimatedStatePtr(new EstimatedState(
                                         config["EstimatedState"],
                                         eventHub)))
{


}

StateEstimatorBase::StateEstimatorBase(core::ConfigNode config,
                                       core::SubsystemList deps) :
    IStateEstimator(config["name"].asString(),
        core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    estimatedState(EstimatedStatePtr(
                       new EstimatedState(
                           config["EstimatedState"],
                           core::Subsystem::getSubsystemOfType<core::EventHub>(deps))))
{


}

math::Vector2 StateEstimatorBase::getEstimatedPosition()
{
    return estimatedState->getEstimatedPosition();
}

math::Vector2 StateEstimatorBase::getEstimatedVelocity()
{
    return estimatedState->getEstimatedVelocity();
}

math::Vector3 StateEstimatorBase::getEstimatedLinearAcceleration()
{
    return estimatedState->getEstimatedLinearAccel();
}

math::Vector3 StateEstimatorBase::getEstimatedAngularRate()
{
    return estimatedState->getEstimatedAngularRate();
}

math::Quaternion StateEstimatorBase::getEstimatedOrientation()
{
    return estimatedState->getEstimatedOrientation();
}

double StateEstimatorBase::getEstimatedDepth()
{
    return estimatedState->getEstimatedDepth();
}

double StateEstimatorBase::getEstimatedDepthRate()
{
    return estimatedState->getEstimatedDepthRate();
}

void StateEstimatorBase::addObstacle(Obstacle::ObstacleType name, ObstaclePtr obstacle)
{
    estimatedState->addObstacle(name,obstacle);
}

math::Vector2 StateEstimatorBase::getObstaclePosition(Obstacle::ObstacleType name)
{
    return estimatedState->getObstaclePosition(name);
}

double StateEstimatorBase::getObstacleDepth(Obstacle::ObstacleType  name)
{
    return estimatedState->getObstacleDepth(name);
}

double StateEstimatorBase::getEstimatedMass()
{
    return estimatedState->getEstimatedMass();
}
ObstaclePtr StateEstimatorBase::getObstacle(Obstacle::ObstacleType name)
{
    return estimatedState->getObstacle(name);
}

math::Vector3 StateEstimatorBase::getEstimatedThrusterForces()
{
    return estimatedState->getEstimatedThrusterForces();
}

math::Vector3 StateEstimatorBase::getEstimatedThrusterTorques()
{
    return estimatedState->getEstimatedThrusterForces();
}

} // namespace estimation
} // namespace ram



