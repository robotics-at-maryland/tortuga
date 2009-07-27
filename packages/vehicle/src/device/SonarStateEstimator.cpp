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
#include "core/include/TimeVal.h"

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
    m_lastUpdateTime(core::TimeVal::timeOfDay().get_double()),
    m_stateHat(0.0 , 8) // 8 elements long, all start out 0

{
    // Initialize the the xHat (state) vector (all elements currently zero)
    // +X goes north, +Y goes west (at Transdec)
    m_stateHat[4] = config["pingerLeftPosition"][0].asDouble(30); // X
    m_stateHat[5] = config["pingerLeftPosition"][0].asDouble(10); // Y
    m_stateHat[6] = config["pingerRightPosition"][0].asDouble(30); // X
    m_stateHat[7] = config["pingerRightPosition"][0].asDouble(-10); // Y

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

void SonarStateEstimator::createMeasurementModel(const math::VectorN& xHat, 
                                                 math::MatrixN& result)
{
    // Make the output the proper size
    result.resize(2,8);

    // Do work!
}

math::Radian SonarStateEstimator::findAbsPingerAngle(
    math::Quaternion 
    vehicleOrientation,
    math::Vector3 relativePingerVector)
{
    return math::Radian(0);
}

math::Vector2 SonarStateEstimator::getLeftPingerEstimatedPosition()
{
    return math::Vector2(m_stateHat[4], m_stateHat[5]);
}

math::Vector2 SonarStateEstimator::getRightPingerEstimatedPosition()
{
    return math::Vector2(m_stateHat[6], m_stateHat[7]);
}

double SonarStateEstimator::getDeltaT()
{
    return m_lastUpdateTime - core::TimeVal::timeOfDay().get_double();
}

void SonarStateEstimator::onSonarEvent(core::EventPtr event)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    SonarEventPtr sEvent = boost::dynamic_pointer_cast<SonarEvent>(event);
    
    // Calculate the angle
    math::Radian angle = findAbsPingerAngle(m_currentOrientation,
                                            sEvent->direction);
    
    // Calculate the deltaT since the last update
    double dt = getDeltaT();

    // Call pinger update
    if (sEvent->pingerID == 0)
        pingerRightFilterUpdate(angle, dt);
    else
        pingerLeftFilterUpdate(angle, dt);

    // Don't forget to update m_velocity and m_position from the latest xHat

    // Mark the time this update was finished so we can calculate the dt
    // next frame
    m_lastUpdateTime = core::TimeVal::timeOfDay().get_double();
}

void SonarStateEstimator::pingerLeftFilterUpdate(math::Degree angle, double dt)
{
    // Put update code here
}
    
void SonarStateEstimator::pingerRightFilterUpdate(math::Degree angle, double dt)
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
