/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee
 * All rights reserved.
 *
 * Authory: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestControlFunctions.cxx
 */

// Library Includes
#include <iostream>
#include <UnitTest++/UnitTest++.h>
//#include <boost/lambda.hpp>

// Project Includes
#include "math/test/include/MathChecks.h"
#include "vehicle/test/include/MockVehicle.h"
#include "control/include/BWPDController.h"
#include "control/test/include/ControllerTests.h"

using namespace ram;

struct Fixture
{
    Fixture() : controller(&vehicle,
                           core::ConfigNode::fromString(
                               "{ 'name' : 'TestController',"
                        	   "'angularPGain' : 10,"
                               "'angularDGain' : 1,"
                               "'desiredQuaternion' : [0, 0, 0, 1] }"))
    {}

    MockVehicle vehicle;
    control::BWPDController controller;
};

TEST_FIXTURE(Fixture, YawControl)
{
    // First is north along horizontal (desired)
    // Rotated 32 degrees from north to the west in horizontal (actual)
    vehicle.orientation = math::Quaternion(0, 0, 0.2756, 0.9613);
    
    math::Vector3 exp_rotTorque(0, 0, -3.5497);
    controller.update(1);
    CHECK_CLOSE(exp_rotTorque, vehicle.torque, 0.0001);
}

TEST_FIXTURE(Fixture, PitchControl)
{
    math::Vector3 exp_rotTorque(0, -2.7872, 0);
    vehicle.orientation = math::Quaternion(0, 0.2164, 0, 0.9763);

    controller.update(1);
    CHECK_CLOSE(exp_rotTorque, vehicle.torque, 0.0001);
}

TEST_FIXTURE(Fixture, RollControl)
{
    math::Vector3 exp_rotTorque(0.7692, 0, 0);
    vehicle.orientation = math::Quaternion(-0.3827, 0, 0, 0.9239);

    controller.update(1);
    CHECK_CLOSE(exp_rotTorque, vehicle.torque, 0.0001);
}

TEST_FIXTURE(Fixture, DepthControl)
{
    math::Vector3 exp_tranForce(0, 0, 0);

    // Test at correct depth response
    controller.setDepth(0);
    vehicle.depth = 0;
    controller.update(1);
    CHECK_CLOSE(exp_tranForce, vehicle.force, 0.0001);

    controller.setDepth(3);
    vehicle.depth = 3;    
    controller.update(1);
    CHECK_CLOSE(exp_tranForce, vehicle.force, 0.0001);

    // Test we need to dive response
    exp_tranForce = math::Vector3(0,0,-5);
    
    controller.setDepth(5);
    vehicle.depth = 0;
    controller.update(1);
    CHECK_CLOSE(exp_tranForce, vehicle.force, 0.0001);
}
