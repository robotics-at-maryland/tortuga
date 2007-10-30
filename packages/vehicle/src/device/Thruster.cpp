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

// Project Includes
#include "vehicle/include/device/Thruster.h"
#include "vehicle/include/device/ThrusterCommunicator.h"
#include "vehicle/include/device/ThrusterCommand.h"

namespace ram {
namespace vehicle {
namespace device {
    
Thruster::Thruster(core::ConfigNode config) :
    Device(config["name"].asString()),
    m_address(config["address"].asInt()),
    m_calibrationFactor(config["calibration_factor"].asDouble()),
    m_direction(config["direction"].asInt(1))
{
    // Register thruster
    ThrusterCommunicator::registerThruster(this);

    // Preform a soft reset just to be safe
    ThrusterCommunicator::getSingleton().sendThrusterCommand(
        ThrusterCommand::construct(m_address, ThrusterCommand::SPEED,0));
}

Thruster::~Thruster()
{
    // Preform a soft reset to make sure the power dies to the thruster
    ThrusterCommunicator::getSingleton().sendThrusterCommand(
        ThrusterCommand::construct(m_address, ThrusterCommand::SPEED, 0));

    // Unregister from communicator so it will no when to destory itself
    ThrusterCommunicator::unRegisterThruster(this);
}

ThrusterPtr Thruster::construct(core::ConfigNode config)
{
    return ThrusterPtr(new Thruster(config));
}

ThrusterPtr Thruster::castTo(IDevicePtr ptr)
{
    return boost::dynamic_pointer_cast<Thruster>(ptr);
}
/*
Thruster* Thruster::castTo(IDevice* ptr)
{
    return dynamic_cast<Thruster*>(ptr);
    }*/
    
void Thruster::setForce(double force)
{
    double b = 0; // Not currently used
    int motorCount;
    
    // Convert force here (using calibration factor)
    motorCount = (int)((force / m_calibrationFactor - b) * 1023) / 27;

    // Take into acount motor direction
    motorCount = motorCount * m_direction;
    
    // Clamp the values
    if (motorCount > 1024)
        motorCount = 1023;
    else if (motorCount < -1024)
        motorCount = -1023;

    ThrusterCommunicator::getSingleton().sendThrusterCommand(
        ThrusterCommand::construct(m_address, ThrusterCommand::SPEED,
                                   motorCount));

    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_forceMutex);
        m_force = force;
        m_motorCount = motorCount;
    }


    // Notify observers
    setChanged();
    notifyObservers(0, FORCE_UPDATE);
}

double Thruster::getForce()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_forceMutex);
    return m_force;
}

int Thruster::getMotorCount()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_forceMutex);
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
