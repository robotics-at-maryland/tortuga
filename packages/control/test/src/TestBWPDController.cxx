/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee
 * All rights reserved.
 *
 * Authory: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestControlFunctions.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vehicle/test/include/MockVehicle.h"
#include "control/include/BWPDController.h"

using namespace ram;

struct Fixture
{
    Fixture() : controller(&vehicle,
                           core::ConfigNode::fromString(
                               "{ angularPGain: 10"
                               "angularDGain: 1"
                               "desiredQuaternion: [0, 0, 0, 1] }"))
    {}

    MockVehicle vehicle;
    control::IController controller;
};

TEST_FIXTURE(Fixture, BWPDController)
{
    // First is north along horizontal (desired)
    // Rotated 32 degrees from north to the west in horizontal (actual)

    /*
    
    // Quat = 0,0,0,1 and everything else zero
    control::DesiredState desired = {0, // speed
                                     0, // depth
                                     {0, 0, 0, 1}, // quat
                                     {0} // angularRate
    };

    control::MeasuredState measured = {0,// depth
                              {0},  // Accel
                              {0}, // Mag
                              {0, 0, 0.2756, 0.9613}, // Quat
                              {0}}; // Ang Rate
    
    control::ControllerState state = {10, // angP gain
                                1, // angD gain
                                {{0.201, 0, 0}, // inertia estimates
                                 {0, 1.288, 0},
                                 {0, 0, 1.288}},
                                0, // depthP gain (not used)
                                0};// speedP gain (not used)

    // Testing Yaw Control
    double exp_rotTorques[3] = {0, 0, -3.5497};
    double act_rotTorques[3] = {0};
    control::BongWiePDRotationalController(&measured, &desired, &state,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques, act_rotTorques, 3, 0.0001);


    // Testing pitch Control
    double exp_rotTorques2[3] = {0, -2.7872, 0};
    control::MeasuredState measured2 = {0, {0}, {0}, 
                                       {0, 0.2164, 0, 0.9763},
                                       {0}};
    control::BongWiePDRotationalController(&measured2, &desired, &state,
                                           1, act_rotTorques);
    CHECK_ARRAY_CLOSE(exp_rotTorques2, act_rotTorques, 3, 0.0001);

    // Testing Roll Control
    double exp_rotTorques3[3] = {0.7692, 0, 0};
    control::MeasuredState measured3 = {0, {0}, {0}, 
                                        {-0.3827, 0, 0, 0.9239},
                                       {0}};
    control::BongWiePDRotationalController(&measured3, &desired, &state,
                                           1, act_rotTorques);
                                           CHECK_ARRAY_CLOSE(exp_rotTorques3, act_rotTorques, 3, 0.0001);*/
    CHECK(true)
}

TEST_FIXTURE(Fixture, doIsOriented)
{
    CHECK(true);
    /*
    // Not matching
    control::DesiredState desiredBad = {0, 0, {0.0872, 0, 0, 0.9962}, {0}};

    control::MeasuredState measuredBad = {0, {0}, {0},
                                          {0.2588, 0, 0, 0.9659},
                                          {0}};

    CHECK_EQUAL(false, doIsOriented(&measuredBad, &desiredBad, 0.15));

    // Matching
    control::DesiredState desiredGood = {0, 0, {0.0872, 0, 0, 0.9962}, {0}};
    control::MeasuredState measuredGood = {0, {0}, {0},
                                           {0.173, 0.0858, -0.0151, 0.9811},
                                          {0}};
    
                                          CHECK_EQUAL(true, doIsOriented(&measuredGood, &desiredGood, 0.15));*/
}
