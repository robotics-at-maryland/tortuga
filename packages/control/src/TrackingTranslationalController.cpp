/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/TrackingTranslationalController.cpp
 */

#include <math.h>
#include <iostream>

#include "control/include/TrackingTranslationalController.h"
#include "control/include/ControllerMaker.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix2.h"
#include "control/include/Helpers.h"

namespace ram {
namespace control {

static TranslationalControllerImpMakerTemplate<
    TrackingTranslationalController> 
registerTrackingTranslationalController(
    "TrackingTranslationalController");

TrackingTranslationalController::TrackingTranslationalController(core::ConfigNode config) :
    TranslationalControllerBase(config),
    m_prevPositionError(math::Vector2::ZERO),
    m_prevVelocityError(math::Vector2::ZERO),
    m_iPositionError(math::Vector2::ZERO),
    m_iVelocityError(math::Vector2::ZERO),
    x1kp(0), x1ki(0), x1kd(0),
    x2kp(0), x2ki(0), x2kd(0)
{
    x1kp = config["x1kp"].asDouble(0);
    x1ki = config["x1ki"].asDouble(0);
    x1kd = config["x1kd"].asDouble(0);
    x2kp = config["x2kp"].asDouble(0);
    x2ki = config["x2ki"].asDouble(0);
    x2kd = config["x2kd"].asDouble(0);
}

math::Vector3 TrackingTranslationalController::translationalUpdate(
    double timestep,
    math::Vector3 linearAcceleration,
    math::Quaternion orientation,
    math::Vector2 position,
    math::Vector2 velocity,
    controltest::DesiredStatePtr desiredState)
{
    double yaw = orientation.getYaw().valueRadians(); 

    //Update current position and velocity
    math::Vector2 currentPosition = position;     //assume position in inertial frame
    math::Vector2 currentVelocity = nRb(yaw)*velocity; //rotate velocity to inertial frame

    math::Vector2 desiredPosition = desiredState->getDesiredPosition();
    math::Vector2 desiredVelocity = desiredState->getDesiredVelocity();

    //Based on control mode, modify desired state
    switch(m_controlMode){
    case ControlMode::POSITION:
        desiredVelocity = (desiredPosition - currentPosition)/timestep; 
        desiredState->setDesiredVelocity(desiredVelocity);
        break;
    case ControlMode::VELOCITY:
        desiredPosition += desiredVelocity*timestep;
        desiredState->setDesiredPosition(desiredPosition);
        break;
    case ControlMode::POSITIONANDVELOCITY:
    case ControlMode::OPEN_LOOP:;
    }

    //Initialize error vectors
    math::Vector2 positionPError(0,0), positionIError(0,0), positionDError(0,0);
    math::Vector2 velocityPError(0,0), velocityIError(0,0), velocityDError(0,0);

    //Calculate proportional errors
    positionPError = desiredPosition - currentPosition;
    velocityPError = desiredVelocity - currentVelocity;

    //Calculate derivative errors
    positionDError = (positionPError - m_prevPositionError)/timestep;
    velocityDError = (velocityPError - m_prevVelocityError)/timestep;

    //Calculate integral errors
    positionIError = m_iPositionError + positionPError * timestep;
    velocityIError = m_iVelocityError + velocityPError * timestep;
	 
    //Calculate the signal
    double signal_x1, signal_x2;

    signal_x1 = 
        x1kp * positionPError[0] +
        x1kd * velocityPError[0] +
        x1ki * positionIError[0];

    signal_x2 = 
        x2kp * positionPError[1] +
        x2kd * velocityPError[1] +
        x2ki * positionIError[1];

    math::Vector2 translationalSignal_n(signal_x1,signal_x2);

    //Calculate desired yaw based on direction of translationalSignal_n
    //double desired_yaw = atan(signal_n[1]/signal_n[0]);

    //Rotate translationalSignal_n from the inertial frame to the body frame
    math::Vector2 translationalSignal_b(bRn(yaw)*translationalSignal_n);
	 
    //Store previous error to be used in the next call
    m_prevPositionError = positionPError;
    m_prevVelocityError = velocityPError;

    //Need to return a Vector3
    return math::Vector3(translationalSignal_b[0],
                         translationalSignal_b[1], 0);
}

} // namespace control
} // namespace ram
