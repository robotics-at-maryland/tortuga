/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/DesiredState.cpp
 */

// Project Includes
#include "control/include/Helpers.h"
#include "control/include/DesiredState.h"
#include "control/include/IController.h"
#include "vehicle/include/Common.h"
#include "core/include/ReadWriteMutex.h"
#include "math/include/Events.h"
#include "core/include/EventHub.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

DesiredState::DesiredState(core::ConfigNode config, core::EventHubPtr eventHub) :
    core::EventPublisher(eventHub, "DesiredState"),
    m_desiredVelocity(math::Vector2::ZERO),
    m_desiredPosition(math::Vector2::ZERO),
    m_desiredAccel(math::Vector2::ZERO),
    m_desiredDepth(0),
    m_desiredDepthRate(0),
    m_desiredDepthAccel(0),
    m_desiredOrientation(math::Quaternion::IDENTITY),
    m_desiredAngularRate(math::Vector3::ZERO),
    m_desiredAngularAccel(math::Vector3::ZERO)
{
    vx = false;
    vy = false;
    vz = false;
}

DesiredState::~DesiredState() {}

void DesiredState::setvCon(bool x, bool y, bool z)
{
    vx = x;
    vy = y;
    vz = z;
}

void DesiredState::init(core::ConfigNode config)
{
    setDesiredVelocity(math::Vector2(config["desiredVelocity"][0].asDouble(0),
                                     config["desiredVelocity"][1].asDouble(0)));

    setDesiredPosition(math::Vector2(config["desiredPosition"][0].asDouble(0),
                                     config["desiredPosition"][1].asDouble(0)));

    setDesiredAccel(math::Vector2(config["desiredAccel"][0].asDouble(0),
                                  config["desiredAccel"][1].asDouble(0)));

    setDesiredDepth(config["desiredDepth"].asDouble(0));
    setDesiredDepthRate(config["desiredDepthRate"].asDouble(0));
    setDesiredDepthAccel(config["desiredDepthAccel"].asDouble(0));

    setDesiredOrientation(
        math::Quaternion(config["desiredOrientation"][0].asDouble(0),
                         config["desiredOrientation"][1].asDouble(0),
                         config["desiredOrientation"][2].asDouble(0),
                         config["desiredOrientation"][3].asDouble(1)));

    setDesiredAngularRate(
        math::Vector3(config["desiredAngularRate"][0].asDouble(0),
                      config["desiredAngularRate"][1].asDouble(0),
                      config["desiredAngularRate"][2].asDouble(0)));

    setDesiredAngularAccel(
        math::Vector3(config["desiredAngularAccel"][0].asDouble(0),
                      config["desiredAngularAccel"][1].asDouble(0),
                      config["desiredAngularAccel"][2].asDouble(0)));

    newDesiredDepthSet(getDesiredDepth());
    newDesiredDepthRateSet(getDesiredDepthRate());
    newDesiredDepthAccelSet(getDesiredDepthAccel());

    newDesiredOrientationSet(getDesiredOrientation());
    newDesiredAngularRateSet(getDesiredAngularRate());
    newDesiredAngularAccelSet(getDesiredAngularAccel());
    
    newDesiredPositionSet(getDesiredPosition());
    newDesiredVelocitySet(getDesiredVelocity());
    newDesiredLinearAccelSet(getDesiredAccel());
    vx = false;
    vy = false;
    vz = false;
}


math::Vector2 DesiredState::getDesiredPosition()
{   
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredPosition;
}

math::Vector2 DesiredState::getDesiredVelocity()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredVelocity;
}

math::Vector2 DesiredState::getDesiredAccel()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredAccel;
}



math::Quaternion DesiredState::getDesiredOrientation()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredOrientation;
}

math::Vector3 DesiredState::getDesiredAngularRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredAngularRate;
}

math::Vector3 DesiredState::getDesiredAngularAccel()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredAngularAccel;
}



double DesiredState::getDesiredDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredDepth;
}

double DesiredState::getDesiredDepthRate()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredDepthRate;
}

