/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/device/Thruster.cpp
 */

// STD Includes
#include <sstream>

// Project Includes
#include "vehicle/include/device/Thruster.h"
#include "vehicle/include/device/ThrusterCommunicator.h"
#include "vehicle/include/device/ThrusterCommand.h"

namespace ram {
namespace vehicle {
namespace device {
    
const std::string Thruster::SOFT_RESET = std::string("Y");
const std::string Thruster::SET_FORCE = std::string("C");
    
Thruster::Thruster(Vehicle* vehicle, core::ConfigNode config) :
    Device(vehicle, config["name"].asString()),
    m_address(config["address"].asString()),
    m_calibrationFactor(config["calibration_factor"].asDouble())
{
    // Register thruster
    ThrusterCommunicator::registerThruster(this);

    // Preform a soft reset just to be safe
    ThrusterCommandPtr cmd(new ThrusterCommand(m_address, SOFT_RESET));
    ThrusterCommunicator::getSingleton().sendThrusterCommand(cmd);
}

Thruster::~Thruster()
{
    // Preform a soft reset to make sure the power dies to the thruster
    ThrusterCommandPtr cmd(new ThrusterCommand(m_address, SOFT_RESET));
    ThrusterCommunicator::getSingleton().sendThrusterCommand(cmd);

    // Unregister from communicator so it will no when to destory itself
    ThrusterCommunicator::unRegisterThruster(this);
}

ThrusterPtr Thruster::construct(Vehicle* vehicle, core::ConfigNode config)
{
    return ThrusterPtr(new Thruster(vehicle, config));
}
    
void Thruster::setForce(double force)
{
    m_force = force;
    double b = 0; // Not currently used
    int motorCount;
    
    // Convert force here (using calibration factor)
    motorCount = (int)((m_force / m_calibrationFactor - b) * 1023) / 27;

    // Clamp the values
    if (motorCount > 1024)
        motorCount = 1023;
    else if (motorCount < -1024)
        motorCount = -1023;
    m_motorCount = motorCount;

    std::stringstream ss;
    ss << " " << m_motorCount;
    ThrusterCommandPtr cmd(new ThrusterCommand(m_address, SET_FORCE, ss.str()));
    ThrusterCommunicator::getSingleton().sendThrusterCommand(cmd);
}

double Thruster::getForce()
{
    return m_force;
}

int Thruster::getMotorCount()
{
    return m_motorCount;
}
    
void Thruster::update(double timestep)
{
    ThrusterCommunicator::getSingleton().update(timestep);
}
    
void Thruster::background(int interval)
{
    ThrusterCommunicator::getSingleton().background(interval);
}

void Thruster::unbackground(bool join)
{
    ThrusterCommunicator::getSingleton().unbackground(join);
}

bool Thruster::backgrounded()
{
    return ThrusterCommunicator::getSingleton().backgrounded();
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
