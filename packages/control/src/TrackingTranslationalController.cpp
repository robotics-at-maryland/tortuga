/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/TrackingTranslationalController.cpp
 */

// STD Includes
#include <math.h>
#include <iostream>

// Project Includes
#include "estimation/include/IStateEstimator.h"
#include "control/include/TrackingTranslationalController.h"
#include "control/include/ControllerMaker.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix2.h"
#include "control/include/Helpers.h"
#include "math/include/Helpers.h"

namespace ram {
namespace control {

static TranslationalControllerImpMakerTemplate<
    TrackingTranslationalController> 
registerTrackingTranslationalController(
    "TrackingTranslationalController");

TrackingTranslationalController::TrackingTranslationalController(core::ConfigNode config) :
    TranslationalControllerBase(config),
    m_iError(math::Vector2::ZERO),
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
    estimation::IStateEstimatorPtr estimator,
    control::DesiredStatePtr desiredState)
{
    math::Quaternion orientation = estimator->getEstimatedOrientation();
    double yaw = orientation.getYaw().valueRadians(); 

    // update current position and velocity
    // assume position in inertial frame
    math::Vector2 currentPosition = estimator->getEstimatedPosition();
    // rotate velocity to inertial frame
    math::Vector2 currentVelocity = math::nRb(yaw)*estimator->getEstimatedVelocity();

    math::Vector2 desiredPosition = desiredState->getDesiredPosition();
    math::Vector2 desiredVelocity = desiredState->getDesiredVelocity();

    // propagate the desired state
    desiredPosition += desiredVelocity * timestep;
        
    // initialize error vectors
    math::Vector2 pError(0,0), dError(0,0), iError(0,0);

    // calculate error terms
    pError = desiredPosition - currentPosition;
    dError = desiredVelocity - currentVelocity;
    iError = m_iError + pError * timestep;
    
    // calculate the signal
    double signal_x1, signal_x2;

    signal_x1 = 
        x1kp * pError[0] +
        x1kd * dError[0] +
        x1ki * iError[0];

    signal_x2 = 
        x2kp * pError[1] +
        x2kd * dError[1] +
        x2ki * iError[1];

    math::Vector2 translationalSignal_n(signal_x1,signal_x2);

    // rotate translationalSignal_n from the inertial frame to the body frame
    math::Vector2 translationalSignal_b(math::bRn(yaw)*translationalSignal_n);
    
    // the quaternion with just the pitch
    math::Quaternion quatPitch(math::Degree(orientation.Inverse().getPitch()),
                               math::Vector3::UNIT_Y);

    // need to return a Vector3
    return quatPitch * math::Vector3(translationalSignal_b[0],
                                     translationalSignal_b[1], 0);

}

} // namespace control
} // namespace ram
