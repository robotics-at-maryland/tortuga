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
    static std::string id2Name[6] = {
        "Batt 1",
        "Batt 2",
        "Batt 3",
        "Batt 4",
        "Batt 5",
        "Shore",
    };

    assert((id >= 0) && (id <= 5) && "ID Out of range");
    
    return id2Name[id];
}
    
SBPowerSource::SBPowerSource(core::ConfigNode config,
                             core::EventHubPtr eventHub,
                             IVehiclePtr vehicle) :
    Device(idToName(config["id"].asInt())),
    IPowerSource(eventHub, idToName(config["id"].asInt())),
    m_id(config["id"].asInt()),
    m_enabled(false),
    m_inUse(false),
    m_voltage(0.0),
    m_current(0.0),
    m_sensorBoard(SensorBoardPtr())
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
    m_connection = m_sensorBoard->subscribe(
        SensorBoard::POWERSOURCE_UPDATE,
        boost::bind(&SBPowerSource::onPowerSourceUpdate, this, _1));

    m_enabled = m_sensorBoard->isPowerSourceEnabled(m_id);
    m_inUse = m_sensorBoard->isPowerSourceInUse(m_id);
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

bool SBPowerSource::inUse()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_inUse;
}

void SBPowerSource::setEnabled(bool state)
{
    m_sensorBoard->setPowerSouceEnabled(m_id, state);
}
    
void SBPowerSource::onPowerSourceUpdate(core::EventPtr event)
{
    ram::vehicle::PowerSourceEventPtr psEvent =
        boost::dynamic_pointer_cast<ram::vehicle::PowerSourceEvent>(event);

    // Drop out if the event isn't for us
    if (m_id != psEvent->id)
        return;

    core::Event::EventType enabledEventType;
    core::EventPtr enabledEventPtr;

    core::Event::EventType inUseEventType;
    core::EventPtr inUseEventPtr;
    
    // Read in new values
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);

        if (m_enabled && !psEvent->enabled)
        {
            enabledEventType = DISABLED;
            enabledEventPtr = core::EventPtr(new core::Event);
        }
        else if (!m_enabled && psEvent->enabled)
        {
            enabledEventType = ENABLED;
            enabledEventPtr = core::EventPtr(new core::Event);
        }

        if (m_inUse && !psEvent->inUse)
        {
            inUseEventType = NOT_USING;
            inUseEventPtr = core::EventPtr(new core::Event);
        }
        else if (!m_inUse && psEvent->inUse)
        {
            inUseEventType = USING;
            inUseEventPtr = core::EventPtr(new core::Event);
        }
        
        m_enabled = psEvent->enabled;
        m_inUse = psEvent->inUse;
        m_voltage = psEvent->voltage;
        m_current = psEvent->current;
    }

    // Publish Events as needed
    if (enabledEventPtr.get())
        publish(enabledEventType, enabledEventPtr);

    if (inUseEventPtr.get())
        publish(inUseEventType, inUseEventPtr);
    
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
