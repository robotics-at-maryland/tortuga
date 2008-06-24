/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/SBTempSensor.cpp
 */

// STD Includes
#include <cassert>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/Events.h"
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/device/SBTempSensor.h"

#include "core/include/EventConnection.h"

#include "math/include/Events.h"

#include "sensorapi-r5/include/sensorapi.h"

namespace ram {
namespace vehicle {
namespace device {
    
SBTempSensor::SBTempSensor(core::ConfigNode config,
                             core::EventHubPtr eventHub,
                             IVehiclePtr vehicle) :
    Device(tempSensorIDToText(config["id"].asInt())),
    ITempSensor(eventHub),
    m_id(config["id"].asInt()),
    m_temp(0),
    m_sensorBoard(SensorBoardPtr())
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
    m_connection = m_sensorBoard->subscribe(
        SensorBoard::TEMPSENSOR_UPDATE,
        boost::bind(&SBTempSensor::onTempSensorUpdate, this, _1));
}
    
SBTempSensor::~SBTempSensor()
{
    m_connection->disconnect();
}

int SBTempSensor::getTemp()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_temp;
}

void SBTempSensor::onTempSensorUpdate(core::EventPtr event)
{
    ram::vehicle::TempSensorEventPtr tempEvent =
        boost::dynamic_pointer_cast<ram::vehicle::TempSensorEvent>(event);

    // Drop out if the event isn't for us
    if (m_id != tempEvent->id)
        return;

    // Read in new values
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);

        m_temp = tempEvent->temp;
    }

    math::NumericEventPtr nevent(new math::NumericEvent);
    nevent->number = tempEvent->temp;
    publish(ITempSensor::UPDATE, nevent);
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