double DesiredState::getDesiredDepthAccel()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredDepthAccel;
}


/* For setting the desired state, it is important that the member
 * variable is set before an event is published.  Otherwise, it
 * would be possible that a function bound to the event would get
 * the incorrect value.  Locks are held for minimal time.
 */

void DesiredState::setDesiredPosition(math::Vector2 position)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_desiredPosition = position;
    }
    //vx = false;
    //vy = false;
    //vz = false;
    newDesiredPositionSet(position);
}

void DesiredState::setDesiredVelocity(math::Vector2 velocity)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_desiredVelocity = velocity;
    }
    newDesiredVelocitySet(velocity);
}

void DesiredState::setDesiredAccel(math::Vector2 accel)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_desiredAccel = accel;
    }
    newDesiredLinearAccelSet(accel);
}



void DesiredState::setDesiredDepth(double depth)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_desiredDepth = depth;
    }
    //vx = false;
    //vy = false;
    //vz = false;
    newDesiredDepthSet(depth);
}

void DesiredState::setDesiredDepthRate(double depthRate)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_desiredDepthRate = depthRate;
    }
    newDesiredDepthRateSet(depthRate);
}

void DesiredState::setDesiredDepthAccel(double depthAccel)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_desiredDepthAccel = depthAccel;
    }
    newDesiredDepthAccelSet(depthAccel);
}




void DesiredState::setDesiredOrientation(math::Quaternion orientation)
{
    orientation.normalise();
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_desiredOrientation = orientation;
    }
    newDesiredOrientationSet(orientation);
}

void DesiredState::setDesiredAngularRate(math::Vector3 angularRate)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_desiredAngularRate = angularRate;
    }
    newDesiredAngularRateSet(angularRate);
}

void DesiredState::setDesiredAngularAccel(math::Vector3 angularAccel)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_desiredAngularAccel = angularAccel;
    }
    newDesiredAngularAccelSet(angularAccel);
}




void DesiredState::newDesiredDepthSet(const double& newDepth)
{
    // Publish event indicating new update
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = newDepth;
    publish(control::IController::DESIRED_DEPTH_UPDATE, event);
}

void DesiredState::newDesiredDepthRateSet(const double& newDepthRate)
{
    // Publish event indicating new update
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = newDepthRate;
    publish(control::IController::DESIRED_DEPTHRATE_UPDATE, event);
}

void DesiredState::newDesiredDepthAccelSet(const double& newDepthAccel)
{
    // Publish event indicating new update
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = newDepthAccel;
    publish(control::IController::DESIRED_DEPTHACCEL_UPDATE, event);
}




void DesiredState::newDesiredOrientationSet(
    const math::Quaternion& newOrientation)
{
    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = newOrientation;
    publish(control::IController::DESIRED_ORIENTATION_UPDATE, event);
}

void DesiredState::newDesiredAngularRateSet(
    const math::Vector3& newAngularRate)
{
    math::Vector3EventPtr event(new math::Vector3Event());
    event->vector3 = newAngularRate;
    publish(control::IController::DESIRED_ANGULARRATE_UPDATE, event);
}

void DesiredState::newDesiredAngularAccelSet(
    const math::Vector3& newAngularAccel)
{
    math::Vector3EventPtr event(new math::Vector3Event());
    event->vector3 = newAngularAccel;
    publish(control::IController::DESIRED_ANGULARACCEL_UPDATE, event);
}




void DesiredState::newDesiredVelocitySet(const math::Vector2& newVelocity)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = newVelocity;
    publish(control::IController::DESIRED_VELOCITY_UPDATE, event);
}

void DesiredState::newDesiredPositionSet(const math::Vector2& newPosition)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = newPosition;
    publish(control::IController::DESIRED_POSITION_UPDATE, event);
}

void DesiredState::newDesiredLinearAccelSet(const math::Vector2& newLinearAccel)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = newLinearAccel;
    publish(control::IController::DESIRED_LINEARACCEL_UPDATE, event);
}

} //namespace control
} //namespace ram
