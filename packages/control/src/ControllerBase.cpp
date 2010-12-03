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
#include "control/include/DesiredState.h"

#include "vehicle/include/IVehicle.h"
#include "estimation/include/IStateEstimator.h"

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

ControllerBase::ControllerBase(core::EventHubPtr eventHub,
                               vehicle::IVehiclePtr vehicle,
                               estimation::IStateEstimatorPtr estimator,
                               core::ConfigNode config) :
    IController(config["name"].asString(), eventHub),
    m_desiredState(control::DesiredStatePtr(
                       new control::DesiredState(config["DesiredState"], eventHub))),
    m_stateEstimator(estimator),
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
    m_initHoldHeading(1),
    m_initHoldPosition(1)
{   
    init(config, eventHub); 
}

ControllerBase::ControllerBase(core::ConfigNode config,
                               core::SubsystemList deps) :
    IController(config["name"].asString(),
                core::Subsystem::getSubsystemOfType<core::EventHub>(deps)),
    m_desiredState(control::DesiredStatePtr(
                     new control::DesiredState(
                         config["DesiredState"],
                         core::Subsystem::getSubsystemOfType<core::EventHub>(deps)))),
    m_stateEstimator(core::Subsystem::getSubsystemOfType<estimation::IStateEstimator>(deps)),
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
    m_initHoldHeading(1),
    m_initHoldPosition(1)
{
    core::EventHubPtr eventHub = 
        core::Subsystem::getSubsystemOfType<core::EventHub>(deps);

    init(config, eventHub); 
}

ControllerBase::~ControllerBase()
{
    unbackground(true);

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
}

void ControllerBase::update(double timestep)
{
    // Run the base values
    math::Vector3 translationalForce(0,0,0);
    math::Vector3 rotationalTorque(0,0,0);

    doUpdate(timestep, translationalForce, rotationalTorque);

    // Actually set motor values
    m_vehicle->applyForcesAndTorques(translationalForce, rotationalTorque);
}



void ControllerBase::translate(math::Vector2 position, math::Vector2 velocity)
{
    m_desiredState->setDesiredVelocity(velocity);
    m_desiredState->setDesiredPosition(position);
}

void ControllerBase::changeDepth(double depth, double rate)
{
    m_desiredState->setDesiredDepth(depth);
    m_desiredState->setDesiredDepthRate(rate);
}

void ControllerBase::rotate(math::Quaternion orientation, math::Vector3 angularRate)
{
    m_desiredState->setDesiredOrientation(orientation);
    m_desiredState->setDesiredAngularRate(angularRate);
}

void ControllerBase::yawVehicle(double degrees, double rate)
{
    rotate(yawVehicleHelper(m_desiredState->getDesiredOrientation(),degrees),
           math::Vector3::UNIT_Z);
}

void ControllerBase::pitchVehicle(double degrees, double rate)
{
    rotate(pitchVehicleHelper(m_desiredState->getDesiredOrientation(),degrees),
           math::Vector3::UNIT_Y);
}

void ControllerBase::rollVehicle(double degrees, double rate)
{
    rotate(rollVehicleHelper(m_desiredState->getDesiredOrientation(),degrees),
           math::Vector3::UNIT_X);
}




math::Vector2 ControllerBase::getDesiredPosition()
{
    return m_desiredState->getDesiredPosition();
}

math::Vector2 ControllerBase::getDesiredVelocity()
{
    return m_desiredState->getDesiredVelocity();
}

math::Quaternion ControllerBase::getDesiredOrientation()
{
    return m_desiredState->getDesiredOrientation();
}

math::Vector3 ControllerBase::getDesiredAngularRate()
{
    return m_desiredState->getDesiredAngularRate();
}

double ControllerBase::getDesiredDepth()
{
    return m_desiredState->getDesiredDepth();
}

double ControllerBase::getDesiredDepthRate()
{
    return m_desiredState->getDesiredDepthRate();
}



void ControllerBase::holdCurrentHeading()
{
    math::Quaternion qCurr(m_stateEstimator->getEstimatedOrientation());
    math::Quaternion qHeading = holdCurrentHeadingHelper(qCurr);

    rotate(qHeading, math::Vector3::ZERO);
}

void ControllerBase::holdCurrentOrientation()
{    
    rotate(m_stateEstimator->getEstimatedOrientation(), 
           math::Vector3::ZERO);
}

void ControllerBase::holdCurrentPosition()
{
    translate(m_stateEstimator->getEstimatedPosition(),
              math::Vector2::ZERO);
}

void ControllerBase::holdCurrentDepth()
{
    changeDepth(m_stateEstimator->getEstimatedDepth(), 0);
}



bool ControllerBase::atPosition()
{
    math::Vector2 currentPosition = m_stateEstimator->getEstimatedPosition();
    math::Vector2 desiredPosition = m_desiredState->getDesiredPosition();
    double difference0 = fabs(currentPosition[0] - desiredPosition[0]);
    double difference1 = fabs(currentPosition[1] - desiredPosition[1]);
    return difference0 <= m_positionThreshold && difference1 <= m_positionThreshold;
}

bool ControllerBase::atVelocity()
{
    math::Vector2 currentVelocity = m_stateEstimator->getEstimatedVelocity();
    math::Vector2 desiredVelocity = m_desiredState->getDesiredVelocity();
    double difference0 = fabs(currentVelocity[0] - desiredVelocity[0]);
    double difference1 = fabs(currentVelocity[1] - desiredVelocity[1]);
    return difference0 <= m_velocityThreshold && difference1 <= m_velocityThreshold;
}

