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
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/IVehicle.h"

#include "vision/include/Events.h"
#include "vision/include/VisionSystem.h"

#include "math/include/Events.h"
#include "math/include/Math.h"

#include "core/include/EventHub.h"
#include "core/include/EventConnection.h"

namespace ram {
namespace vehicle {
namespace device {

static const double FEET_TO_METERS = 0.3048;
    
VisionVelocitySensor::VisionVelocitySensor(core::ConfigNode config,
                                           core::EventHubPtr eventHub,
                                           IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IVelocitySensor(eventHub, config["name"].asString()),
    m_velocity(0.0, 0.0),
    m_downwardDistance(0),
    m_bottomDepth(config["bottomDepth"].asDouble(25.0)),
    m_lastTimeStamp(-1)
{
    assert(0 != eventHub.get() && "Vision Velocity Sensor Needs Event Hub");

    m_velocityConnection = eventHub->subscribeToType(
        vision::EventType::VELOCITY_UPDATE,
        boost::bind(&VisionVelocitySensor::onVelocityUpdate, this, _1));

    std::string depthSensorName =
        config["depthSensorName"].asString("SensorBoard");
    IDepthSensorPtr depthSensor(IDevice::castTo<IDepthSensor>(
        vehicle->getDevice(depthSensorName)));
    m_depthConnection = depthSensor->subscribe(
        IDepthSensor::UPDATE,
        boost::bind(&VisionVelocitySensor::onDepthUpdate, this, _1));
}
    
VisionVelocitySensor::~VisionVelocitySensor()
{
    m_velocityConnection->disconnect();
    m_depthConnection->disconnect();
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

    
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);

        if (m_lastTimeStamp >= 0)
        {
            // Convert to meters from pixels (per second added below)
            math::Vector2 pixelVelocity(vevent->vector2);
            double xMeters = pixelVelocity.x * getXConversionFactor();
            double yMeters = pixelVelocity.y * getYConversionFactor();

            // Determine time difference
            double dt = event->timeStamp - m_lastTimeStamp;
            m_lastTimeStamp = event->timeStamp;

            // Get the velocity meters/timediference
            m_velocity = math::Vector2(xMeters/dt, yMeters/dt);
        }
        else
        {
            m_lastTimeStamp = event->timeStamp;
        }
    }

    // Publish sensor update event
    publish(IVelocitySensor::UPDATE, vevent);
}

void VisionVelocitySensor::onDepthUpdate(core::EventPtr event)
{
    math::NumericEventPtr depthEvent =
        boost::dynamic_pointer_cast<math::NumericEvent>(event);
    
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_downwardDistance = (m_bottomDepth - depthEvent->number) * FEET_TO_METERS;
}

double VisionVelocitySensor::getXConversionFactor()
{
    double imageWidthM = m_downwardDistance * math::Math::Tan(
        vision::VisionSystem::getDownHorizontalFieldOfView() / 2);
    return imageWidthM /
        (vision::VisionSystem::getDownHorizontalPixelResolution() / 2);
}

double VisionVelocitySensor::getYConversionFactor()
{
    double imageHeightM = m_downwardDistance * math::Math::Tan(
        vision::VisionSystem::getDownVerticalFieldOfView() / 2);
    return imageHeightM /
        (vision::VisionSystem::getDownVerticalPixelResolution() / 2);
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
