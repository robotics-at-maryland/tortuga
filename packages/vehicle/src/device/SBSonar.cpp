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
#include "vehicle/include/device/SBSonar.h"

#include "core/include/EventConnection.h"

//#include "drivers/sensor-r5/include/sensorapi.h"

namespace ram {
namespace vehicle {
namespace device {
    
SBSonar::SBSonar(core::ConfigNode config,
                 core::EventHubPtr eventHub,
                 IVehiclePtr vehicle) :
    Device(config["name"].asString("Sonar")),
    ISonar(eventHub, config["name"].asString("Sonar")),
    m_direction(math::Vector3::ZERO),
    m_range(0),
    m_sensorBoard(SensorBoardPtr())
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
    m_connection = m_sensorBoard->subscribe(
        SensorBoard::SONAR_UPDATE,
        boost::bind(&SBSonar::onSonarUpdate, this, _1));
}
    
SBSonar::~SBSonar()
{
    m_connection->disconnect();
}
    
math::Vector3 SBSonar::getDirection()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_direction;
}

double SBSonar::getRange()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_range;
}
    
core::TimeVal SBSonar::getPingTime()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_pingTime;
}

void SBSonar::onSonarUpdate(core::EventPtr event)
{
    ram::vehicle::SonarEventPtr sonarEvent =
        boost::dynamic_pointer_cast<ram::vehicle::SonarEvent>(event);

    // Read in new values
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);

        m_direction = sonarEvent->direction;
        m_range = sonarEvent->range;
        m_pingTime = core::TimeVal(sonarEvent->pingTimeSec,
                                   sonarEvent->pingTimeUSec);
    }

    // Send event telling everyone the new pinger direction
    publish(ISonar::UPDATE, sonarEvent);
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
