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


TEST_UTILITY_FWD(setGetDepth, (ram::control::IController* controller));

TEST_UTILITY_FWD(yawVehicle, (ram::control::IController* controller));
TEST_UTILITY_FWD(pitchVehicle, (ram::control::IController* controller));
TEST_UTILITY_FWD(rollVehicle, (ram::control::IController* controller));
TEST_UTILITY_FWD(setGetDesiredOrientation, (ram::control::IController* controller));

TEST_UTILITY_FWD(setGetSpeed, (ram::control::IController* controller));
TEST_UTILITY_FWD(setGetSidewaysSpeed, (ram::control::IController* controller));
TEST_UTILITY_FWD(setGetDesiredVelocity, (ram::control::IController* controller));
TEST_UTILITY_FWD(setGetDesiredPosition, (ram::control::IController* controller));





// /** Tests whether the IDepthController::atDepth function is working
//  *
//  *  Note: this assumes the threshold is 0.5.
//  */

// TEST_UTILITY_FWD(atDepth,
//                  (ram::control::IDepthController* controller,
//                   boost::function<void(double)> setDepth,
//                   boost::function<void (void)> update));

// /** Tests whether hte IDepthController::holdCurrentDepth function is working*/
// TEST_UTILITY_FWD(holdCurrentDepth,
//                  (ram::control::IDepthController* controller,
//                   boost::function<void(double)> setDepth,
//                   boost::function<void (void)> update));


// TEST_UTILITY_FWD(holdCurrentHeading,
//                  (ram::control::IController* controller,
//                   boost::function<void(ram::math::Quaternion)> setOrientation));

// TEST_UTILITY_FWD(atOrientation,
//                  (ram::control::IController* controller,
//                   boost::function<void(ram::math::Quaternion)> setOrientation,
//                   boost::function<void (void)> update));

#endif // RAM_CONTROL_TEST_ICONTROLLERIMPTESTS_H
