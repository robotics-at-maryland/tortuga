/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/test/include/DesiredStateTests.h
 */


/* Make test utilities for all functions in control/include/DesiredState.h */

#ifndef RAM_CONTROL_TEST_DESIREDSTATETESTS_H
#define RAM_CONTROL_TEST_DESIREDSTATETESTS_H

// Project Includes
#include "control/include/DesiredState.h"
#include "core/test/include/Macros.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

TEST_UTILITY_FWD(setGetDesiredDepth, 
                 (ram::controltest::DesiredStatePtr desiredState));

TEST_UTILITY_FWD(setGetDesiredOrientation, 
                 (ram::controltest::DesiredStatePtr desiredState));

TEST_UTILITY_FWD(setGetDesiredAngularRate, 
                 (ram::controltest::DesiredStatePtr desiredState));

TEST_UTILITY_FWD(setGetDesiredPosition,
                 (ram::controltest::DesiredStatePtr desiredState));

TEST_UTILITY_FWD(setGetDesiredVelocity,
                 (ram::controltest::DesiredStatePtr desiredState));

#endif // RAM_CONTROL_TEST_DESIREDSTATETESTS_H
