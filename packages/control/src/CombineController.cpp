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
    
void CombineController::setSpeed(double speed)
{
}

void CombineController::setSidewaysSpeed(double speed)
{
}

void CombineController::setDepth(double depth)
{

}

double CombineController::getSpeed()
{
    return 0;
}

double CombineController::getSidewaysSpeed()
{
    return 0;
}

double CombineController::getDepth()
{
    return 0;
}



double CombineController::getEstimatedDepth()
{
    return 0;
}

double CombineController::getEstimatedDepthDot()
{
    return 0;
}
    
void CombineController::rollVehicle(double degrees)
{

}

void CombineController::pitchVehicle(double degrees)
{

}

void CombineController::yawVehicle(double degrees)
{

}

math::Quaternion CombineController::getDesiredOrientation()
{
    return math::Quaternion::IDENTITY;
}

void CombineController::setDesiredOrientation(math::Quaternion newQuaternion)
{

}
    
bool CombineController::atOrientation()
{
    return true;
}

bool CombineController::atDepth()
{
    return true;
}

void CombineController::holdCurrentHeading()
{

}

void CombineController::update(double timestep)
{

}
        
} // namespace control
} // namespace ram
