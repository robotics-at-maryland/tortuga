/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/device/Thruster.cpp
 */

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/Events.h"
#include "vehicle/include/device/Thruster.h"
#include "vehicle/include/device/SensorBoard.h"

#include "math/include/Events.h"

namespace ram {
namespace vehicle {
namespace device {
    
Thruster::Thruster(core::ConfigNode config, core::EventHubPtr eventHub,
                   IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IThruster(eventHub),
    m_address(config["address"].asInt()),
    m_calibrationFactor(config["calibration_factor"].asDouble()),
    m_direction(config["direction"].asInt(1)),
    m_offset(0),
    m_current(0.0),
    m_sensorBoard(SensorBoardPtr())
{
    // A little hack to determine the offset based on thruster type
    std::string name(getName());
    if (std::string::npos != name.find("Starboard"))
        m_offset = config["offset"].asDouble(0.1905);
    else if (std::string::npos != name.find("Port"))
        m_offset = config["offset"].asDouble(0.1905);
    else if (std::string::npos != name.find("Fore"))
        m_offset = config["offset"].asDouble(0.3366);
    else if (std::string::npos != name.find("Aft"))
        m_offset = config["offset"].asDouble(0.3366);
    else if (std::string::npos != name.find("Top"))
        m_offset = config["offset"].asDouble(0.193);
    else if (std::string::npos != name.find("Bottom"))
        m_offset = config["offset"].asDouble(0.193);

    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
    m_connection = m_sensorBoard->subscribe(
        SensorBoard::MOTORCURRENT_UPDATE,
        boost::bind(&Thruster::onMotorCurrentUpdate, this, _1));
}

Thruster::~Thruster()
{
    // Zero forces
    setForce(0);
}

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

    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_force = force;
        m_motorCount = motorCount;
    }

    // Send the computed value to the hardware
    m_sensorBoard->setThrusterValue(m_address, motorCount);

    math::NumericEventPtr event(new math::NumericEvent());
    event->number = m_force;
    publish(IThruster::FORCE_UPDATE, event);
}

double Thruster::getForce()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_force;
}

double Thruster::getMaxForce()
{
    return (((1023.0 * 27) / 1023) + 0.0) * m_calibrationFactor;
}
    
double Thruster::getMinForce()
{
    return (((-1023 * 27) / 1023) + 0.0) * m_calibrationFactor;
}

bool Thruster::isEnabled()
{
    return m_sensorBoard->isThrusterEnabled(m_address);
}

void Thruster::setEnabled(bool state)
{
    m_sensorBoard->setThrusterEnable(m_address, state);
}
    
double Thruster::getOffset()
{
    return m_offset;
}

double Thruster::getCurrent()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_current;
}
    
int Thruster::getMotorCount()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_motorCount;
}

void Thruster::update(double)
{
}
    
void Thruster::background(int)
{
}

void Thruster::unbackground(bool join)
{
}

bool Thruster::backgrounded()
{
    return false;
}

void Thruster::onMotorCurrentUpdate(core::EventPtr event)
{
    ram::vehicle::MotorCurrentEventPtr mcEvent =
        boost::dynamic_pointer_cast<ram::vehicle::MotorCurrentEvent>(event);

    // Drop out if the event isn't for us
    if (m_address != mcEvent->address)
        return;

    // Read in new values
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
            
        m_current = mcEvent->current;
    }

    math::NumericEventPtr nevent(new math::NumericEvent);
    nevent->number = mcEvent->current;
    publish(ICurrentProvider::UPDATE, nevent);

}
    
} // namespace device
} // namespace vehicle
} // namespace ram
