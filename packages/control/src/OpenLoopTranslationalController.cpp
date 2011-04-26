/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/OpenLoopTranslationalController.h
 */

// Project Includes
#include "estimation/include/IStateEstimator.h"
#include "control/include/OpenLoopTranslationalController.h"
#include "control/include/ControllerMaker.h"
#include "control/include/Helpers.h"


namespace ram {
namespace control {

static TranslationalControllerImpMakerTemplate<
    OpenLoopTranslationalController>
registerOpenLoopTranslationalController(
    "OpenLoopTranslationalController");
    
OpenLoopTranslationalController::OpenLoopTranslationalController(
    ram::core::ConfigNode config) :
    TranslationalControllerBase(config),
    m_speedPGain(0),
    m_sidewaysSpeedPGain(0)
{
    // Load things from the configuration file
    m_speedPGain = config["speedPGain"].asDouble(1);
    m_sidewaysSpeedPGain = config["sidewaysSpeedPGain"].asDouble(1);
}
    
math::Vector3 OpenLoopTranslationalController::translationalUpdate(
    double timestep,
    estimation::IStateEstimatorPtr estimator,
    control::DesiredStatePtr desiredState)
{
    math::Quaternion orientation = estimator->getEstimatedOrientation();
    math::Vector2 desiredVelocity = desiredState->getDesiredVelocity();

    // Compute the base force
    math::Vector3 foreAftComponent(
        m_speedPGain * desiredVelocity[0],
        m_sidewaysSpeedPGain * desiredVelocity[1],
        0);

    // Rotate it to the body frame
    math::Vector3 result = orientation * foreAftComponent;

    // Return the results
    return result;
}

} // namespace control
} // namespace ram
