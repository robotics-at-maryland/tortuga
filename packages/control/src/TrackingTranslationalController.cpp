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

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include "estimation/include/IStateEstimator.h"
#include "control/include/TrackingTranslationalController.h"
#include "control/include/ControllerMaker.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix2.h"
#include "control/include/Helpers.h"
#include "math/include/Helpers.h"

// create a category for logging specific depth controller info
static log4cpp::Category& LOGGER(log4cpp::Category::getInstance(
                                     "TranslationalController"));

namespace ram {
namespace control {

static TranslationalControllerImpMakerTemplate<TrackingTranslationalController> 
registerTrackingTranslationalController("TrackingTranslationalController");

TrackingTranslationalController::TrackingTranslationalController(
    core::ConfigNode config) :
    TranslationalControllerBase(config),
    m_iError(math::Vector2::ZERO),
    m_x1kp(0), m_x1ki(0), m_x1kd(0),
    m_x2kp(0), m_x2ki(0), m_x2kd(0),
    m_dtMin(0), m_dtMax(1)
{
    m_x1kp = config["x1kp"].asDouble(0);
    m_x1ki = config["x1ki"].asDouble(0);
    m_x1kd = config["x1kd"].asDouble(0);
    m_x2kp = config["x2kp"].asDouble(0);
    m_x2ki = config["x2ki"].asDouble(0);
    m_x2kd = config["x2kd"].asDouble(0);
    m_dtMin = config["dtmin"].asDouble(0.02);
    m_dtMax = config["dtmax"].asDouble(0.5);
    
    // logging header
    LOGGER.info("PIDTracking dPosition(2) dVelocity(2) dAccel(2) " 
                "ePosition(2) eVelocity(2) mass timestep "
                "pxSig pySig dxSig dySig ixSig iySig accelxSig accelySig"
                "eQuat(4) forces_n(3) forces_b(3)");
}

math::Vector3 TrackingTranslationalController::translationalUpdate(
    double timestep,
    estimation::IStateEstimatorPtr estimator,
    control::DesiredStatePtr desiredState)
{
    // assume position and velocity are in inertial frame
    math::Vector2 ePosition = estimator->getEstimatedPosition();
    math::Vector2 eVelocity = estimator->getEstimatedVelocity();

    math::Vector2 dPosition = desiredState->getDesiredPosition();
    math::Vector2 dVelocity = desiredState->getDesiredVelocity();
    math::Vector2 dAccel = desiredState->getDesiredAccel();

    math::Quaternion orientation = estimator->getEstimatedOrientation();
    double mass = estimator->getEstimatedMass();

    // propagate the desired state
    dPosition += dVelocity * timestep;
    desiredState->setDesiredPosition(dPosition);

    // make sure timestep is not to large or small
    if(timestep < m_dtMin)
        timestep = m_dtMin;
    if(timestep > m_dtMax)
        timestep = m_dtMax;

    // calculate error terms
    math::Vector2 pError = dPosition - ePosition;
    math::Vector2 dError = dVelocity - eVelocity;
    math::Vector2 iError = m_iError + pError * timestep;
    m_iError = iError;
    
    // calculate the signal
    double signal_x1 = 
        m_x1kp * pError[0] +
        m_x1kd * dError[0] +
        m_x1ki * iError[0] +
        mass * dAccel[0];

    double signal_x2 = 
        m_x2kp * pError[1] +
        m_x2kd * dError[1] +
        m_x2ki * iError[1] +
        mass * dAccel[1];

    // we need to return a Vector3
    math::Vector3 controlSignal_n(signal_x1, signal_x2, 0);
    // rotate the control signal to the body frame
    math::Vector3 controlSignal_b = orientation * controlSignal_n;

    // log everything we could possibly want to know
    LOGGER.infoStream() << dPosition[0] << " "
                        << dPosition[1] << " "
                        << dVelocity[0] << " "
                        << dVelocity[1] << " "
                        << dAccel[0] << " "
                        << dAccel[1] << " "
                        << ePosition[0] << " "
                        << ePosition[1] << " "
                        << eVelocity[0] << " "
                        << eVelocity[1] << " "
                        << mass << " "
                        << timestep << " "
                        << m_x1kp * pError[0] << " "
                        << m_x2kp * pError[1] << " "
                        << m_x1kd * dError[0] << " "
                        << m_x2kd * dError[1] << " "
                        << m_x1ki * iError[0] << " "
                        << m_x2ki * iError[1] << " "
                        << mass * dAccel[0] << " "
                        << mass * dAccel[1] << " "
                        << orientation[0] << " "
                        << orientation[1] << " "
                        << orientation[2] << " "
                        << orientation[3] << " "
                        << controlSignal_n[0] << " "
                        << controlSignal_n[1] << " "
                        << controlSignal_n[2] << " "
                        << controlSignal_b[0] << " "
                        << controlSignal_b[1] << " "
                        << controlSignal_b[2];


    return controlSignal_b;
}

} // namespace control
} // namespace ram
