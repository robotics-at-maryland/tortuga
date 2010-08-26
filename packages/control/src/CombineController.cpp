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

using namespace std;

namespace ram {
namespace control {

CombineController::CombineController(core::EventHubPtr eventHub,
                                     vehicle::IVehiclePtr vehicle,
                                     estimation::IStateEstimatorPtr estimator,
                                     core::ConfigNode config) :
    ControllerBase(eventHub, vehicle, estimator, config),
    m_transController(ITranslationalControllerImpPtr()),
    m_depthController(IDepthControllerImpPtr()),
    m_rotController(IRotationalControllerImpPtr()),
    m_initializationPause(0)
{ 
    init(config);
}

CombineController::CombineController(core::ConfigNode config,
                                     core::SubsystemList deps) :
    ControllerBase(config, deps),
    m_transController(ITranslationalControllerImpPtr()),
    m_depthController(IDepthControllerImpPtr()),
    m_rotController(IRotationalControllerImpPtr()),
    m_initializationPause(0)
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

    m_initializationPause = config["InitializationPause"].asDouble(0);
}

void CombineController::doUpdate(const double& timestep,
                                 math::Vector3& translationalForceOut,
                                 math::Vector3& rotationalTorqueOut)
{

    if (m_initializationPause > 0)
    {
        m_initializationPause -= timestep;
    	return;
    }

    // Update controllers
    math::Vector3 inPlaneControlForce(
        m_transController->translationalUpdate(timestep,
                                               m_stateEstimator,
                                               m_desiredState));

    math::Vector3 depthControlForce(
        m_depthController->depthUpdate(timestep,
                                       m_stateEstimator,
                                       m_desiredState));

    math::Vector3 rotControlTorque(
        m_rotController->rotationalUpdate(timestep,
                                          m_stateEstimator,
                                          m_desiredState));
    
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
    double sidewaysSpeed = 0;

    // clamp speeds
    if(speed > 5)
        speed = 5;
    else if(speed < -5)
        speed = -5;

    // when going between control modes, reset the sideways speed to 0
    if(m_transController->getControlMode() == ControlMode::OPEN_LOOP)
        sidewaysSpeed = getDesiredVelocity(IController::INERTIAL_FRAME)[1];

    /*  Store velocity as if the robots frame is the inertial frame.  
        In other words, the robot should always travel at this velocity 
        relative to itself.  Doing this, the robot's actual velocity in
        the true inertial frame will change as its orientation changes. */
    setDesiredVelocity(math::Vector2(speed,sidewaysSpeed), IController::INERTIAL_FRAME);
    m_transController->setControlMode(ControlMode::OPEN_LOOP);
}

void CombineController::setSidewaysSpeed(double speed)
{
    double forwardSpeed = 0;
    
    // clamp speeds
    if(speed > 5)
        speed = 5;
    else if(speed < -5)
        speed = -5;

    // when going between control modes, reset the velocity
    if(m_transController->getControlMode() == ControlMode::OPEN_LOOP)
        forwardSpeed = getDesiredVelocity(IController::INERTIAL_FRAME)[0];

    /*  Store velocity as if the robots frame is the inertial frame.  
        In other words, the robot should always travel at this velocity 
        relative to itself.  Doing this, the robot's actual velocity in
        the true inertial frame will change as its orientation changes. */
    setDesiredVelocity(math::Vector2(forwardSpeed,speed), IController::INERTIAL_FRAME);
    m_transController->setControlMode(ControlMode::OPEN_LOOP);
}

void CombineController::setDesiredVelocity(math::Vector2 velocity, int frame)
{
    if(frame == IController::BODY_FRAME)
        velocity = math::nRb(m_desiredState->getDesiredOrientation().getYaw().valueRadians())*velocity;
    m_desiredState->setDesiredVelocity(velocity);
    m_desiredState->setDesiredPosition(m_stateEstimator->getEstimatedPosition());
    m_transController->setControlMode(ControlMode::VELOCITY);
}

void CombineController::setDesiredPosition(math::Vector2 position, int frame)
{
    if(frame == IController::BODY_FRAME)
        position = math::nRb(m_stateEstimator->getEstimatedOrientation().getYaw().valueRadians())*position;
    m_desiredState->setDesiredPosition(position);
    m_desiredState->setDesiredVelocity(math::Vector2::ZERO);
    m_transController->setControlMode(ControlMode::POSITION);
}

void CombineController::setDesiredPositionAndVelocity(math::Vector2 position,
                                                      math::Vector2 velocity)
{
    m_desiredState->setDesiredVelocity(velocity);
    m_desiredState->setDesiredPosition(position);
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
