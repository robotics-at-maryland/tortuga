/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/test/src/IControllerImpTests.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/test/include/IControllerImpTests.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

using namespace ram;


TEST_UTILITY_IMP(setGetDepth, (ram::control::IController* controller))
{
    double depth = 1.67;
    controller->setDepth(depth);
    CHECK_EQUAL(depth, controller->getDepth());

    depth = 6.7;
    controller->setDepth(depth);
    CHECK_EQUAL(depth, controller->getDepth());
}

TEST_UTILITY_IMP(yawVehicle, (ram::control::IController* controller))
{
    double orig_yaw = controller->getDesiredOrientation().getYaw().valueDegrees();
    double degrees = 78.2;
    controller->yawVehicle(degrees);
    double new_yaw = controller->getDesiredOrientation().getYaw().valueDegrees();
    CHECK_CLOSE(degrees, (new_yaw - orig_yaw), 0.0001);

    degrees = -degrees;
    controller->yawVehicle(degrees);
    new_yaw = controller->getDesiredOrientation().getYaw().valueDegrees();
    CHECK_CLOSE(0, (new_yaw - orig_yaw), 0.0001);

//     math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Z);
//     controller->yawVehicle(30);
//     CHECK_CLOSE(expected, controller->getDesiredOrientation(), 0.0001);
}

TEST_UTILITY_IMP(pitchVehicle, (ram::control::IController* controller))
{
    double curr_pitch = controller->getDesiredOrientation().getPitch().valueDegrees();
    double degrees = 45.2;
    controller->pitchVehicle(degrees);
    double new_pitch = controller->getDesiredOrientation().getPitch().valueDegrees();
    CHECK_CLOSE(degrees, (new_pitch - curr_pitch), 0.0001);

//     math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Y);
//     controller->pitchVehicle(30);
//     CHECK_CLOSE(expected, controller->getDesiredOrientation(), 0.0001);
}

TEST_UTILITY_IMP(rollVehicle, (ram::control::IController* controller))
{
    double curr_roll = controller->getDesiredOrientation().getRoll().valueDegrees();
    double degrees = 45.2;
    controller->rollVehicle(degrees);
    double new_roll = controller->getDesiredOrientation().getRoll().valueDegrees();
    CHECK_CLOSE(degrees, (new_roll - curr_roll), 0.0001);

//     math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
//     controller->rollVehicle(30);
//     CHECK_CLOSE(expected, controller->getDesiredOrientation(), 0.0001);
}

TEST_UTILITY_IMP(setGetDesiredOrientation, (ram::control::IController* controller))
{
    math::Quaternion orientation(0.2, 1.2, 2.5, 1);
    controller->setDesiredOrientation(orientation);
    orientation.normalise();
    CHECK_EQUAL(orientation, controller->getDesiredOrientation());


//     math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
//     controller->setDesiredOrientation(expected);
//     math::Quaternion actual(controller->getDesiredOrientation());
//     CHECK_EQUAL(expected, actual);
}

TEST_UTILITY_IMP(setGetVelocity, (ram::control::IController* controller))
{
    math::Vector2 velocity(3.5,-2.7);
    controller->setVelocity(velocity);
    CHECK_EQUAL(velocity, controller->getVelocity());

//     ram::math::Vector2 velocity(1.5, 2.9);
//     controller->setVelocity(velocity);
//     CHECK_EQUAL(velocity, controller->getVelocity());

}

TEST_UTILITY_IMP(setGetSpeed, (ram::control::IController* controller))
{
    double speed = 5.8;
    controller->setSpeed(speed);
    CHECK_EQUAL(5, controller->getSpeed());

//     double speed = 1.5;
//     controller->setSpeed(speed);
//     CHECK_EQUAL(speed, controller->getSpeed());

}

TEST_UTILITY_IMP(setGetSidewaysSpeed, (ram::control::IController* controller))
{
    double sidewaysSpeed = 2.8;
    controller->setSidewaysSpeed(sidewaysSpeed);
    CHECK_EQUAL(sidewaysSpeed, controller->getSidewaysSpeed());

//     double sidewaysSpeed = 1.5;
//     controller->setSidewaysSpeed(sidewaysSpeed);
//     CHECK_EQUAL(sidewaysSpeed, controller->getSidewaysSpeed());

}

TEST_UTILITY_IMP(setGetDesiredVelocity, (ram::control::IController* controller))
{
/* Set Inertial, Get Inertial */
    math::Vector2 velocity_n_n(2.4,-4.3);
    controller->setDesiredVelocity(velocity_n_n, control::IController::INERTIAL_FRAME);
    CHECK_EQUAL(velocity_n_n, controller->getDesiredVelocity(control::IController::INERTIAL_FRAME));

/* The Following rely on the current orientation, so the vehicle's current orientation
   must be set to a known value and correct results calculated with this value */

/* Set Inertial, Get Body */

/* Set Body, Get Body */

/* Set Body, Get Inertial */
}

TEST_UTILITY_IMP(setGetDesiredPosition, (ram::control::IController* controller))
{
/* Set Inertial, Get Inertial */
    math::Vector2 position_n_n(2.4,-4.3);
    controller->setDesiredPosition(position_n_n, control::IController::INERTIAL_FRAME);
    CHECK_EQUAL(position_n_n, controller->getDesiredPosition(control::IController::INERTIAL_FRAME));

/* The Following rely on the current orientation, so the vehicle's current orientation
   must be set to a known value and correct results calculated with this value */

/* Set Inertial, Get Body */

/* Set Body, Get Body */

/* Set Body, Get Inertial */
}



// TEST_UTILITY_IMP(atDepth,
//                  (control::IDepthController* controller,
//                   boost::function<void(double)> setDepth,
//                   boost::function<void (void)> update))
// {
//     // This assumes the default threshold for depth is 0.5
//     setDepth(4);
//     update();
    
//     controller->setDepth(5);
//     CHECK_EQUAL(false, controller->atDepth());

//     controller->setDepth(3);
//     CHECK_EQUAL(false, controller->atDepth());

//     controller->setDepth(4.3);
//     CHECK_EQUAL(true, controller->atDepth());

//     controller->setDepth(3.7);
//     CHECK_EQUAL(true, controller->atDepth());
    
//     controller->setDepth(4);
//     CHECK(controller->atDepth());
// }

// TEST_UTILITY_IMP(holdCurrentDepth,
//                  (ram::control::IDepthController* controller,
//                   boost::function<void(double)> setDepth,
//                   boost::function<void (void)> update))
// {
//     // Lock in current depth
//     setDepth(4);
//     update();

//     // Check Normall setting
//     controller->setDepth(5);
//     CHECK_EQUAL(5, controller->getDepth());

//     // Hold and make sure we have changed
//     controller->holdCurrentDepth();
//     CHECK_EQUAL(4, controller->getDepth());
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
