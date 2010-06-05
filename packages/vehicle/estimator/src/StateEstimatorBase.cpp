/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/src/StateEstimatorBase.cpp
 */


// Library Includes
#include <iostream>

// Project Includes
#include "vehicle/estimator/include/StateEstimatorBase.h"

namespace ram {
namespace estimator {

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
    estimatedState(EstimatedStatePtr(new EstimatedState()))
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
    return estimatedState->getEstimatedLinearAcceleration();
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

double StateEstimatorBase::getEstimatedDepthDot()
{
    return estimatedState->getEstimatedDepthDot();
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

} // namespace estimator
} // namespace ram
