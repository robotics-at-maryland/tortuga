/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/SBPowerSource.cpp
 */

// STD Includes
#include <cassert>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/Events.h"
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/device/SBPowerSource.h"

#include "core/include/EventConnection.h"

#include "math/include/Events.h"

namespace ram {
namespace vehicle {
namespace device {

static std::string idToName(int id)
{
    static std::string id2Name[5] = {
        "Batt 1",
        "Batt 2",
        "Batt 3",
        "Batt 4",
        "Shore",
    };

    assert((id >= 0) && (id <= 5) && "ID Out of range");
    
    return id2Name[id];
}
    
SBPowerSource::SBPowerSource(core::ConfigNode config,
                             core::EventHubPtr eventHub,
                             IVehiclePtr vehicle) :
    Device(idToName(config["id"].asInt())),
    IPowerSource(eventHub),
    m_id(config["id"].asInt()),
    m_enabled(false),
    m_voltage(0.0),
    m_current(0.0),
    m_sensorBoard(SensorBoardPtr())
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
    m_connection = m_sensorBoard->subscribe(
        SensorBoard::POWERSOURCE_UPDATE,
        boost::bind(&SBPowerSource::onPowerSourceUpdate, this, _1));
}
    
SBPowerSource::~SBPowerSource()
{
    m_connection->disconnect();
}

double SBPowerSource::getCurrent()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_current;
}
    
double SBPowerSource::getVoltage()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_voltage;
}
    
bool SBPowerSource::isEnabled()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_enabled;
}

void SBPowerSource::onPowerSourceUpdate(core::EventPtr event)
{
    ram::vehicle::PowerSourceEventPtr psEvent =
        boost::dynamic_pointer_cast<ram::vehicle::PowerSourceEvent>(event);

    // Drop out if the event isn't for us
    if (m_id != psEvent->id)
        return;

    core::Event::EventType enabledEventType;
    core::EventPtr eventPtr;
    
    // Read in new values
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);

        if (m_enabled && !psEvent->enabled)
        {
            enabledEventType = ENABLED;
            eventPtr = core::EventPtr(new core::Event);
        }
        else if (!m_enabled && psEvent->enabled)
        {
            enabledEventType = DISABLED;
            eventPtr = core::EventPtr(new core::Event);
        }
            
        m_enabled = psEvent->enabled;
        m_voltage = psEvent->voltage;
        m_current = psEvent->current;
    }

    // Publish Events as needed
    if (eventPtr.get())
        publish(enabledEventType, eventPtr);

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
