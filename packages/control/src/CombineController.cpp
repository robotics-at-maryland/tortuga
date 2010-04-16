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
//#include "core/include/TimeVal.h"

#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "math/include/Events.h"
//#include "imu/include/imuapi.h"

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
{ init(config); }

    
CombineController::CombineController(core::ConfigNode config,
                                     core::SubsystemList deps) :
    ControllerBase(config, deps)
{ init(config); }

CombineController::~CombineController()
{ unbackground(true); }

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

// void CombineController::setVelocity(math::Vector2 velocity)
// { m_transController->setVelocity(velocity); }

// math::Vector2 CombineController::getVelocity()
// { return m_transController->getVelocity(); }
    
// void CombineController::setSpeed(double speed)
// { m_transController->setSpeed(speed); }

// void CombineController::setSidewaysSpeed(double speed)
// { m_transController->setSidewaysSpeed(speed); }

// double CombineController::getSpeed()
// { return m_transController->getSpeed(); }

// double CombineController::getSidewaysSpeed()
// { return m_transController->getSidewaysSpeed(); }

// void CombineController::holdCurrentPosition()
// { m_transController->holdCurrentPosition(); }

// void CombineController::setDesiredVelocity(math::Vector2 velocity)
// { 
//     m_transController->setDesiredVelocity(velocity);
//     desiredState->setDesiredVelocity(velocity);
// }
    

// void CombineController::setDesiredPosition(math::Vector2 position)
// { m_transController->setDesiredPosition(position); }
 
 
// void CombineController::setDesiredPositionAndVelocity(math::Vector2 position,
//                                                       math::Vector2 velocity)
// { m_transController->setDesiredPositionAndVelocity(position,velocity); }


// math::Vector2 CombineController::getDesiredVelocity()
// { return m_transController->getDesiredVelocity(); }


// math::Vector2 CombineController::getDesiredPosition()
// { return m_transController->getDesiredPosition(); }

// bool CombineController::atPosition()
// { return m_transController->atPosition(); }

// bool CombineController::atVelocity()
// { return m_transController->atVelocity(); }
    
// // Depth controller methods
// void CombineController::setDepth(double depth)
// { m_depthController->setDepth(depth); }
    
// double CombineController::getDepth()
// { return m_depthController->getDepth(); }

// double CombineController::getEstimatedDepth()
// { return m_depthController->getEstimatedDepth(); }

// double CombineController::getEstimatedDepthDot()
// { return m_depthController->getEstimatedDepthDot(); }

// bool CombineController::atDepth()
// { return m_depthController->atDepth(); }

// void CombineController::holdCurrentDepth()
// { m_depthController->holdCurrentDepth(); }
    
// // Rotational controller methods
// void CombineController::rollVehicle(double degrees)
// {
//     m_rotController->rollVehicle(degrees);
//     newDesiredOrientationSet(m_rotController->getDesiredOrientation());
//  }

// void CombineController::pitchVehicle(double degrees)
// { 
//     m_rotController->pitchVehicle(degrees); 
//     newDesiredOrientationSet(m_rotController->getDesiredOrientation());
// }

// void CombineController::yawVehicle(double degrees)
// { 
//     m_rotController->yawVehicle(degrees);
//     newDesiredOrientationSet(m_rotController->getDesiredOrientation());
// }

// math::Quaternion CombineController::getDesiredOrientation()
// { return m_rotController->getDesiredOrientation(); }

// void CombineController::setDesiredOrientation(math::Quaternion newOrientation)
// { 
//     m_rotController->setDesiredOrientation(newOrientation);
//     newDesiredOrientationSet(m_rotController->getDesiredOrientation());
// }
    
// bool CombineController::atOrientation()
// { return m_rotController->atOrientation(); }

// void CombineController::holdCurrentHeading()
// { 
//     m_rotController->holdCurrentHeading();
//     newDesiredOrientationSet(m_rotController->getDesiredOrientation());
// }

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

ITranslationalControllerPtr CombineController::getTranslationalController()
{ return m_transController; }
   
IDepthControllerPtr CombineController::getDepthController()
{ return m_depthController; }
    
IRotationalControllerPtr CombineController::getRotationalController()
{ return m_rotController; }

// void CombineController::setBuoyantTorqueCorrection(double x, double y, double z){}

// void CombineController::setHeading(double degrees){}

// double CombineController::getHeading()
// { return 0; }



    
} // namespace control
} // namespace ram
