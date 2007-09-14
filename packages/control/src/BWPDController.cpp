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
#include "vehicle/include/device/IMU.h"
#include "vehicle/include/device/Thruster.h"

#include "math/include/Helpers.h"
#include "math/include/Vector3.h"
#include "imu/include/imuapi.h"

using namespace std;

namespace ram {
namespace control {

BWPDController::BWPDController(vehicle::IVehicle* vehicle,
                               core::ConfigNode config) :
    m_vehicle(vehicle),
    m_config(config),
    m_desiredState(0),
    m_measuredState(0),
    m_controllerState(0),
    m_rStarboard(0),
    m_rPort(0),
    m_rFore(0),
    m_rAft(0),
    m_hackedPitchGain(0),
    m_hackedYawGain(0)
{
    printf("Creating controller\n");
    
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
    m_desiredState->depth = config["desiredDepth"].asDouble();
    
    // Set controller state from config file (defaults hard coded)
    m_controllerState->angularPGain = config["angularPGain"].asDouble(1);
    m_controllerState->angularDGain = config["angularDGain"].asDouble(1);
    m_controllerState->depthPGain = config["depthPGain"].asDouble(1);
    m_controllerState->speedPGain = config["speedPGain"].asDouble(1);

    // HACK!! /// TODO: Remove me
    m_hackedPitchGain = config["hackedPitchGain"].asDouble();
    m_hackedYawGain = config["hackedYawGain"].asDouble();
    
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

    // Grab thruster combining constants
    m_rStarboard = config["rStarboard"].asDouble(0.1905);
    m_rPort = config["rPort"].asDouble(0.1905);
    m_rFore= config["rFore"].asDouble(0.3366);
    m_rAft = config["rAft"].asDouble(0.3366);
    
    // Grab our devices
    cout << "Name: " << config["StarboardThrusterName"].asString("StarboardThruster") << endl;
    m_starboardThruster = vehicle::device::Thruster::castTo(vehicle->getDevice(
        config["StarboardThrusterName"].asString("StarboardThruster")));
    assert(m_starboardThruster.get() && "Bad starboard thruster");
    
    m_portThruster = vehicle::device::Thruster::castTo(vehicle->getDevice(
        config["PortThrusterName"].asString("PortThruster")));
    assert(m_starboardThruster.get() && "Bad port thruster");
    
    m_foreThruster = vehicle::device::Thruster::castTo(vehicle->getDevice(
        config["ForeThrusterName"].asString("ForeThruster")));
    assert(m_portThruster.get() && "Bad fore thruster");
    
    m_aftThruster = vehicle::device::Thruster::castTo(vehicle->getDevice(
        config["AftThrusterName"].asString("AftThruster")));
    assert(m_aftThruster.get() && "Bad aft thruster");

    m_imu = vehicle::device::IMU::castTo(vehicle->getDevice(
         config["IMUName"].asString("IMU")));
    assert(m_imu.get() && "Bad imu");
    
/*    m_starboardThruster = dynamic_cast<vehicle::device::Thruster*>(vehicle->getDevice(
        config["StarboardThrusterName"].asString("StarboardThruster")));
    m_portThruster = dynamic_cast<vehicle::device::Thruster*>(vehicle->getDevice(
        config["PortThrusterName"].asString("PortThruster")));
    m_portThruster = dynamic_cast<vehicle::device::Thruster*>(vehicle->getDevice(
        config["ForeThrusterName"].asString("ForeThruster")));
    m_aftThruster = dynamic_cast<vehicle::device::Thruster*>(vehicle->getDevice(
        config["AftThrusterName"].asString("AftThruster")));
    
    m_imu = dynamic_cast<vehicle::device::IMU*>(vehicle->getDevice(
         config["IMUName"].asString("IMU")));*/
}

BWPDController::~BWPDController()
{
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
    math::Vector3 linearAcceleration(m_imu->getLinearAcceleration());
    memcpy(&m_measuredState->linearAcceleration[0],
           &linearAcceleration, sizeof(double) * 3);

//    std::cout << "A: " << linearAcceleration << std::endl;
    math::Quaternion orientation(m_imu->getOrientation());
    memcpy(&m_measuredState->quaternion[0],
           &orientation, sizeof(double) * 4);

//    std::cout << "O: " << orientation << std::endl;
    
    math::Vector3 angularRate(m_imu->getAngularRate());
    memcpy(&m_measuredState->angularRate[0],
           &angularRate, sizeof(double) * 3);

//    std::cout << "W: " << angularRate << std::endl;
    
    vehicle::device::FilteredIMUData state;
    m_imu->getFilteredState(state);
    m_measuredState->magneticField[0] = state.magX;
    m_measuredState->magneticField[1] = state.magY;
    m_measuredState->magneticField[2] = state.magZ;

    m_measuredState->depth = m_vehicle->getDepth();
    
    // Calculate new forces
    double translationalForces[3] = {0,0,0};
    double rotationalTorques[3] = {0,0,0};
//    double pitchHack = 0;

    {
        core::ReadWriteMutex::ScopedReadLock lock(m_desiredStateMutex);
        
        translationalController(m_measuredState, m_desiredState,
        	                m_controllerState, timestep,
                                translationalForces);

        // Doesn't currently handle pitch
        BongWiePDRotationalController(m_measuredState, m_desiredState,
                                      m_controllerState, timestep,
                                      rotationalTorques);
        
//	pitchHack = HackedPDPitchControl(m_measuredState, 
//					 m_desiredState,
//					 m_controllerState, m_hackedPitchGain);
    }

    // Actually set motor values

    // Hacking torques get pitch control
//    rotationalTorques[1] = pitchHack;

    applyForcesAndTorques(translationalForces, rotationalTorques);
}

void BWPDController::applyForcesAndTorques(double* translationalForces,
                                           double* rotationalTorques)
{
    std::cout << "Rot Torq: " << math::Vector3(rotationalTorques) << std::endl;
    std::cout << "Speed " << translationalForces[0] << " Depth " << translationalForces[2] << std::endl;
//    std::cout << "Applying forces" << std::endl;

    double star = translationalForces[0] / 2 +
        0.5 * rotationalTorques[2] / m_rStarboard;
    double port = translationalForces[0] / 2 -
        0.5 * rotationalTorques[2] / m_rPort;
    double fore = translationalForces[2] / 2 +
        0.5 * rotationalTorques[1] / m_rFore;
    double aft = translationalForces[2]/2 -
      0.5 * rotationalTorques[1] / m_rAft;

    // Igrnore rotational control on these thrusters
    // These multipliers are temporary because we have no pitch control
    //    double fore = translationalForces[2] * 1.1;
    //    double aft = translationalForces[2] * 0.9;
//    double fore = 0;
//    double aft = 0;
    
    std::cout << "Force S: " << star << " P: " << port << " F: "
              << fore << " A: " << aft << std::endl;
//    std::cout << "Torque: " << math::Vector3(rotationalTorques) << std::endl;
    
    
    m_starboardThruster->setForce(star);

    m_portThruster->setForce(port);

    m_foreThruster->setForce(fore);

    m_aftThruster->setForce(aft);
}
    
} // namespace control
} // namespace ram
