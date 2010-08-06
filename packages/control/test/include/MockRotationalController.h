/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/MockRotationalController.h
 */

#ifndef RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008
#define RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008

// Project Includes
#include "control/include/IRotationalController.h"
#include "core/include/ConfigNode.h"

class MockRotationalController :
    public ram::control::IRotationalControllerImp
{
public:
    MockRotationalController(ram::core::ConfigNode) :
        timestep(0),
        orientation(ram::math::Quaternion::IDENTITY),
        angularRate(0, 0, 0),
        torque(0, 0, 0)
        {}
    
    virtual ~MockRotationalController() {}

    virtual ram::math::Vector3 rotationalUpdate(
        double timestep_,
        ram::math::Quaternion orientation_,
        ram::math::Vector3 angularRate_,
        ram::controltest::DesiredStatePtr desiredState_) {
        timestep = timestep_;
        orientation = orientation_;
        angularRate = angularRate_;
        return torque; }

    double timestep;
    ram::math::Quaternion orientation;
    ram::math::Vector3 angularRate;
    ram::math::Vector3 torque;
};

#endif	// RAM_CONTROL_TEST_ROTATIONALCONTROLLER_09_01_2008
