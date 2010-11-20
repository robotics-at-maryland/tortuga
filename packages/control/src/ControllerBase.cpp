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

// Library Includes
#include <boost/bind.hpp>

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

#ifdef RAM_WINDOWS
#define M_PI 3.14159265358979323846
#endif

namespace ram {
namespace control {

ControllerBase::ControllerBase(vehicle::IVehiclePtr vehicle,
                               core::ConfigNode config) :
    IController(config["name"].asString()),
    desiredState(controltest::DesiredStatePtr(
                     new controltest::DesiredState(config["DesiredState"]))),
    stateEstimator(estimator::IStateEstimatorPtr()),
    m_vehicle(vehicle),
    conn_desired_atDepth(core::EventConnectionPtr()),
    conn_estimated_atDepth(core::EventConnectionPtr()),
    conn_desired_atPosition(core::EventConnectionPtr()),
    conn_estimated_atPosition(core::EventConnectionPtr()),
    conn_desired_atVelocity(core::EventConnectionPtr()),
    conn_estimated_atVelocity(core::EventConnectionPtr()),
    conn_desired_atOrientation(core::EventConnectionPtr()),
    conn_estimated_atOrientation(core::EventConnectionPtr()),
    m_atDepth(false),
    m_atOrientation(false),
    m_atVelocity(false),
    m_atPosition(false), 
    m_depthThreshold(0.05),
    m_orientationThreshold(0.05),
    m_velocityThreshold(0.05),
    m_positionThreshold(0.05),
    m_initHoldDepth(1),
    m_initHoldHeading(1)
{   
    init(config); 
}

ControllerBase::ControllerBase(core::ConfigNode config,
                               core::SubsystemList deps) :
    IController(config["name"].asString(),
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    desiredState(controltest::DesiredStatePtr(
                     new controltest::DesiredState(
                         config["DesiredState"],
                         core::Subsystem::getSubsystemOfType<core::EventHub>(deps)))),
    stateEstimator(estimator::IStateEstimatorPtr()),
    m_vehicle(core::Subsystem::getSubsystemOfType<vehicle::IVehicle>(deps)),  
    conn_desired_atDepth(core::EventConnectionPtr()),
    conn_estimated_atDepth(core::EventConnectionPtr()),
    conn_desired_atPosition(core::EventConnectionPtr()),
    conn_estimated_atPosition(core::EventConnectionPtr()),
    conn_desired_atVelocity(core::EventConnectionPtr()),
    conn_estimated_atVelocity(core::EventConnectionPtr()),
    conn_desired_atOrientation(core::EventConnectionPtr()),
    conn_estimated_atOrientation(core::EventConnectionPtr()),
    m_atDepth(false),
    m_atOrientation(false),
    m_atVelocity(false),
    m_atPosition(false),
    m_depthThreshold(0.05),
    m_orientationThreshold(0.05),
    m_velocityThreshold(0.05),
    m_positionThreshold(0.05),
    m_initHoldDepth(1),
    m_initHoldHeading(1)
{


    core::EventHubPtr eventHub = 
        core::Subsystem::getSubsystemOfType<core::EventHub>(deps);

    if(eventHub != core::EventHubPtr()) {
    /* Bind atDepth, atPosition, atVelocity, and atOrientation to desired state 
       and estimated state update events */

        conn_desired_atDepth = eventHub->subscribeToType(
            IController::DESIRED_DEPTH_UPDATE,
            boost::bind(&ControllerBase::atDepthUpdate,this,_1));

        conn_estimated_atDepth = eventHub->subscribeToType(
            estimator::IStateEstimator::ESTIMATED_DEPTH_UPDATE,
            boost::bind(&ControllerBase::atDepthUpdate,this,_1));

        conn_desired_atPosition = eventHub->subscribeToType(
            IController::DESIRED_POSITION_UPDATE,
            boost::bind(&ControllerBase::atPositionUpdate,this,_1));

        conn_estimated_atPosition = eventHub->subscribeToType(
            estimator::IStateEstimator::ESTIMATED_POSITION_UPDATE,
            boost::bind(&ControllerBase::atPositionUpdate,this,_1));

        conn_desired_atVelocity = eventHub->subscribeToType(
            IController::DESIRED_VELOCITY_UPDATE,
            boost::bind(&ControllerBase::atVelocityUpdate,this,_1));

        conn_estimated_atVelocity = eventHub->subscribeToType(
            estimator::IStateEstimator::ESTIMATED_VELOCITY_UPDATE,
            boost::bind(&ControllerBase::atVelocityUpdate,this,_1));

        conn_desired_atOrientation = eventHub->subscribeToType(
            IController::DESIRED_ORIENTATION_UPDATE,
            boost::bind(&ControllerBase::atOrientationUpdate,this,_1));

        conn_estimated_atOrientation = eventHub->subscribeToType(
            estimator::IStateEstimator::ESTIMATED_ORIENTATION_UPDATE,
            boost::bind(&ControllerBase::atOrientationUpdate,this,_1));       
    }
    if(m_vehicle != vehicle::IVehiclePtr()){
        stateEstimator = m_vehicle->getStateEstimator();
    }

    init(config); 
}

ControllerBase::~ControllerBase()
{
    // unbind the event 
    if(conn_desired_atDepth)
        conn_desired_atDepth->disconnect();

    if(conn_estimated_atDepth)
        conn_estimated_atDepth->disconnect();

    if(conn_desired_atPosition)
        conn_desired_atPosition->disconnect();

    if(conn_estimated_atPosition)
        conn_estimated_atPosition->disconnect();

    if(conn_desired_atVelocity)
        conn_desired_atVelocity->disconnect();

    if(conn_estimated_atVelocity)
        conn_estimated_atVelocity->disconnect();

    if(conn_desired_atOrientation)
        conn_desired_atOrientation->disconnect();

    if(conn_estimated_atOrientation)
        conn_estimated_atOrientation->disconnect();

    unbackground(true);
}

void ControllerBase::update(double timestep)
{
    // Get vehicle state
    // once the new state estimator is complete, change m_vehicle to stateEstimator
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
}

void ControllerBase::setVelocity(math::Vector2 velocity)
{
    setDesiredVelocity(velocity,IController::BODY_FRAME);
}

void ControllerBase::setSpeed(double speed)
{
    // clip speed at 5
    if(speed > 5)
        speed = 5;
    else if(speed < -5)
        speed = -5;

    setDesiredVelocity(math::Vector2(speed,0), IController::BODY_FRAME);
}

void ControllerBase::setSidewaysSpeed(double speed)
{
    // clip speed at 5
    if(speed > 5)
        speed = 5;
    else if(speed < -5)
        speed = -5;

    setDesiredVelocity(math::Vector2(0,speed), IController::BODY_FRAME);
}

void ControllerBase::setDesiredVelocity(math::Vector2 velocity, int frame)
{
    if(frame == IController::BODY_FRAME)
        velocity = math::nRb(m_vehicle->getOrientation().getYaw().valueRadians())*velocity;
    desiredState->setDesiredVelocity(velocity);
}

void ControllerBase::setDesiredPosition(math::Vector2 position, int frame)
{
    if(frame == IController::BODY_FRAME)
        position = math::nRb(m_vehicle->getOrientation().getYaw().valueRadians())*position;
    desiredState->setDesiredPosition(position);
}

void ControllerBase::setDesiredPositionAndVelocity(math::Vector2 position, math::Vector2 velocity)
{
    desiredState->setDesiredVelocity(velocity);
    desiredState->setDesiredPosition(position);
}
double ControllerBase::getSpeed()
{
    math::Vector2 velocity = getDesiredVelocity(IController::BODY_FRAME);
    return velocity[0];
}

double ControllerBase::getSidewaysSpeed()
{
    math::Vector2 velocity = getDesiredVelocity(IController::BODY_FRAME);
    return velocity[1];
}

math::Vector2 ControllerBase::getVelocity()
{
    math::Vector2 velocity(desiredState->getDesiredVelocity());
    velocity = math::bRn(m_vehicle->getOrientation().getYaw().valueRadians())*velocity;
    return velocity;
}

math::Vector2 ControllerBase::getDesiredVelocity(int frame)
{
    math::Vector2 velocity(desiredState->getDesiredVelocity());
    if(frame == IController::BODY_FRAME)
        velocity = math::bRn(m_vehicle->getOrientation().getYaw().valueRadians())*velocity;
    return velocity;
}

math::Vector2 ControllerBase::getDesiredPosition(int frame)
{
    math::Vector2 position(desiredState->getDesiredPosition());
    if(frame == IController::BODY_FRAME)
        position = math::bRn(m_vehicle->getOrientation().getYaw().valueRadians())*position;
    return position;
}

void ControllerBase::holdCurrentPosition()
{
    setDesiredPosition(m_vehicle->getPosition(),INERTIAL_FRAME);
}

bool ControllerBase::atPosition()
{
    math::Vector2 currentPosition = m_vehicle->getPosition();
    math::Vector2 desiredPosition = desiredState->getDesiredPosition();
    double difference0 = fabs(currentPosition[0] - desiredPosition[0]);
    double difference1 = fabs(currentPosition[1] - desiredPosition[1]);
    return difference0 <= m_positionThreshold && difference1 <= m_positionThreshold;
}

bool ControllerBase::atVelocity()
{
    math::Vector2 currentVelocity = m_vehicle->getVelocity();
    math::Vector2 desiredVelocity = desiredState->getDesiredVelocity();
    double difference0 = fabs(currentVelocity[0] - desiredVelocity[0]);
    double difference1 = fabs(currentVelocity[1] - desiredVelocity[1]);
    return difference0 <= m_velocityThreshold && difference1 <= m_velocityThreshold;
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
    return m_vehicle->getDepth();
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

void ControllerBase::init(core::ConfigNode config)
{
    // Load threshold for being at depth
    m_depthThreshold = config["depthThreshold"].asDouble(DEPTH_TOLERANCE);
    m_orientationThreshold =
        config["orientationThreshold"].asDouble(ORIENTATION_THRESHOLD);
    m_positionThreshold = config["positionThreshold"].asDouble(POSITION_THRESHOLD);
    m_velocityThreshold = config["velocityThreshold"].asDouble(VELOCITY_THRESHOLD);

    m_initHoldDepth = config["holdCurrentDepth"].asInt(1);
    m_initHoldHeading = config["holdCurrentHeading"].asInt(1);

    if(m_initHoldDepth)
        holdCurrentDepth();
    if(m_initHoldHeading)
        holdCurrentHeading();
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

void ControllerBase::atDepthUpdate(core::EventPtr event)
{
    if(atDepth())
        if(!m_atDepth){
            m_atDepth = true;
            publishAtDepth(desiredState->getDesiredDepth());
        } else {
            // dont publish if we were already at depth
        }
    else
        m_atDepth = false;
}

void ControllerBase::atPositionUpdate(core::EventPtr event)
{
    if(atPosition())
        if(!m_atPosition){
            m_atPosition = true;
            publishAtPosition(desiredState->getDesiredPosition());
        } else {
            // dont publish if we were already at position
        }
    else
        m_atPosition = false;
}

void ControllerBase::atVelocityUpdate(core::EventPtr event)
{
    if(atVelocity())
        if(!m_atVelocity){
            m_atVelocity = true;
            publishAtVelocity(desiredState->getDesiredVelocity());
        } else {
            //dont publish if we were already at position
        }
    else
        m_atVelocity = false;
}

void ControllerBase::atOrientationUpdate(core::EventPtr event)
{
    if(atOrientation())
        if(!m_atOrientation){
            m_atOrientation = true;
            publishAtOrientation(desiredState->getDesiredOrientation());
        } else {
            // don't publish if we were already at orientation
        }
    else
        m_atOrientation = false;
   
}

} // namespace control
} // namespace ram
