/*
 * Copyright (C) 2007 Robotics  Maryland
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
#include "core/include/Events.h"

#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Events.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::control::CombineController, CombineController);

// Create category for logging
static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Controller"));

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

    m_initializationPause = config["initializationPause"].asDouble(0);

    LOGGER.infoStream() << "ForceOut TorqueOut";
}

void CombineController::doUpdate(const double& timestep,
                                 math::Vector3& translationalForceOut,
                                 math::Vector3& rotationalTorqueOut)
{

    // Don't do anything during the initialization pause
    if (m_initializationPause > 0)
    {
        m_initializationPause -= timestep;

        // so at the end of the initialization pause we
        // are steady

        if(m_initHoldDepth)
            holdCurrentDepth();
        if(m_initHoldHeading)
            holdCurrentHeading();
        if(m_initHoldPosition)
            holdCurrentPosition();

    	return;
    }

    // Update controllers
    math::Vector3 inPlaneControlForce(
        m_transController->translationalUpdate(
            timestep, m_stateEstimator, m_desiredState));

    math::Vector3 depthControlForce(
        m_depthController->depthUpdate(
            timestep, m_stateEstimator, m_desiredState));

    math::Vector3 rotControlTorque(
        m_rotController->rotationalUpdate(
            timestep, m_stateEstimator, m_desiredState));
    
    // Combine into desired rotational control and torque
    translationalForceOut = inPlaneControlForce + depthControlForce;
    rotationalTorqueOut = rotControlTorque;

    LOGGER.infoStream() << translationalForceOut[0] << " "
                        << translationalForceOut[1] << " "
                        << translationalForceOut[2] << " " 
                        << rotationalTorqueOut[0] << " "
                        << rotationalTorqueOut[1] << " "
                        << rotationalTorqueOut[2];

    // publish the individual control signals
    math::Vector3EventPtr dEvent = math::Vector3EventPtr(new math::Vector3Event());
    dEvent->vector3 = depthControlForce;

    math::Vector3EventPtr tEvent = math::Vector3EventPtr(new math::Vector3Event());
    tEvent->vector3 = inPlaneControlForce;
    
    math::Vector3EventPtr oEvent = math::Vector3EventPtr(new math::Vector3Event());
    oEvent->vector3 = rotControlTorque;

    publish(IController::DEPTH_CONTROL_SIGNAL_UPDATE, dEvent);
    publish(IController::TRANSLATION_CONTROL_SIGNAL_UPDATE, tEvent);
    publish(IController::ORIENTATION_CONTROL_SIGNAL_UPDATE, oEvent);
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
