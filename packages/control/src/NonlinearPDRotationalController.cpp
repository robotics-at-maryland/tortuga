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

// Project Includes
#include "control/include/NonlinearPDRotationalController.h"
#include "control/include/ControllerMaker.h"
#include "math/include/Matrix3.h"
#include "math/include/Helpers.h"

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
{}


math::Vector3 NonlinearPDRotationalController::rotationalUpdate(
    double timestep,
    math::Quaternion orientation,
    math::Vector3 angularRate,
    controltest::DesiredStatePtr desiredState)
{
    math::Quaternion desiredOrientation = desiredState->getDesiredOrientation();

    //compute error quaternion
    math::Quaternion q_tilde;
    math::Quaternion q_meas(orientation);
    math::Quaternion q_des(desiredOrientation);
    q_tilde = q_meas.errorQuaternion(q_des);  

    //break up quaternion into vector and scalar parts for later convenience
    math::Vector3 epsilon_tilde(q_tilde.x, q_tilde.y, q_tilde.z);
    double eta_tilde = q_tilde.w;

    //compute angular rate error
    math::Vector3 w_error(angularRate[0],
                          angularRate[1],
                          angularRate[2]);

    //compute matrix needed for gyroscopic term
    math::Matrix3 w_tilde;
    w_tilde.ToSkewSymmetric(w_error);

    //compute control signal
    math::Vector3 rotTorques;
    double kp = angularPGain;
    double kd = angularDGain;
    rotTorques = - kp*inertiaEstimate*(math::sign(eta_tilde))*epsilon_tilde
        - kd*inertiaEstimate*w_error + w_tilde*inertiaEstimate*w_error;
    
    return rotTorques;
}

} // namespace control
} // namespace ram
