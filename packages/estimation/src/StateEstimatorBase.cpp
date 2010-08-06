/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/StateEstimatorBase.cpp
 */


// Library Includes
#include <iostream>

// Project Includes
#include "estimation/include/StateEstimatorBase.h"

namespace ram {
namespace estimation {

StateEstimatorBase::StateEstimatorBase(
    core::ConfigNode config,
    core::EventHubPtr eventHub,
    vehicle::IVehiclePtr vehicle) :
    IStateEstimator(),
    updateConnection_IMU(core::EventConnectionPtr()),
    updateConnection_DVL(core::EventConnectionPtr()),
    updateConnection_DepthSensor(core::EventConnectionPtr()),
    updateConnection_Sonar(core::EventConnectionPtr()),
    updateConnection_Vision(core::EventConnectionPtr()),
    initConnection_IMU(core::EventConnectionPtr()),
    initConnection_DVL(core::EventConnectionPtr()),
    initConnection_DepthSensor(core::EventConnectionPtr()),
    estimatedState(EstimatedStatePtr(new EstimatedState(
                                         config["EstimatedState"],
                                         eventHub)))
{


}

math::Vector2 StateEstimatorBase::getEstimatedPosition()
{
    return estimatedState->getEstPosition();
}

math::Vector2 StateEstimatorBase::getEstimatedVelocity()
{
    return estimatedState->getEstVelocity();
}

math::Vector3 StateEstimatorBase::getEstimatedLinearAcceleration()
{
    return estimatedState->getEstLinearAccel();
}

math::Vector3 StateEstimatorBase::getEstimatedAngularRate()
{
    return estimatedState->getEstAngularRate();
}

math::Quaternion StateEstimatorBase::getEstimatedOrientation()
{
    return estimatedState->getEstOrientation();
}

double StateEstimatorBase::getEstimatedDepth()
{
    return estimatedState->getEstDepth();
}

double StateEstimatorBase::getEstimatedDepthDot()
{
    return estimatedState->getEstDepthDot();
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
