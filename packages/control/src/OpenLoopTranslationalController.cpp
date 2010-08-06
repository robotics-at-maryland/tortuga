/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/OpenLoopTranslationalController.h
 */

// Project Includes
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
    m_speedPGain = config["speedPGain"].asInt(1);
    m_sidewaysSpeedPGain = config["sidewaysSpeedPGain"].asInt(1);
}
    
math::Vector3 OpenLoopTranslationalController::translationalUpdate(
    double timestep,
    math::Vector3 linearAcceleration,
    math::Quaternion orientation,
    math::Vector2 position,
    math::Vector2 velocity,
    controltest::DesiredStatePtr desiredState)
{
    // The quaternion with just the pitch
    math::Quaternion quatPitch(math::Degree(orientation.Inverse().getPitch()),
                               math::Vector3::UNIT_Y);

    /*  Retrieve velocity as if the robots frame is the inertial frame.  
        In other words, the robot should always travel at this velocity 
        relative to itself.  Doing this, the robot's actual velocity in
        the true inertial frame will change as its orientation changes. */
    math::Vector2 desiredVelocity = desiredState->getDesiredVelocity();

    // Compute the base force
    math::Vector3 foreAftComponent(
        m_speedPGain * desiredVelocity[0],
        m_sidewaysSpeedPGain * desiredVelocity[1],
        0);

    // Rotate it to account for odd pitches
    math::Vector3 result = quatPitch * foreAftComponent;

    // Return the results
    return result;
}

} // namespace control
} // namespace ram
