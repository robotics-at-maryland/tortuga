/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/device/SBThruster.cpp
 */

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/Events.h"
#include "vehicle/include/device/SBThruster.h"
#include "vehicle/include/device/SensorBoard.h"

#include "math/include/Events.h"

namespace ram {
namespace vehicle {
namespace device {
    
SBThruster::SBThruster(core::ConfigNode config, core::EventHubPtr eventHub,
                   IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IThruster(eventHub, config["name"].asString()),
    m_address(config["address"].asInt()),
    m_calibrationFactor(config["calibration_factor"].asDouble()),
    m_direction(config["direction"].asInt(1)),
    m_offset(0),
    m_current(0.0),
    m_enabled(false),
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
        SensorBoard::THRUSTER_UPDATE,
        boost::bind(&SBThruster::onThrusterUpdate, this, _1));
}

SBThruster::~SBThruster()
{
    // Zero forces
    setForce(0);
}

void SBThruster::setForce(double force)
{
    double b = 0; // Not currently used
    int motorCount;
    
    // Convert force here (using calibration factor)
    //double motorVoltage = m_sensorBoard->getMainBusVoltage();
    double motorVoltage = 1;
    motorCount =
        (int)(((force / m_calibrationFactor - b) * 1023) / motorVoltage);

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

double SBThruster::getForce()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_force;
}

double SBThruster::getMaxForce()
{
  return m_calibrationFactor; //(((1023.0 * 27) / 1023) + 0.0) * m_calibrationFactor;
}
    
double SBThruster::getMinForce()
{
  return -m_calibrationFactor; //(((-1023 * 27) / 1023) + 0.0) * m_calibrationFactor;
}

bool SBThruster::isEnabled()
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
            
    bool enabled = m_sensorBoard->isThrusterEnabled(m_address);
    doEnabledEvents(m_enabled, enabled);
    m_enabled = enabled;
    
    return m_enabled;
}

void SBThruster::setEnabled(bool state)
{
    m_sensorBoard->setThrusterEnable(m_address, state);
}
    
double SBThruster::getOffset()
{
    return m_offset;
}

double SBThruster::getCurrent()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_current;
}
    
int SBThruster::getMotorCount()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_motorCount;
}

void SBThruster::update(double)
{
}
    
void SBThruster::background(int)
{
}

void SBThruster::unbackground(bool join)
{
}

bool SBThruster::backgrounded()
{
    return false;
}

void SBThruster::onThrusterUpdate(core::EventPtr event)
{
    ram::vehicle::ThrusterEventPtr thEvent =
        boost::dynamic_pointer_cast<ram::vehicle::ThrusterEvent>(event);

    // Drop out if the event isn't for us
    if (m_address != thEvent->address)
        return;
    
    // Read in new values
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
            
        m_current = thEvent->current;

        doEnabledEvents(m_enabled, thEvent->enabled);
        m_enabled = thEvent->enabled;
    }
    
    math::NumericEventPtr nevent(new math::NumericEvent);
    nevent->number = thEvent->current;
    publish(ICurrentProvider::UPDATE, nevent);

}

void SBThruster::doEnabledEvents(bool currentState, bool newState)
{
    core::Event::EventType enabledEventType;
    core::EventPtr enabledEventPtr;
    
    if (currentState && !newState)
    {
        enabledEventType = DISABLED;
        enabledEventPtr = core::EventPtr(new core::Event);
    }
    else if (!currentState && newState)
    {
        enabledEventType = ENABLED;
        enabledEventPtr = core::EventPtr(new core::Event);
    }

    // Publish Events as needed
    if (enabledEventPtr.get())
        publish(enabledEventType, enabledEventPtr);
}
    
} // namespace device
} // namespace vehicle
} // namespace ram