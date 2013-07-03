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
    kpvx = config["kpvx"].asDouble(0);
    kivx = config["kivx"].asDouble(0);
    kdvx = config["kdvx"].asDouble(0);
    kpvy = config["kpvy"].asDouble(0);
    kivy = config["kivy"].asDouble(0);
    kdvy = config["kdvy"].asDouble(0);
    kpvz = config["kpvz"].asDouble(0);
    kivz = config["kivz"].asDouble(0);
    kdvz = config["kdvz"].asDouble(0);
    intTermxy = math::Vector2::ZERO;
    intTermz = 0;
    lastDV = 0;
    vConx = false;
    vCony = false;
    vConz = false;

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

    //if visual servoing is enabled, fix the desired state for that DOF so that
    //the integral terms don't go insane
    math::Vector2 dxy = m_desiredState->getDesiredPosition();
    math::Vector2 estxy = m_stateEstimator->getEstimatedPosition();
    double ed = m_stateEstimator->getEstimatedDepth();
    double dd = m_desiredState->getDesiredDepth();
    if(m_desiredState->vx == true)
    {
        dxy.x = estxy.x;
    }
    if(m_desiredState->vy == true)
    {
        dxy.y = estxy.y;
    }
    if(m_desiredState->vz == true)
    {
        dd = ed;
    }
    m_desiredState->setDesiredPosition(dxy);
    m_desiredState->setDesiredDepth(dd);

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

        //moved
        //if(m_desiredState->vz == true && vConz == false)
        //{
        // intTermz = m_depthController->getISum(); //steal the positional controllers z integral term
            //}
        
        //begin the velocity controller
        //yes this is the wrong place, but the system doesn't really provide effective support for controller switching
        //let alone using multiple controllers with different types of states simultaneously
        math::Vector2 eVelocity = m_stateEstimator->getEstimatedVelocity();
        math::Vector2 dVelocity = m_desiredState->getDesiredVelocity();
        math::Vector3 eAccel = m_stateEstimator->getEstimatedLinearAcceleration();
        math::Vector2 dAccelxy = m_desiredState->getDesiredAccel();
        //double dAccel = m_desiredState->getDesiredDepthAccel();
        double eRate = m_stateEstimator->getEstimatedDepthRate();
        math::Vector3 toRot(eVelocity.x,eVelocity.y,eRate);
        math::Vector3 rot = m_stateEstimator->getEstimatedOrientation() * toRot;
        double dRate = m_desiredState->getDesiredDepthRate();
        eVelocity.x = rot.x;
        eVelocity.y = rot.y;
        eRate = rot.z;
        //double eAccel = (eRate - lastDV)/timestep;
        lastDV = eRate;
        double fX, fY,fZ;
        math::Vector2 errVxy = dVelocity - eVelocity;
        double errVz = eRate - dRate;
        intTermxy = intTermxy + errVxy*timestep;
        intTermz = intTermz + errVz*timestep;


        //if about to turn on one of the visual servoing controllers, stabilize self
        if((vConx == false && m_desiredState->vx == true) || (vCony == false && m_desiredState->vy == true) || (vConz == false && m_desiredState->vz == true))
        {

                holdCurrentDepth();
                holdCurrentHeading();
                holdCurrentPosition();
                if(m_desiredState->vx == true)
                {
                    intTermxy.x = 0;
                }
                if(m_desiredState->vy == true)
                {
                    intTermxy.y = 0;
                }
                if(m_desiredState->vz == true)
                {
                    intTermz = m_depthController->getISum(); //steal the positional controllers z integral term
                }
        }
       //if turning off visual servoing, hold the current position for all axes so the position controllers are ready
        if((vConx == true && m_desiredState->vx == false) || (vCony == true && m_desiredState->vy == false) || (vConz == true && m_desiredState->vz == false))
        {
                holdCurrentDepth();
                holdCurrentHeading();
                holdCurrentPosition();
        }
        vConx = m_desiredState->vx;
        vCony = m_desiredState->vy;
        vConz = m_desiredState->vz;
        if(vConx == true)
        {
            double eXv = errVxy.x;
            double eXa = eAccel.x;
            double eXi = intTermxy.x;
            fX = kpvx * eXv + kivx*eXi + kdvx*eXa;
            translationalForceOut.x = fX;
        }
        if(vCony == true)
        {
            double eYv = errVxy.y;
            double eYa = eAccel.y;
            double eYi = intTermxy.y;
            fY = kpvy * eYv + kivy*eYi + kdvy*eYa;
            translationalForceOut.y = fY;
        }
        if(vConz == true)
        {
            double eZv = errVz;
            double eZa = eAccel.z;
            double eZi = intTermz;
            fZ = kpvz * eZv + kivz*eZi + kdvz*eZa;
            translationalForceOut.z = fZ;
        }


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
