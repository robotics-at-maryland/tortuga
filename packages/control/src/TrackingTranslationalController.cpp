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
    std::cout << " In TrackingTranslationalController Constructor" << std::endl;
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
    math::Vector2 velocity)
{
    double yaw = orientation.getYaw().valueRadians(); 

    // Only call cos and sin once
    double yaw_cos = cos(yaw), yaw_sin = sin(yaw);
	 
    //Compute rotation matrix from inertial frame to body frame
    math::Matrix2 bRn(yaw_cos,yaw_sin,-yaw_sin,yaw_cos);
    //Compute rotation matrix from body frame to inertial frame
    math::Matrix2 nRb(yaw_cos,-yaw_sin,yaw_sin,yaw_cos);

    //Update current position and velocity
    m_currentPosition = position;     //assume position in inertial frame
    m_currentVelocity = nRb*velocity; //rotate velocity to inertial frame
	 
    //Initialize error vectors
    math::Vector2 positionPError(0,0), positionIError(0,0), positionDError(0,0);
    math::Vector2 velocityPError(0,0), velocityIError(0,0), velocityDError(0,0);

    //Calculate proportional errors
    positionPError = m_desiredPosition - m_currentPosition;
    velocityPError = m_desiredVelocity - m_currentVelocity;

    //Calculate derivative errors
    positionDError = (positionPError - m_prevPositionError)/timestep;
    velocityDError = (velocityPError - m_prevVelocityError)/timestep;

    //Calculate integral errors
    positionIError = m_iPositionError + positionPError * timestep;
    velocityIError = m_iVelocityError + velocityPError * timestep;
	 
    //Calculate control signals in inertial frame
    double positionSig_x1, positionSig_x2;
    double velocitySig_x1, velocitySig_x2;

    positionSig_x1 = 
        x1kp * positionPError[0] +
        x1kd * positionDError[0] +
        x1ki * positionIError[0];

    positionSig_x2 = 
        x2kp * positionPError[1] +
        x2kd * positionDError[1] +
        x2ki * positionIError[1];

    velocitySig_x1 = 
        x1kp * velocityPError[0] +
        x1kd * positionDError[0] +
        x1ki * positionIError[0];

    velocitySig_x2 = 
        x2kp * velocityPError[1] +
        x2kd * positionDError[1] +
        x2ki * positionIError[1];

    //Put signals into an array so we can make a Vector2 
    double signal_n[2] = {0,0};

    //Decide what signal to use based on control mode
    switch(m_controlMode){
    case ControlMode::POSITION:
        signal_n[0] = positionSig_x1;
        signal_n[1] = positionSig_x2;
        break;
    case ControlMode::VELOCITY:
        signal_n[0] = velocitySig_x1;
        signal_n[1] = velocitySig_x2;
        break;
    case ControlMode::POSITIONANDVELOCITY:
        signal_n[0] = positionSig_x1 + velocitySig_x1;
        signal_n[1] = positionSig_x2 + velocitySig_x2;
        break;
    case ControlMode::OPEN_LOOP:
        break;
    }

    math::Vector2 translationalSignal_n(signal_n);

    //Calculate desired yaw based on direction of translationalSignal_n
    //double desired_yaw = atan(signal_n[1]/signal_n[0]);

    //Rotate translationalSignal_n from the inertial frame to the body frame
    math::Vector2 translationalSignal_b(bRn*translationalSignal_n);
	 
    //Store previous error to be used in the next call
    m_prevPositionError = positionPError;
    m_prevVelocityError = velocityPError;


    //Need to return a Vector3
	 
    return math::Vector3(translationalSignal_b[0],
                         translationalSignal_b[1], 0);
}

} // namespace control
} // namespace ram
