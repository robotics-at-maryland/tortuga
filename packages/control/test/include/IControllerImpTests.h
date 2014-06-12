/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/test/include/IControllerImpTests.h
 */

#ifndef RAM_CONTROL_TEST_ICONTROLLERIMPTESTS_H
#define RAM_CONTROL_TEST_ICONTROLLERIMPTESTS_H

// Library Includes
#include <boost/function.hpp>

// Project Includes
#include "control/include/IController.h"
#include "core/test/include/Macros.h"

/* Helps with testing functions required to be an implementation of IController */

TEST_UTILITY_FWD(changeDepth, (ram::control::IController* controller));
TEST_UTILITY_FWD(rotate, (ram::control::IController* controller));
TEST_UTILITY_FWD(translate, (ram::control::IController* controller));

TEST_UTILITY_FWD(yawVehicle, (ram::control::IController* controller));
TEST_UTILITY_FWD(pitchVehicle, (ram::control::IController* controller));
TEST_UTILITY_FWD(rollVehicle, (ram::control::IController* controller));

#endif // RAM_CONTROL_TEST_ICONTROLLERIMPTESTS_H
