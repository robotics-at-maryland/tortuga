/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/SonarStateEstimator.h
 */

// Library Includes
#include "boost/bind.hpp"

// Project Includes
#include "vehicle/include/device/SonarStateEstimator.h"
#include "vehicle/include/device/ISonar.h"
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/Events.h"

#include "core/include/EventConnection.h"

namespace ram {
namespace vehicle {
namespace device {

SonarStateEstimator::SonarStateEstimator(core::ConfigNode config,
                                         core::EventHubPtr eventHub,
                                         IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IStateEstimator(eventHub, config["name"].asString()),
    m_estimatedOrientation(math::Quaternion::IDENTITY),
    m_estimatedVelocity(math::Vector2::ZERO),
    m_estimatedPosition(math::Vector2::ZERO),
    m_estimatedDepth(0),
    m_currentOrientation(math::Quaternion::IDENTITY),
    m_currentVelocity(math::Vector2::ZERO),
    m_currentDepth(0),
    
    // Load the initial pinger positions
    m_pinger0Position(config["pinger0Position"][0].asDouble(10),
                      config["pinger0Position"][1].asDouble(0)),
    m_pinger1Position(config["pinger1Position"][0].asDouble(-10),
                      config["pinger1Position"][1].asDouble(0))
{
    // Subscribe to the the sonar events
    std::string sonarDeviceName  = config["sonarDeviceName"].asString("Sonar");
    IDevicePtr sonarDevice(vehicle->getDevice(sonarDeviceName));
    m_sonarConnection = sonarDevice->subscribe(ISonar::UPDATE,
        boost::bind(&SonarStateEstimator::onSonarEvent, this, _1));
}
    
SonarStateEstimator::~SonarStateEstimator()
{
    m_sonarConnection->disconnect();
}

void SonarStateEstimator::orientationUpdate(math::Quaternion orientation)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_currentOrientation = orientation;

    // We don't estimated orientation so lets pipe it straight through
    m_estimatedOrientation = m_currentOrientation;
}

void SonarStateEstimator::velocityUpdate(math::Vector2 velocity)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_currentVelocity = velocity;

    // Update the filter based on the new velocity
    velocityFilterUpdate(velocity);
}

void SonarStateEstimator::positionUpdate(math::Vector2 position)
{
    // Do nothing because we don't depend on another position sensors its
    // all estimated internally
}
    
void SonarStateEstimator::depthUpdate(double depth)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_currentDepth = depth;

    // We don't estimated depth, so pipe it right through
    m_estimatedDepth = m_currentDepth;
}
    
math::Quaternion SonarStateEstimator::getOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_estimatedOrientation;
}

math::Vector2 SonarStateEstimator::getVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_estimatedVelocity;
}

math::Vector2 SonarStateEstimator::getPosition()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_estimatedPosition;
}
    
double SonarStateEstimator::getDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_mutex);
    return m_estimatedDepth;
}

void SonarStateEstimator::onSonarEvent(core::EventPtr event)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    SonarEventPtr sEvent = boost::dynamic_pointer_cast<SonarEvent>(event);

    // TODO: Any possible angle corrections needed
    // Remove Z component from direction
//    math::Vector3 flatDirection(direction.x, direction.y, 0);

    // Create the vector that points in the direction the vehicle is facing
    // in the inertial frame
//    math::Vector3 vehicleDirection =
//        m_currentOrientation * math::Vector3::UNIT_X;
//    referenceVector3.z = 0;
    
    // Calculate the angle
    math::Quaternion quat =
        math::Vector3::UNIT_X.getRotationTo(sEvent->direction);
    math::Degree angle(quat.getYaw(true));
    
    // Call pinger update
    if (sEvent->pingerID == 0)
        pinger0FilterUpdate(angle);
    else
        pinger1FilterUpdate(angle);
}

void SonarStateEstimator::pinger0FilterUpdate(math::Degree angle)
{
    // Put update code here
}
    
void SonarStateEstimator::pinger1FilterUpdate(math::Degree angle)
{
    // Put update code here
}

void SonarStateEstimator::velocityFilterUpdate(math::Vector2 velocity)
{
    // Put update code here
    // Pipe it straight through right now
    m_estimatedVelocity = velocity;
}

    
    
} // namespace device
} // namespace vehicle
} // namespace ram
