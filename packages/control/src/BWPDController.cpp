/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/BWPDController.cpp
 */

// STD Includes
#include <cmath>
#include <cstdio>
#include <iostream>

// Project Includes
#include "control/include/BWPDController.h"
#include "control/include/ControlFunctions.h"

#include "vehicle/include/IVehicle.h"

#include "core/include/SubsystemMaker.h"

#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "imu/include/imuapi.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::control::BWPDController, BWPDController);

using namespace std;

namespace ram {
namespace control {

BWPDController::BWPDController(vehicle::IVehiclePtr vehicle,
                               core::ConfigNode config) :
    IController(config["name"].asString()),
    m_vehicle(vehicle),
    m_config(config),
    m_desiredState(0),
    m_measuredState(0),
    m_controllerState(0)
{   
    init(config); 
}

BWPDController::BWPDController(core::ConfigNode config,
                               core::SubsystemList deps) :
    IController(config["name"].asString()),
    m_vehicle(core::Subsystem::getSubsystemOfType<vehicle::IVehicle>(deps)),
    m_config(config),
    m_desiredState(0),
    m_measuredState(0),
    m_controllerState(0)
{
    init(config);
}
    
BWPDController::~BWPDController()
{
    unbackground(true);
	
    m_logfile.close();
    delete m_desiredState;
    delete m_measuredState;
    delete m_controllerState;
}
    
void BWPDController::setSpeed(int speed)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_desiredStateMutex);
    m_desiredState->speed = speed;
}

void BWPDController::setHeading(double degrees)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_desiredStateMutex);

    // Put math here
}

void BWPDController::setDepth(double depth)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_desiredStateMutex);
    m_desiredState->depth = depth;
}

int BWPDController::getSpeed()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_desiredStateMutex);
    return (int)m_desiredState->speed;
}

double BWPDController::getHeading()
{
    //core::ReadWriteMutex::ScopedReadLock lock(m_desiredStateMutex);
    return 0;
}

double BWPDController::getDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_desiredStateMutex);
    return m_desiredState->depth;
}
    
void BWPDController::rollVehicle(double degrees)
{
  //use Helpers.cpp
  using namespace ram::math;

  //fetch old desired quaternion
  core::ReadWriteMutex::ScopedWriteLock lock(m_desiredStateMutex);

  //create rotation quaternion based on user input
  double rotationQuaternion[4];
  double rollAxis[3] = {1,0,0};
  quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
  //rotate the quaternion and store in a new place
  double newQuaternion[4];
  quaternionCrossProduct(rotationQuaternion, m_desiredState->quaternion,
			 newQuaternion);

  //store the new quaternion as the new desired quaternion
  m_desiredState->quaternion[0] = newQuaternion[0];
  m_desiredState->quaternion[1] = newQuaternion[1];
  m_desiredState->quaternion[2] = newQuaternion[2];
  m_desiredState->quaternion[3] = newQuaternion[3];
    
}

void BWPDController::pitchVehicle(double degrees)
{
  //use Helpers.cpp
  using namespace ram::math;

  //fetch old desired quaternion
  core::ReadWriteMutex::ScopedWriteLock lock(m_desiredStateMutex);

  //create rotation quaternion based on user input
  double rotationQuaternion[4];
  double rollAxis[3] = {0,1,0};
  quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
  //rotate the quaternion and store in a new place
  double newQuaternion[4];
  quaternionCrossProduct(rotationQuaternion, m_desiredState->quaternion,
			 newQuaternion);

  //store the new quaternion as the new desired quaternion
  m_desiredState->quaternion[0] = newQuaternion[0];
  m_desiredState->quaternion[1] = newQuaternion[1];
  m_desiredState->quaternion[2] = newQuaternion[2];
  m_desiredState->quaternion[3] = newQuaternion[3];

}

void BWPDController::yawVehicle(double degrees)
{
  //use Helpers.cpp
  using namespace ram::math;

  //fetch old desired quaternion
  core::ReadWriteMutex::ScopedWriteLock lock(m_desiredStateMutex);

  //create rotation quaternion based on user input
  double rotationQuaternion[4];
  double rollAxis[3] = {0,0,1};
  quaternionFromEulerAxis(rollAxis,degrees*M_PI/180,rotationQuaternion);
  
  //rotate the quaternion and store in a new place
  double newQuaternion[4];
  quaternionCrossProduct(rotationQuaternion, m_desiredState->quaternion,
			 newQuaternion);

  //store the new quaternion as the new desired quaternion
  m_desiredState->quaternion[0] = newQuaternion[0];
  m_desiredState->quaternion[1] = newQuaternion[1];
  m_desiredState->quaternion[2] = newQuaternion[2];
  m_desiredState->quaternion[3] = newQuaternion[3];
}

    
bool BWPDController::isOriented()
{
    return doIsOriented(m_measuredState, m_desiredState,
                        ORIENTATION_THRESHOLD);
}

