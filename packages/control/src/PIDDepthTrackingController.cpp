/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/PIDDepthTrackingController.cpp
 */

// Library Includes
#include <log4cpp/Category.hh>

// Project Includes
#include <cmath>
#include <stdlib.h>
#include "control/include/PIDDepthTrackingController.h"
#include "control/include/ControllerMaker.h"
#include "estimation/include/IStateEstimator.h"

// create a category for logging specific depth controller info
static log4cpp::Category& LOGGER(log4cpp::Category::getInstance(
                                     "DepthController"));

namespace ram {
namespace control {

static DepthControllerImpMakerTemplate<PIDDepthTrackingController>
registerPIDDepthTrackingController("PIDDepthTrackingController");

PIDDepthTrackingController::PIDDepthTrackingController(
    ram::core::ConfigNode config) :
    DepthControllerBase(config),
    m_iErr(0),
    m_kp(config["kp"].asDouble(0)),
    m_kd(config["kd"].asDouble(0)),
    m_ki(config["ki"].asDouble(0)),
    m_dtMin(config["dtMin"].asDouble(0.02)),
    m_dtMax(config["dtMax"].asDouble(0.5))
{
    // logging header
    LOGGER.info("PIDTracking dDepth dRate dAccel eDepth eRate eQuat(4) "
                "mass timestep pSig dSig iSig accelSig "
                "force_n(3) force_b(3)");
}

math::Vector3 PIDDepthTrackingController::depthUpdate(
    double timestep,
    estimation::IStateEstimatorPtr estimator,
    control::DesiredStatePtr desiredState)
{
    // get desired and estimated quantities
    double dDepth = desiredState->getDesiredDepth();
    double dRate = desiredState->getDesiredDepthRate();
    double dAccel = desiredState->getDesiredDepthAccel();

    double eDepth = estimator->getEstimatedDepth();
    double eRate = estimator->getEstimatedDepthRate();

    math::Quaternion orientation = estimator->getEstimatedOrientation();
    double mass = estimator->getEstimatedMass();

    // propagate desired state
    dRate += dAccel * timestep;
    dDepth += dRate * timestep;
//    desiredState->setDesiredDepthRate(dRate);
//    desiredState->setDesiredDepth(dDepth);
    
    // make sure timestep is not to large or small
    if(timestep < m_dtMin)
        timestep = m_dtMin;
    if(timestep > m_dtMax)
        timestep = m_dtMax;

    // calculate error terms for PID
    double pErr = eDepth - dDepth;
    double dErr = eRate - dRate;
    double iErr = m_iErr + pErr * timestep;
    m_iErr = iErr;

    double depthControlSignal = 
        m_kp * pErr + m_kd * dErr + m_ki * iErr;
    // this term causes bad performace due buoyancy and inertia - mass * dAccel;

    // we need to return a Vector3
    math::Vector3 controlSignal_n(0, 0, depthControlSignal);
    // rotate the control signal into the body frame
    math::Vector3 controlSignal_b = orientation * controlSignal_n;

    // log everything we could possibly want to know
    LOGGER.infoStream() << dDepth << " "
                        << dRate << " "
                        << dAccel << " "
                        << eDepth << " "
                        << eRate << " "
                        << orientation[0] << " "
                        << orientation[1] << " "
                        << orientation[2] << " "
                        << orientation[3] << " "
                        << mass << " "
                        << timestep << " "
                        << m_kp * pErr << " "
                        << m_kd * dErr << " "
                        << m_ki * iErr << " "
                        << mass * dAccel << " "
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
