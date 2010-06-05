/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/CombineController.cpp
 */

// STD Includes
#include <cmath>
#include <cstdio>
#include <iostream>

#ifdef RAM_WINDOWS
#define M_PI 3.14159265358979323846
#endif

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "control/include/CombineController.h"
#include "control/include/ITranslationalController.h"
#include "control/include/IDepthController.h"
#include "control/include/IRotationalController.h"
#include "control/include/ControllerMaker.h"
#include "control/include/DesiredState.h"

#include "vehicle/include/IVehicle.h"

#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"

#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Events.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::control::CombineController, CombineController);

// Create category for logging
static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Controller"));
//static log4cpp::Category& ADPT_LOGGER(log4cpp::Category::getInstance("AdaptCtrl"));

using namespace std;

namespace ram {
namespace control {

CombineController::CombineController(vehicle::IVehiclePtr vehicle,
                                     core::ConfigNode config) :
    ControllerBase(vehicle, config)  
{ 
    init(config);
}

CombineController::CombineController(core::ConfigNode config,
                                     core::SubsystemList deps) :
    ControllerBase(config, deps)
{
    init(config);
}

CombineController::~CombineController()
{
    unbackground(true);
}

void CombineController::init(core::ConfigNode config)
{
    // Create In plane controller
    core::ConfigNode node(config["TranslationalController"]);
    m_transController = TranslationalControllerImpMaker::newObject(node);
    
    // Create depth controller
    node = config["DepthController"];
    m_depthController = DepthControllerImpMaker::newObject(node);
    
    // Create rotational controller
    node = config["RotationalController"];
    m_rotController = RotationalControllerImpMaker::newObject(node);
}

void CombineController::doUpdate(const double& timestep,
                                 const math::Vector3& linearAcceleration,
                                 const math::Quaternion& orientation,
                                 const math::Vector3& angularRate,
                                 const double& depth,
                                 const math::Vector2& position,
                                 const math::Vector2& velocity,
                                 math::Vector3& translationalForceOut,
                                 math::Vector3& rotationalTorqueOut)
{
    // Update controllers
    math::Vector3 inPlaneControlForce(
        m_transController->translationalUpdate(timestep, linearAcceleration,
                                               orientation, position,
                                               velocity, desiredState));

    math::Vector3 depthControlForce(
        m_depthController->depthUpdate(timestep, depth, orientation, desiredState));

    math::Vector3 rotControlTorque(
        m_rotController->rotationalUpdate(timestep, orientation, angularRate, desiredState));
    
    // Combine into desired rotational control and torque
    translationalForceOut = inPlaneControlForce + depthControlForce;
    rotationalTorqueOut = rotControlTorque;
}

void CombineController::setVelocity(math::Vector2 velocity)
{
    setDesiredVelocity(velocity,IController::BODY_FRAME);
    m_transController->setControlMode(ControlMode::VELOCITY);
}

void CombineController::setSpeed(double speed)
{
    if(speed > 5)
        speed = 5;
    else if(speed < -5)
        speed = -5;

    double sidewaysSpeed = desiredState->getDesiredVelocity()[1];

    setDesiredVelocity(math::Vector2(speed,sidewaysSpeed), IController::BODY_FRAME);
    m_transController->setControlMode(ControlMode::VELOCITY);
}

void CombineController::setSidewaysSpeed(double speed)
{
    if(speed > 5)
        speed = 5;
    else if(speed < -5)
        speed = -5;

    double forewardSpeed = desiredState->getDesiredVelocity()[0];

    setDesiredVelocity(math::Vector2(forewardSpeed,speed), IController::BODY_FRAME);
    m_transController->setControlMode(ControlMode::VELOCITY);
}

void CombineController::setDesiredVelocity(math::Vector2 velocity, int frame)
{
    if(frame == IController::BODY_FRAME)
        velocity = nRb(m_vehicle->getOrientation().getYaw().valueRadians())*velocity;
    desiredState->setDesiredVelocity(velocity);
    m_transController->setControlMode(ControlMode::VELOCITY);
}

void CombineController::setDesiredPosition(math::Vector2 position, int frame)
{
    if(frame == IController::BODY_FRAME)
        position = nRb(m_vehicle->getOrientation().getYaw().valueRadians())*position;
    desiredState->setDesiredPosition(position);
    m_transController->setControlMode(ControlMode::POSITION);
}

void CombineController::setDesiredPositionAndVelocity(math::Vector2 position,
                                                      math::Vector2 velocity)
{
    desiredState->setDesiredVelocity(velocity);
    desiredState->setDesiredPosition(position);
    m_transController->setControlMode(ControlMode::POSITIONANDVELOCITY);
}


ITranslationalControllerPtr CombineController::getTranslationalController()
{ 
    return m_transController;
}

IDepthControllerPtr CombineController::getDepthController()
{
    return m_depthController; 
}

IRotationalControllerPtr CombineController::getRotationalController()
{
    return m_rotController; 
}

} // namespace control
} // namespace ram
