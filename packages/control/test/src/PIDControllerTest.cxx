/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee
 * All rights reserved.
 *
 * Authory: ajanas
 * File:  packages/control/test/src/TestPIDController.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/include/ControlFunctions.h"

using namespace ram;

TEST(PIDController)
{
    // First is north along horizontal (desired)
    // Rotated 32 degrees from north to the west in horizontal (actual)
    
    // Quat = 0,0,0,1 and everything else zero
    control::DesiredState desired = {0, // speed
                                     0, // sidewaysSpeed
                                     0, // depth
                                     {0, 0, 0, 1}, // quat
                                     {0} // angularRate
    };

    control::MeasuredState measured = {0,// depth
                              {0},  // Accel
                              {0}, // Mag
                              {0, 0, 0, 1}, // Quat
                              {0}}; // Ang Rate
    
    control::ControllerState state = {0, // angP gain
                                0, // angD gain
                                {{0.201, 0, 0}, // inertia estimates
                                 {0, 1.288, 0},
                                 {0, 0, 1.288}},
                                0, // depthP gain (not used)
                                0};// speedP gain (not used)
    control::EstimatedState estimated = {math::Vector2(0,0)};

   //Testing PID controller
    desired.depth = 4;
    measured.depth = 3;
    //state.dt = 0.025;    
    state.depthKp = 40;
    state.depthKd = 30;
    state.depthKi = .5;
    state.depthSumError = -46;
    state.depthPrevX = 15;

    double result = control::depthPIDController(&measured,
                          &desired,
                          &state,
                          &estimated,0.025);
    CHECK_CLOSE(14463, result, 1);

}

TEST(PIDController2)
{
    // First is north along horizontal (desired)
    // Rotated 32 degrees from north to the west in horizontal (actual)
    
    // Quat = 0,0,0,1 and everything else zero
    control::DesiredState desired = {0, // speed
                                     0, // sidewaysSpeed
                                     0, // depth
                                     {0, 0, 0, 1}, // quat
                                     {0} // angularRate
    };

    control::MeasuredState measured = {0,// depth
                              {0},  // Accel
                              {0}, // Mag
                              {0, 0, 0, 1}, // Quat
                              {0}}; // Ang Rate
    
    control::ControllerState state = {0, // angP gain
                                0, // angD gain
                                {{0.201, 0, 0}, // inertia estimates
                                 {0, 1.288, 0},
                                 {0, 0, 1.288}},
                                0, // depthP gain (not used)
                                0};// speedP gain (not used)
    control::EstimatedState estimated = {math::Vector2(0,0)};
    estimated.xHat4Depth = math::Vector4(0,0,0,0);


   //Testing PID controller
    desired.depth = 3.1;
    measured.depth = 3;
    //state.dt = .0250;    
    state.depthKp = 40;
    state.depthKd = 30;
    state.depthKi = .5;
    state.depthSumError = -46;
    state.depthPrevX = 15;

    double result = control::depthPIDController(&measured,
                          &desired,
                          &state,
                          &estimated,0.025);
    CHECK_CLOSE(14427, result, 1);

}
