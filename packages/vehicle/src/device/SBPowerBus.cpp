/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/SBPowerBus.cpp
 */

// STD Includes
#include <cassert>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/Events.h"
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/device/SBPowerBus.h"

#include "core/include/EventConnection.h"

#include "math/include/Events.h"

namespace ram {
namespace vehicle {
namespace device {

static std::string voltageToName(int voltage)
{
    if (5 == voltage) {
        return "5 Volts";
    } else if (12 == voltage) {
        return "12 Volts";
    }

    assert(false && "Bus of given voltage does not exist");
    
    return "Does Not Exist";
}
    
SBPowerBus::SBPowerBus(core::ConfigNode config,
                             core::EventHubPtr eventHub,
                             IVehiclePtr vehicle) :
    Device(voltageToName(config["voltage"].asInt())),
    IPowerBus(eventHub, voltageToName(config["voltage"].asInt())),
    m_id(config["voltage"].asInt()),
    m_voltage(0.0),
    m_current(0.0),
    m_sensorBoard(SensorBoardPtr())
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
    m_connection = m_sensorBoard->subscribe(
        SensorBoard::POWERSOURCE_UPDATE,
        boost::bind(&SBPowerBus::onPowerSourceUpdate, this, _1));
}
    
SBPowerBus::~SBPowerBus()
{
    m_connection->disconnect();
}

double SBPowerBus::getCurrent()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_current;
}
    
double SBPowerBus::getVoltage()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_voltage;
}
    
void SBPowerBus::onPowerSourceUpdate(core::EventPtr event)
{
    ram::vehicle::PowerSourceEventPtr psEvent =
        boost::dynamic_pointer_cast<ram::vehicle::PowerSourceEvent>(event);

    // Drop out if the event isn't for us
    if (m_id != psEvent->id)
        return;
    
    // Read in new values
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
        
        m_voltage = psEvent->voltage;
        m_current = psEvent->current;
    }

    // Publish numerical update events
    math::NumericEventPtr nevent(new math::NumericEvent);
    nevent->number = psEvent->voltage;
    publish(IVoltageProvider::UPDATE, nevent);

    nevent = math::NumericEventPtr(new math::NumericEvent);
    nevent->number = psEvent->current;
    publish(ICurrentProvider::UPDATE, nevent);
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