bool BWPDController::atDepth()
{
    double actual = m_measuredState->depth;
    double expected = m_desiredState->depth;
    return (actual >= (expected - DEPTH_TOLERANCE)) &&
        (actual <= (expected + DEPTH_TOLERANCE));
}
    
void BWPDController::update(double timestep)
{
    // Grab latest state (preform small hack to copy it over for the controller)
    math::Vector3 linearAcceleration(m_vehicle->getLinearAcceleration());
    memcpy(&m_measuredState->linearAcceleration[0],
           &linearAcceleration, sizeof(double) * 3);

    //std::cout << "A: " << linearAcceleration << std::endl;
    math::Quaternion orientation(m_vehicle->getOrientation());
    memcpy(&m_measuredState->quaternion[0],
           &orientation, sizeof(double) * 4);

    //std::cout << "O: " << orientation << std::endl;
    
    math::Vector3 angularRate(m_vehicle->getAngularRate());
    memcpy(&m_measuredState->angularRate[0],
           &angularRate, sizeof(double) * 3);

    //std::cout << "W: " << angularRate << std::endl;
    m_measuredState->depth = m_vehicle->getDepth();
    
    // Calculate new forces
    math::Vector3 translationalForce(0,0,0);
    math::Vector3 rotationalTorque(0,0,0);

    {
        core::ReadWriteMutex::ScopedReadLock lock(m_desiredStateMutex);
        
        translationalController(m_measuredState, m_desiredState,
        	                m_controllerState, timestep,
                                translationalForce.ptr());

        BongWiePDRotationalController(m_measuredState, m_desiredState,
                                      m_controllerState, timestep,
                                      rotationalTorque.ptr());
    }

    // Actually set motor values
    m_vehicle->applyForcesAndTorques(translationalForce, rotationalTorque);
    m_logfile << m_measuredState->quaternion[0] << " "
         << m_measuredState->quaternion[1] << " "
         << m_measuredState->quaternion[2] << " "
         << m_measuredState->quaternion[3] << " "
         << m_measuredState->depth << " "
         << m_desiredState->quaternion[0] << " "
         << m_desiredState->quaternion[1] << " "
         << m_desiredState->quaternion[2] << " "
         << m_desiredState->quaternion[3] << " "
         << m_desiredState->depth << " "
         << m_desiredState->speed << " "
         << rotationalTorque[0] << " "
         << rotationalTorque[1] << " "
         << rotationalTorque[2] << " "
         << translationalForce[0] << " "
         << translationalForce[1] << " "
         << translationalForce[2] << std::endl;
        
}

void BWPDController::init(core::ConfigNode config)
{
    // Allocate state structures
    m_desiredState = new DesiredState();
    m_measuredState = new MeasuredState();
    m_controllerState = new ControllerState();

    // Zero all the structs
    memset(m_desiredState, 0, sizeof(DesiredState));
    memset(m_measuredState, 0, sizeof(MeasuredState));
    memset(m_controllerState, 0, sizeof(ControllerState));

    // Set desired state
    m_desiredState->quaternion[0] = config["desiredQuaternion"][0].asDouble();
    m_desiredState->quaternion[1] = config["desiredQuaternion"][1].asDouble();
    m_desiredState->quaternion[2] = config["desiredQuaternion"][2].asDouble();
    m_desiredState->quaternion[3] = config["desiredQuaternion"][3].asDouble();

    m_desiredState->speed = config["desiredSpeed"].asDouble(0);
    m_desiredState->depth = config["desiredDepth"].asDouble(0);
    
    // Set controller state from config file (defaults hard coded)
    m_controllerState->angularPGain = config["angularPGain"].asDouble(1);
    m_controllerState->angularDGain = config["angularDGain"].asDouble(1);
    m_controllerState->depthPGain = config["depthPGain"].asDouble(1);
    m_controllerState->speedPGain = config["speedPGain"].asDouble(1);

    
    m_controllerState->inertiaEstimate[0][0] =
        config["inertia"][0][0].asDouble(0.201);
    m_controllerState->inertiaEstimate[0][1] =
        config["inertia"][0][1].asDouble(0);
    m_controllerState->inertiaEstimate[0][2] =
        config["inertia"][0][2].asDouble(0);
    
    m_controllerState->inertiaEstimate[1][0] =
        config["inertia"][1][0].asDouble(0);
    m_controllerState->inertiaEstimate[1][1] =
        config["inertia"][1][1].asDouble(1.288);
    m_controllerState->inertiaEstimate[1][2] =
        config["inertia"][1][2].asDouble(0);
    
    m_controllerState->inertiaEstimate[2][0] =
        config["inertia"][2][0].asDouble(0);
    m_controllerState->inertiaEstimate[2][1] =
        config["inertia"][2][1].asDouble(0);
    m_controllerState->inertiaEstimate[2][2] =
        config["inertia"][2][2].asDouble(1.288);

    m_logfile.open("control_log.txt");
    m_logfile << "% M-Quat M-Depth D-Quat D-Depth D-Speed RotTorq TranForce"
              << std::endl;    
}
    
} // namespace control
} // namespace ram
