/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/ControllerBase.cpp
 */

// STD Includes
#include <cmath>
#include <cassert>

#ifdef RAM_WINDOWS
#define M_PI 3.14159265358979323846
#endif


// Project Includes
#include "control/include/ControllerBase.h"
#include "control/include/ControlFunctions.h"
#include "control/include/DesiredState.h"

#include "vehicle/include/IVehicle.h"

#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"
#include "core/include/TimeVal.h"

#include "math/include/Helpers.h"
#include "math/include/Events.h"
#include "math/include/Quaternion.h"
//#include "imu/include/imuapi.h"


using namespace std;

namespace ram {
namespace control {

ControllerBase::ControllerBase(vehicle::IVehiclePtr vehicle,
                               core::ConfigNode config) :
    IController(config["name"].asString()),
    desiredState(controltest::DesiredStatePtr()),
    m_atDepth(false),
    m_atOrientation(false),
    m_atVelocity(false),
    m_atPosition(false),
    m_depthThreshold(0),
    m_orientationThreshold(0),
    m_velocityThreshold(0),
    m_positionThreshold(0),
    m_vehicle(vehicle)
{   
    init(config); 
}

ControllerBase::ControllerBase(core::ConfigNode config,
                               core::SubsystemList deps) :
    IController(config["name"].asString(),
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    desiredState(controltest::DesiredStatePtr()),
    m_atDepth(false),
    m_atOrientation(false),
    m_atVelocity(false),
    m_atPosition(false),
    m_depthThreshold(0),
    m_orientationThreshold(0),
    m_velocityThreshold(0),
    m_positionThreshold(0),
    m_vehicle(core::Subsystem::getSubsystemOfType<vehicle::IVehicle>(deps))  
{
    init(config); 
}

ControllerBase::~ControllerBase()
{
    unbackground(true);
}

void ControllerBase::update(double timestep)
{
    // Get vehicle state
    math::Vector3 linearAcceleration(m_vehicle->getLinearAcceleration());
    math::Quaternion orientation(m_vehicle->getOrientation());
    math::Vector3 angularRate(m_vehicle->getAngularRate());
    math::Vector2 position(m_vehicle->getPosition());
    math::Vector2 velocity(m_vehicle->getVelocity());
    double depth = m_vehicle->getDepth();
    
    // Run the base values
    math::Vector3 translationalForce(0,0,0);
    math::Vector3 rotationalTorque(0,0,0);

    // Call the base class update function
    doUpdate(timestep, linearAcceleration, orientation, angularRate, depth,
             position, velocity, translationalForce, rotationalTorque);

    // Actually set motor values
    m_vehicle->applyForcesAndTorques(translationalForce, rotationalTorque);
    
    // We use to be at depth now we aren't
    if (m_atDepth && !atDepth())
        m_atDepth = false;
    // We weren't at depth, now we are
    else if (!m_atDepth && atDepth())
        publishAtDepth(getDepth());

    // We used to be at orientation now we aren't
    if (m_atOrientation && !atOrientation())
        m_atOrientation = false;
    // We weren't at depth, now we are
    else if (!m_atOrientation && atOrientation())
        publishAtOrientation(getDesiredOrientation());
}

void ControllerBase::setVelocity(math::Vector2 velocity)
{
    desiredState->setDesiredVelocity(velocity);
}

void ControllerBase::setSpeed(double speed)
{
    desiredState->setDesiredVelocity(math::Vector2(speed,0));
}

void ControllerBase::setSidewaysSpeed(double speed)
{
    desiredState->setDesiredVelocity(math::Vector2(0,speed));
}

void ControllerBase::setDesiredVelocity(math::Vector2 velocity, Frame frame)
{
    if(frame == BODY)
        velocity = nRb(m_vehicle->getOrientation().getYaw().valueRadians())*velocity;
    desiredState->setDesiredVelocity(velocity);
}

void ControllerBase::setDesiredPosition(math::Vector2 position, Frame frame)
{
    if(frame == BODY)
        position = nRb(m_vehicle->getOrientation().getYaw().valueRadians())*position;
    desiredState->setDesiredPosition(position);
}

void ControllerBase::setDesiredPositionAndVelocity(math::Vector2 position, math::Vector2 velocity)
{
    desiredState->setDesiredVelocity(velocity);
    desiredState->setDesiredPosition(position);
}
double ControllerBase::getSpeed()
{
    math::Vector2 velocity(desiredState->getDesiredVelocity());
    velocity = bRn(m_vehicle->getOrientation().getYaw().valueRadians())*velocity;
    return velocity[0];
}

double ControllerBase::getSidewaysSpeed()
{
    math::Vector2 velocity(desiredState->getDesiredVelocity());
    velocity = bRn(m_vehicle->getOrientation().getYaw().valueRadians())*velocity;
    return velocity[1];
}

math::Vector2 ControllerBase::getVelocity()
{
    math::Vector2 velocity(desiredState->getDesiredVelocity());
    velocity = bRn(m_vehicle->getOrientation().getYaw().valueRadians())*velocity;
    return velocity;
}

math::Vector2 ControllerBase::getDesiredVelocity(Frame frame)
{
    math::Vector2 velocity(desiredState->getDesiredVelocity());
    if(frame == BODY)
        velocity = bRn(m_vehicle->getOrientation().getYaw().valueRadians())*velocity;
    return velocity;
}

math::Vector2 ControllerBase::getDesiredPosition(Frame frame)
{
    math::Vector2 position(desiredState->getDesiredVelocity());
    if(frame == BODY)
        position = bRn(m_vehicle->getOrientation().getYaw().valueRadians())*position;
    return position;
}

void ControllerBase::holdCurrentPosition()
{
    /*Not Yet Implemented */
}

bool ControllerBase::atPosition()
{
    /* NOT IMPLEMENTED */
    return false;
}

bool ControllerBase::atVelocity()
{
    /* NOT IMPLEMENTED */
    return false;
}

void ControllerBase::yawVehicle(double degrees)
{
    desiredState->setDesiredOrientation(
        yawVehicleHelper(desiredState->getDesiredOrientation(),degrees));
}

void ControllerBase::pitchVehicle(double degrees)
{
    desiredState->setDesiredOrientation(
        pitchVehicleHelper(desiredState->getDesiredOrientation(),degrees));
}

void ControllerBase::rollVehicle(double degrees)
{
    desiredState->setDesiredOrientation(
        rollVehicleHelper(desiredState->getDesiredOrientation(),degrees));
}

math::Quaternion ControllerBase::getDesiredOrientation()
{
    return desiredState->getDesiredOrientation();
}

void ControllerBase::setDesiredOrientation(math::Quaternion orientation)
{
    desiredState->setDesiredOrientation(orientation);
}

bool ControllerBase::atOrientation()
{
    // Find the quaternion error between the two
    math::Quaternion error = desiredState->getDesiredOrientation().errorQuaternion(
        m_vehicle->getOrientation());

    // This does the sqrt of the sum of the squares for the first three elements
    math::Vector3 tmp(error.ptr());
    if (tmp.length() > m_orientationThreshold)
        return false;
    else
        return true;
}

void ControllerBase::holdCurrentOrientation()
{
    desiredState->setDesiredOrientation(m_vehicle->getOrientation());
}

void ControllerBase::setDepth(double depth)
{
    desiredState->setDesiredDepth(depth);
}

double ControllerBase::getDepth()
{
    return desiredState->getDesiredDepth();
}

double ControllerBase::getEstimatedDepth()
{
    assert(0 && "Not Yet Implemented");
}

double ControllerBase::getEstimatedDepthDot()
{
    assert(0 && "Not Yet Implemented");
}

bool ControllerBase::atDepth()
{
    double currentDepth = m_vehicle->getDepth();
    double desiredDepth = desiredState->getDesiredDepth();
    double difference = fabs(currentDepth - desiredDepth);
    return difference <= m_depthThreshold;
}

void ControllerBase::holdCurrentDepth()
{
    desiredState->setDesiredDepth(m_vehicle->getDepth());
}

void ControllerBase::holdCurrentHeading()
{
    holdCurrentOrientation();
}

void ControllerBase::newDepthSet(const double& newDepth)
{
    // Publish event indicating new update
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = newDepth;
    publish(IController::DESIRED_DEPTH_UPDATE, event);

    // Make sure to publish if we set a depth that is within our range
    if (atDepth())
        publishAtDepth(newDepth);
}

void ControllerBase::newDesiredOrientationSet(
    const math::Quaternion& newOrientation)
{
    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = newOrientation;
    publish(IController::DESIRED_ORIENTATION_UPDATE, event);
    
    if(atOrientation())
        publishAtOrientation(newOrientation);
}

void ControllerBase::newDesiredVelocitySet(const math::Vector2& newVelocity)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = newVelocity;
    publish(IController::DESIRED_VELOCITY_UPDATE, event);
    
    if(atVelocity())
        publishAtVelocity(newVelocity);
}

void ControllerBase::newDesiredPositionSet(const math::Vector2& newPosition)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = newPosition;
    publish(IController::DESIRED_VELOCITY_UPDATE, event);
    
    
    if(atPosition())
        publishAtPosition(newPosition);
}

void ControllerBase::init(core::ConfigNode config)
{
    // Load threshold for being at depth
    m_depthThreshold = config["depthThreshold"].asDouble(DEPTH_TOLERANCE);
    m_orientationThreshold =
        config["orientationThreshold"].asDouble(ORIENTATION_THRESHOLD);
    m_positionThreshold = config["positionThreshold"].asDouble();
    m_velocityThreshold = config["velocityThreshold"].asDouble();
    desiredState = controltest::DesiredStatePtr(new controltest::DesiredState(config["DesiredState"]));
}
                      

void ControllerBase::publishAtDepth(const double& depth)
{
    m_atDepth = true;
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depth;
    publish(IController::AT_DEPTH, event);
}

void ControllerBase::publishAtOrientation(const math::Quaternion& orientation)
{
    m_atOrientation = true;
    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = orientation;
    publish(IController::AT_ORIENTATION, event);        
}


void ControllerBase::publishAtVelocity(const math::Vector2& velocity)
{
    m_atVelocity = true;
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = velocity;
    publish(IController::AT_VELOCITY, event);
}

void ControllerBase::publishAtPosition(const math::Vector2& position)
{
    m_atPosition = true;
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = position;
    publish(IController::AT_POSITION, event);
}
      
} // namespace control
} // namespace ram
