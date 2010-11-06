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
    updateConnection_IMU(core::EventConnectionPtr()),
    updateConnection_DVL(core::EventConnectionPtr()),
    updateConnection_DepthSensor(core::EventConnectionPtr()),
    updateConnection_Sonar(core::EventConnectionPtr()),
    updateConnection_Vision(core::EventConnectionPtr()),
    estimatedState(EstimatedStatePtr(new EstimatedState(
                                         config["EstimatedState"],
                                         eventHub)))
{


}

StateEstimatorBase::StateEstimatorBase(core::ConfigNode config,
                                       core::SubsystemList deps) :
    IStateEstimator(config["name"].asString(),
        core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    updateConnection_IMU(core::EventConnectionPtr()),
    updateConnection_DVL(core::EventConnectionPtr()),
    updateConnection_DepthSensor(core::EventConnectionPtr()),
    updateConnection_Sonar(core::EventConnectionPtr()),
    updateConnection_Vision(core::EventConnectionPtr()),
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

void StateEstimatorBase::addObstacle(std::string name, ObstaclePtr obstacle)
{
    estimatedState->addObstacle(name,obstacle);
}

math::Vector2 StateEstimatorBase::getObstaclePosition(std::string name)
{
    return estimatedState->getObstaclePosition(name);
}

double StateEstimatorBase::getObstacleDepth(std::string name)
{
    return estimatedState->getObstacleDepth(name);
}

} // namespace estimation
} // namespace ram



