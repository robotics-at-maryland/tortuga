/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/CombineController.cpp
 */


// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "control/include/CombineController.h"

#include "vehicle/include/IVehicle.h"

#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"
//#include "core/include/TimeVal.h"

//#include "math/include/Helpers.h"
//#include "math/include/Vector3.h"
//#include "math/include/Events.h"
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
    IController(config["name"].asString())//,
//    m_vehicle(vehicle)
{   
//    init(config); 
}

CombineController::CombineController(core::ConfigNode config,
                               core::SubsystemList deps) :
    IController(config["name"].asString(),
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps))//,
//    m_vehicle(core::Subsystem::getSubsystemOfType<vehicle::IVehicle>(deps)),
{
//    init(config);
}

CombineController::~CombineController()
{
    unbackground(true);
}


// Translational controller methods
void CombineController::setSpeed(double speed)
{
    m_transController->setSpeed(speed);
}

void CombineController::setSidewaysSpeed(double speed)
{
    m_transController->setSidewaysSpeed(speed);
}

double CombineController::getSpeed()
{
    return m_transController->getSpeed();
}

double CombineController::getSidewaysSpeed()
{
    return m_transController->getSidewaysSpeed();
}
    
// Depth controller methods
void CombineController::setDepth(double depth)
{
    m_depthController->setDepth(depth);
}

double CombineController::getDepth()
{
    return m_depthController->getDepth();
}

double CombineController::getEstimatedDepth()
{
    return m_depthController->getEstimatedDepth();
}

double CombineController::getEstimatedDepthDot()
{
    return m_depthController->getEstimatedDepthDot();
}

// Rotational controller methods
void CombineController::rollVehicle(double degrees)
{
    m_rotController->rollVehicle(degrees);
}

void CombineController::pitchVehicle(double degrees)
{
    m_rotController->pitchVehicle(degrees);
}

void CombineController::yawVehicle(double degrees)
{
    m_rotController->yawVehicle(degrees);
}

math::Quaternion CombineController::getDesiredOrientation()
{
    return m_rotController->getDesiredOrientation();
}

void CombineController::setDesiredOrientation(math::Quaternion newOrientation)
{
    m_rotController->setDesiredOrientation(newOrientation);
}
    
bool CombineController::atOrientation()
{
    return m_rotController->atOrientation();
}

void CombineController::holdCurrentHeading()
{
    m_rotController->holdCurrentHeading();
}

// Misc Methods
bool CombineController::atDepth()
{
    return m_depthController->atDepth();
}


void CombineController::update(double timestep)
{
    // Get vehicle state
    math::Vector3 linearAcceleration(m_vehicle->getLinearAcceleration());
    math::Quaternion orientation(m_vehicle->getOrientation());
    math::Vector3 angularRate(m_vehicle->getAngularRate());
    double depth = m_vehicle->getDepth();
    
    // Update controllers
    math::Vector3 inPlaneControlForce(
        m_transController->translationalUpdate(orientation));
    math::Vector3 depthControlForce(
        m_depthController->depthUpdate(depth, orientation));
    math::Vector3 rotControlTorque(
        m_rotController->rotationalUpdate(orientation, angularRate));
    
    // Combine into desired rotational control and torque
    math::Vector3 translationalForce(inPlaneControlForce + depthControlForce);
    math::Vector3 rotationalTorque(rotControlTorque);

    // Actually set motor values
    m_vehicle->applyForcesAndTorques(translationalForce, rotationalTorque);

    /// TODO: Figure out how to factor out base functionality
}
        
} // namespace control
} // namespace ram
