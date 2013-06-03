/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/NonlinearLinearVelocityAdaptivePDRotationalController.cpp
 */

// Library Includes
#include <iostream>
#include <log4cpp/Category.hh>

// Project Includes
#include "control/include/NonlinearLinearVelocityAdaptivePDRotationalController.h"
#include "control/include/ControllerMaker.h"
#include "math/include/Matrix3.h"
#include "math/include/Helpers.h"
#include "math/include/Math.h"
#include "estimation/include/IStateEstimator.h"

// create a category for logging specific depth controller info
static log4cpp::Category& LOGGER(log4cpp::Category::getInstance(
                                     "RotationalController"));

namespace ram {
namespace control {

static RotationalControllerImpMakerTemplate<NonlinearLinearVelocityAdaptivePDRotationalController>
registerNonlinearLinearVelocityAdaptivePDRotationalController("NonlinearLinearVelocityAdaptivePDRotationalController");


//This variant on the usual rotation controller makes use of a set of adaptive terms to
//attempt to compensate for linear drag effects on rotation
NonlinearLinearVelocityAdaptivePDRotationalController::NonlinearLinearVelocityAdaptivePDRotationalController(
    core::ConfigNode config) :
    RotationalControllerBase(config),
    angularPGain(config["kp"].asDouble(0)),
    angularDGain(config["kd"].asDouble(0)),
    inertiaEstimate(math::Matrix3(config["inertia"][0][0].asDouble(0.201),
                                  config["inertia"][0][1].asDouble(0),
                                  config["inertia"][0][2].asDouble(0),
                                  config["inertia"][1][0].asDouble(0),
                                  config["inertia"][1][1].asDouble(1.288),
                                  config["inertia"][1][2].asDouble(0),
                                  config["inertia"][2][0].asDouble(0),
                                  config["inertia"][2][1].asDouble(0),
                                  config["inertia"][2][2].asDouble(1.288))),
    adaptTerms(math::Vector3(config["adaptTerms"][0][0].asDouble(.01),
                             config["adaptTerms"][1][0].asDouble(.01),
                             config["adaptTerms"][2][0].asDouble(0)))
{
    // logging header
    LOGGER.info("NonlinarPD dQuat(4) eQuat(4) pTerm(3) dTerm(3) "
                "gyroTerm(3) torques(3)");
}


math::Vector3 NonlinearLinearVelocityAdaptivePDRotationalController::rotationalUpdate(
    double timestep,
    estimation::IStateEstimatorPtr estimator,
    control::DesiredStatePtr desiredState)
{
    // get estimated and desired orientation and assure they are normalized
    math::Quaternion q_meas(estimator->getEstimatedOrientation());
    q_meas.normalise();
    math::Quaternion q_des(desiredState->getDesiredOrientation());
    q_des.normalise();

    // compute error quaternion
    math::Quaternion q_tilde = q_meas.errorQuaternion(q_des);  

    // break up quaternion into vector and scalar parts for later convenience
    math::Vector3 epsilon_tilde(q_tilde.x, q_tilde.y, q_tilde.z);
    double eta_tilde = q_tilde.w;

    // compute angular rate error
    math::Vector3 w_error(estimator->getEstimatedAngularRate());

    // compute matrix needed for gyroscopic term
    math::Matrix3 w_tilde;
    w_tilde.ToSkewSymmetric(w_error);

    // compute control signal
    double kp = angularPGain;
    double kd = angularDGain;
    math::Vector3 rotTorques = 
        - kp * inertiaEstimate * (math::sign(eta_tilde)) * epsilon_tilde
        - kd * inertiaEstimate * w_error
        + w_tilde * inertiaEstimate * w_error;
    
    math::Vector3 pTerm = - kp * inertiaEstimate * 
                           (math::sign(eta_tilde)) * epsilon_tilde;
    math::Vector3 dTerm = - kd * inertiaEstimate * w_error;
    math::Vector3 gTerm = w_tilde * inertiaEstimate * w_error;

    //compute linear drag effect compensation
    //generate velocity vector
    math::Vector3 velVec(estimator->getEstimatedVelocity().x,
                         estimator->getEstimatedVelocity().y,
                         estimator->getEstimatedDepthRate());
    //rotate velocity vector into the body frame
    math::Vector3 velBod = q_meas*velVec;
    //generate body acceleration vector
    math::Vector3 accBod = q_meas*estimator->getEstimatedLinearAcceleration();
    //generate desired body acceleration
    math::Vector3 accDes = q_des*estimator->getEstimatedLinearAcceleration();
    //generate psuedoinverse(formula is fixed for skew-symmetric matrix, which this is of)
    math::Matrix3 pinvMat;
    pinvMat.ToSkewSymmetric(velBod);
    //thresholds the psuedo inverse, if too small then does not attempt adaption
    if((velBod.x*velBod.x+velBod.y*velBod.y+velBod.z*velBod.z)>.1)
    {
        pinvMat = -pinvMat*(math::Radian(1)/((double)(velBod.x*velBod.x+velBod.y*velBod.y+velBod.z*velBod.z))).valueRadians();
    }
    else
    {
        pinvMat = math::Matrix3::ZERO;
    }
    //generate B vector
    math::Vector3 B = adaptTerms.crossProduct(accDes) - adaptTerms.crossProduct(accBod);
    //"Solve" linear equation, actually a minimal least squares estimate
    math::Vector3 adaptDot = pinvMat*B;
    //update adaptation terms
    adaptTerms = adaptTerms + adaptDot*timestep;
    //finally, compute correct using adaptive terms
    rotTorques = rotTorques + adaptTerms.crossProduct(velBod);
    // log everything we might want to know
    LOGGER.infoStream() << q_des[0] << " "
                        << q_des[1] << " "
                        << q_des[2] << " "
                        << q_des[3] << " "
                        << q_meas[0] << " "
                        << q_meas[1] << " "
                        << q_meas[2] << " "
                        << q_meas[3] << " "
                        << pTerm[0] << " "
                        << pTerm[1] << " "
                        << pTerm[2] << " "
                        << dTerm[0] << " "
                        << dTerm[1] << " "
                        << dTerm[2] << " "
                        << gTerm[0] << " "
                        << gTerm[1] << " "
                        << gTerm[2] << " "
                        << adaptTerms[0] << " "
                        << adaptTerms[1] << " "
                        << adaptTerms[2] << " "
                        << rotTorques[0] << " "
                        << rotTorques[1] << " "
                        << rotTorques[2];

    return rotTorques;
}

} // namespace control
} // namespace ram
