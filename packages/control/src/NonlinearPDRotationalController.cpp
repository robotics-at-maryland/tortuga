/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/src/NonlinearPDRotationalController.cpp
 */

// Library Includes
#include <iostream>
#include <log4cpp/Category.hh>

// Project Includes
#include "control/include/NonlinearPDRotationalController.h"
#include "control/include/ControllerMaker.h"
#include "math/include/Matrix3.h"
#include "math/include/Helpers.h"
#include "estimation/include/IStateEstimator.h"

// create a category for logging specific depth controller info
static log4cpp::Category& LOGGER(log4cpp::Category::getInstance(
                                     "RotationalController"));

namespace ram {
namespace control {

static RotationalControllerImpMakerTemplate<NonlinearPDRotationalController>
registerNonlinearPDRotationalController("NonlinearPDRotationalController");

NonlinearPDRotationalController::NonlinearPDRotationalController(
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
                                  config["inertia"][2][2].asDouble(1.288)))
{
    // logging header
    LOGGER.info("NonlinarPD dQuat(4) eQuat(4) pTerm(3) dTerm(3) "
                "gyroTerm(3) torques(3)");
}


math::Vector3 NonlinearPDRotationalController::rotationalUpdate(
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
                        << rotTorques[0] << " "
                        << rotTorques[1] << " "
                        << rotTorques[2];

    return rotTorques;
}

} // namespace control
} // namespace ram
