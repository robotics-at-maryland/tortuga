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
  control::DesiredState desired;
  control::MeasuredState measured; 
  control::ControllerState state;
  control::EstimatedState estimated;

   //Testing PID controller
    desired.depth = 2.2;
    measured.depth = 0;
    //state.dt = 0.025;
    state.dtMin = .001;
    state.dtMax = .05;    
    state.depthKp = 40;
    state.depthKd = 30;
    state.depthKi = .5;
    state.depthSumError = 0;
    state.depthPrevX = 0;

    double result = control::depthPIDController(&measured,
                          &desired,
                          &state,
                          &estimated,0.025);
    CHECK_CLOSE(88.0275, result, .001);

}

TEST(PIDController2)
{
    // First is north along horizontal (desired)
    // Rotated 32 degrees from north to the west in horizontal (actual)
    
    // Quat = 0,0,0,1 and everything else zero
  control::DesiredState desired;
  control::MeasuredState measured;
  control::ControllerState state;
  control::EstimatedState estimated;
    estimated.xHat4Depth = math::Vector4(0,0,0,0);

	 
   //Testing PID controller
    desired.depth = 3.1;
    measured.depth = 3.0;
    state.dtMin = .001;
    state.dtMax = .05;    
    state.depthKp = 40;
    state.depthKd = 30;
    state.depthKi = .5;
    state.depthSumError = -46;
    state.depthPrevX = 3.0;

    double result = control::depthPIDController(&measured,
                          &desired,
                          &state,
                          &estimated,0.025);
    CHECK_CLOSE(27.0013, result, .001);

}
