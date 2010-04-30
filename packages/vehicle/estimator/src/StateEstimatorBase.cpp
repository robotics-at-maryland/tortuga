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


void StateEstimatorBase::rawUpdate_DVL(core::EventPtr event)
{
    std::cout << "rawUpdate_DVL" << std::endl;
}

void StateEstimatorBase::rawUpdate_IMU(core::EventPtr event)
{
    std::cout << "rawUpdate_IMU" << std::endl;
}

void StateEstimatorBase::rawUpdate_DepthSensor(core::EventPtr event)
{
    std::cout << "rawUpdate_DepthSensor" << std::endl;
}

void StateEstimatorBase::update_Vision(core::EventPtr event)
{
    std::cout << "rawUpdate_Vision" << std::endl;
}

void StateEstimatorBase::update_Sonar(core::EventPtr event)
{
    std::cout << "rawUpdate_Sonar" << std::endl;
}

} // namespace estimator
} // namespace ram
