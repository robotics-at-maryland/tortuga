/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IVelocitySensor.cpp
 */

// STD Includes
#include <cassert>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/device/VisionVelocitySensor.h"

#include "vision/include/Events.h"

#include "math/include/Events.h"

#include "core/include/EventHub.h"
#include "core/include/EventConnection.h"

namespace ram {
namespace vehicle {
namespace device {

VisionVelocitySensor::VisionVelocitySensor(core::ConfigNode config,
                                           core::EventHubPtr eventHub,
                                           IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IVelocitySensor(eventHub, config["name"].asString()),
    m_velocity(0.0, 0.0)
{
    assert(0 != eventHub.get() && "Vision Velocity Sensor Needs Event Hub");

    m_connection = eventHub->subscribeToType(
        vision::EventType::VELOCITY_UPDATE,
        boost::bind(&VisionVelocitySensor::onVelocityUpdate, this, _1));
}
    
VisionVelocitySensor::~VisionVelocitySensor()
{
    m_connection->disconnect();
}

math::Vector2 VisionVelocitySensor::getVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_velocity;
}

math::Vector3 VisionVelocitySensor::getLocation()
{
    return math::Vector3::ZERO;
}
    
void VisionVelocitySensor::onVelocityUpdate(core::EventPtr event)
{
    ram::math::Vector2EventPtr vevent =
        boost::dynamic_pointer_cast<ram::math::Vector2Event>(event);
    
    // Read in new values
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
        m_velocity = vevent->vector2;
    }

    // Publish sensor update event
    publish(IVelocitySensor::UPDATE, vevent);
}

    
} // namespace device
} // namespace vehicle
} // namespace ram
