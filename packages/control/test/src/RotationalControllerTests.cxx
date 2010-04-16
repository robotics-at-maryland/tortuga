/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/RotationalControllerTests.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/test/include/RotationalControllerTests.h"
#include "math/test/include/MathChecks.h"

using namespace ram;

// TEST_UTILITY_IMP(yawVehicle,
//                  (control::IRotationalController* controller))
// {
//     math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Z);
//     controller->yawVehicle(30);
//     CHECK_CLOSE(expected, controller->getDesiredOrientation(), 0.0001);
// }


// TEST_UTILITY_IMP(pitchVehicle,
//                  (control::IRotationalController* controller))
// {
//     math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Y);
//     controller->pitchVehicle(30);
//     CHECK_CLOSE(expected, controller->getDesiredOrientation(), 0.0001);
// }

// TEST_UTILITY_IMP(rollVehicle,
//                  (control::IRotationalController* controller))
// {
//     math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
//     controller->rollVehicle(30);
//     CHECK_CLOSE(expected, controller->getDesiredOrientation(), 0.0001);
// }

// TEST_UTILITY_IMP(setDesiredOrientation,
//                  (control::IRotationalController* controller))
// {
//     math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
//     controller->setDesiredOrientation(expected);
//     math::Quaternion actual(controller->getDesiredOrientation());
//     CHECK_EQUAL(expected, actual);
// }

// TEST_UTILITY_IMP(holdCurrentHeading,
//                  (ram::control::IRotationalController* controller,
//                   boost::function<void (ram::math::Quaternion)> setOrientation))
// {
//     // Test 1

//     // Set the current "measured" orientation
//     math::Quaternion orientation(math::Degree(15), math::Vector3::UNIT_Z);
//     setOrientation(orientation);

//     // Tell the controller to hold current heading (ignoring roll and pitch)
//     controller->holdCurrentHeading();

//     // Create the expected orientation and make sure the desired orientation
//     // was set properly based on the vehicle current orientation
//     math::Quaternion expectedOrientation(math::Degree(15), 
// 					 math::Vector3::UNIT_Z);

//     CHECK_EQUAL(expectedOrientation, controller->getDesiredOrientation());

//     // Test 2
    
//     // set the current "measured" orientation
//     math::Quaternion orientation2(0.0028, 0.0028, 0.7071, 0.7071);
//     setOrientation(orientation2);

//     // tell the controller to hold current heading
//     controller->holdCurrentHeading();
    
//     // expected output
//     math::Quaternion expectedOrientation2(0, 0, 0.7071, 0.7071);

//     CHECK_CLOSE(expectedOrientation2, controller->getDesiredOrientation(),
//                 0.001);
// }

// TEST_UTILITY_IMP(atOrientation,
//                  (ram::control::IRotationalController* controller,
//                   boost::function<void(ram::math::Quaternion)> setOrientation,
//                   boost::function<void (void)> update))
// {
//     // Yawed 15 degrees left
//     math::Quaternion orientation(math::Degree(15), math::Vector3::UNIT_Z);
//     setOrientation(orientation);
//     update();
    
//     // 15 degrees left of desired
//     controller->yawVehicle(30);
//     CHECK_EQUAL(false, controller->atOrientation());

//     // 15 degrees right of desired
//     controller->yawVehicle(-30);
//     CHECK_EQUAL(false, controller->atOrientation());

//     // 2.5 degrees right of desired
//     controller->yawVehicle(12.5);
//     CHECK_EQUAL(true, controller->atOrientation());

//     // 2.5 degrees left of desired
//     controller->yawVehicle(5);
//     CHECK_EQUAL(true, controller->atOrientation());

//     // Desired = Actual
//     controller->yawVehicle(-2.5);
//     CHECK_EQUAL(true, controller->atOrientation());

// }
