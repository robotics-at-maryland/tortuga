/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockTranslationalController.h
 */

#ifndef RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008
#define RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008

// Project Includes
#include "control/include/ITranslationalController.h"
#include "control/include/IController.h"
#include "core/include/ConfigNode.h"

class MockTranslationalController :
    public ram::control::ITranslationalControllerImp
{
public: 
    MockTranslationalController(ram::core::ConfigNode) :
        timestep(0), linearAcceleration(0, 0, 0),
        orientation(ram::math::Quaternion::IDENTITY),
        force(0, 0, 0)
        {}
    
    virtual ~MockTranslationalController() {}

    virtual ram::math::Vector3 translationalUpdate(
        double timestep_,
        ram::math::Vector3 linearAcceleration_,
        ram::math::Quaternion orientation_,
        ram::math::Vector2 position_,
        ram::math::Vector2 velocity_,
        ram::controltest::DesiredStatePtr desiredState_) {
        timestep = timestep_;
        linearAcceleration = linearAcceleration_;
        orientation = orientation_;
        position = position_;
        velocity = velocity_;
        return force; }

    virtual void setControlMode(ram::control::ControlMode::ModeType mode){}
    virtual ram::control::ControlMode::ModeType getControlMode(){
        return ram::control::ControlMode::OPEN_LOOP;}

    double timestep;
    ram::math::Vector3 linearAcceleration;
    ram::math::Quaternion orientation;
    ram::math::Vector3 force;
    ram::math::Vector2 position;
    ram::math::Vector2 velocity;
};

#endif	// RAM_CONTROL_TEST_TRANSLATIONALCONTROLLER_09_01_2008