bool ControllerBase::atDepth()
{
 
    double currentDepth = m_stateEstimator->getEstimatedDepth();
    double desiredDepth = m_desiredState->getDesiredDepth();
    double difference = fabs(currentDepth - desiredDepth);
    return difference <= m_depthThreshold;
}

bool ControllerBase::atOrientation()
{
    // Find the quaternion error between the two
    math::Quaternion error = m_desiredState->getDesiredOrientation().errorQuaternion(
        m_stateEstimator->getEstimatedOrientation());

    // This does the sqrt of the sum of the squares for the first three elements
    math::Vector3 tmp(error.ptr());
    if (tmp.length() > m_orientationThreshold)
        return false;
    else
        return true;
}



void ControllerBase::init(core::ConfigNode config, 
                          core::EventHubPtr eventHub)
{
    if(eventHub != core::EventHubPtr()) {

    /* Bind atDepth, atPosition, atVelocity, and atOrientation to desired state 
       and estimated state update events */

        conn_desired_atDepth = eventHub->subscribeToType(
            IController::DESIRED_DEPTH_UPDATE,
            boost::bind(&ControllerBase::atDepthUpdate,this,_1));

        conn_estimated_atDepth = eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_DEPTH_UPDATE,
            boost::bind(&ControllerBase::atDepthUpdate,this,_1));

        conn_desired_atPosition = eventHub->subscribeToType(
            IController::DESIRED_POSITION_UPDATE,
            boost::bind(&ControllerBase::atPositionUpdate,this,_1));

        conn_estimated_atPosition = eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_POSITION_UPDATE,
            boost::bind(&ControllerBase::atPositionUpdate,this,_1));

        conn_desired_atVelocity = eventHub->subscribeToType(
            IController::DESIRED_VELOCITY_UPDATE,
            boost::bind(&ControllerBase::atVelocityUpdate,this,_1));

        conn_estimated_atVelocity = eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_VELOCITY_UPDATE,
            boost::bind(&ControllerBase::atVelocityUpdate,this,_1));

        conn_desired_atOrientation = eventHub->subscribeToType(
            IController::DESIRED_ORIENTATION_UPDATE,
            boost::bind(&ControllerBase::atOrientationUpdate,this,_1));

        conn_estimated_atOrientation = eventHub->subscribeToType(
            estimation::IStateEstimator::ESTIMATED_ORIENTATION_UPDATE,
            boost::bind(&ControllerBase::atOrientationUpdate,this,_1));       
    }

    // Load threshold for being at depth
    m_depthThreshold = config["depthThreshold"].asDouble(DEPTH_THRESHOLD);
    m_orientationThreshold =
        config["orientationThreshold"].asDouble(ORIENTATION_THRESHOLD);
    m_positionThreshold = config["positionThreshold"].asDouble(POSITION_THRESHOLD);
    m_velocityThreshold = config["velocityThreshold"].asDouble(VELOCITY_THRESHOLD);

    m_initHoldDepth = config["holdCurrentDepth"].asInt(1);
    m_initHoldHeading = config["holdCurrentHeading"].asInt(1);
    m_initHoldPosition = config["holdCurrentPosition"].asInt(1);

    if(m_initHoldDepth)
        holdCurrentDepth();
    if(m_initHoldHeading)
        holdCurrentHeading();
    if(m_initHoldPosition)
        holdCurrentPosition();
}
                      
void ControllerBase::publishAtDepth(const double& depth)
{
    math::NumericEventPtr event(new math::NumericEvent());
    event->number = depth;
    publish(IController::AT_DEPTH, event);
}

void ControllerBase::publishAtOrientation(const math::Quaternion& orientation)
{
    math::OrientationEventPtr event(new math::OrientationEvent());
    event->orientation = orientation;
    publish(IController::AT_ORIENTATION, event);        
}


void ControllerBase::publishAtVelocity(const math::Vector2& velocity)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = velocity;
    publish(IController::AT_VELOCITY, event);
}

void ControllerBase::publishAtPosition(const math::Vector2& position)
{
    math::Vector2EventPtr event(new math::Vector2Event());
    event->vector2 = position;
    publish(IController::AT_POSITION, event);
}

void ControllerBase::atDepthUpdate(core::EventPtr event)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    if(atDepth())
        if(!m_atDepth){
            m_atDepth = true;
            publishAtDepth(m_desiredState->getDesiredDepth());
        } else {
            // dont publish if we were already at depth
        }
    else
        m_atDepth = false;
}

void ControllerBase::atPositionUpdate(core::EventPtr event)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    if(atPosition())
        if(!m_atPosition){
            m_atPosition = true;
            publishAtPosition(m_desiredState->getDesiredPosition());
        } else {
            // dont publish if we were already at position
        }
    else
        m_atPosition = false;
}

void ControllerBase::atVelocityUpdate(core::EventPtr event)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    if(atVelocity())
        if(!m_atVelocity){
            m_atVelocity = true;
            publishAtVelocity(m_desiredState->getDesiredVelocity());
        } else {
            //dont publish if we were already at position
        }
    else
        m_atVelocity = false;
}

void ControllerBase::atOrientationUpdate(core::EventPtr event)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    if(atOrientation())
        if(!m_atOrientation){
            m_atOrientation = true;
            publishAtOrientation(m_desiredState->getDesiredOrientation());
        } else {
            // don't publish if we were already at orientation
        }
    else
        m_atOrientation = false;
}

} // namespace control
} // namespace ram
