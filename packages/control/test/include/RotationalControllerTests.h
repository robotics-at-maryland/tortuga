/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/include/RotationalControllerTests.h
 */

#ifndef RAM_CONTROL_TEST_ROTATIONALCONTROLLERTESTS_03_01_2009
#define RAM_CONTROL_TEST_ROTATIONALCONTROLLERTESTS_03_01_2009

// Library Includes
#include <boost/function.hpp>

// Project Includes
#include "control/include/IRotationalController.h"
#include "core/test/include/Macros.h"

TEST_UTILITY_FWD(yawVehicle,
                 (ram::control::IRotationalController* controller));

TEST_UTILITY_FWD(pitchVehicle,
                 (ram::control::IRotationalController* controller));

TEST_UTILITY_FWD(rollVehicle,
                 (ram::control::IRotationalController* controller));

TEST_UTILITY_FWD(setDesiredOrientation,
                 (ram::control::IRotationalController* controller));

TEST_UTILITY_FWD(holdCurrentHeading,
                 (ram::control::IRotationalController* controller,
                  boost::function<void(ram::math::Quaternion)> setOrientation));

TEST_UTILITY_FWD(atOrientation,
                 (ram::control::IRotationalController* controller,
                  boost::function<void(ram::math::Quaternion)> setOrientation,
                  boost::function<void (void)> update));

#endif // RAM_CONTROL_TEST_ROTATIONALCONTROLLERTESTS_03_01_2009
