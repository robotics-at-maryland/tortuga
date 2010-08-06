/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File: packages/control/test/src/TestHelpers.cxx 
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "control/include/Helpers.h"

using namespace ram;

SUITE(CONTROL_HELPERS) {


/* TODO
   Add Tests for nRb and bRn which are located in control/include/Helpers.h
 */

TEST_FIXTURE(yawVehicleHelper)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Z);
    math::Quaternion result(control::yawVehicleHelper(
                                math::Quaternion::IDENTITY, 30));
    CHECK_CLOSE(expected, result, 0.0001);
}

TEST_FIXTURE(ControlBaseFixture, pitchVehicleHelper)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Y);
    math::Quaternion result(control::pitchVehicleHelper(
                                math::Quaternion::IDENTITY, 30));
    CHECK_CLOSE(expected, result, 0.0001);
}

TEST_FIXTURE(ControlBaseFixture, rollVehicleHelper)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
    math::Quaternion result(control::rollVehicleHelper(
                                math::Quaternion::IDENTITY, 30));
    CHECK_CLOSE(expected, result, 0.0001);
}

TEST_FIXTURE(ControlBaseFixture, holdCurrentHeadingHelper)
{
    // Test 1

    // Set the current "measured" orientation
    math::Quaternion orientation(math::Degree(15), math::Vector3::UNIT_Z);
    vehicle->orientation = orientation;

    // Tell the controller to hold current heading (ignoring roll and pitch)
    math::Quaternion result =
        control::holdCurrentHeadingHelper(orientation);

    // Create the expected orientation and make sure the desired orientation
    // was set properly based on the vehicle current orientation
    math::Quaternion expectedOrientation(math::Degree(15), 
					 math::Vector3::UNIT_Z);
    CHECK_EQUAL(expectedOrientation, result);

    // Test 2
    
    // set the current "measured" orientation
    math::Quaternion orientation2(0.0028, 0.0028, 0.7071, 0.7071);
    vehicle->orientation = orientation2;

    // tell the controller to hold current heading
    math::Quaternion result2 =
        control::holdCurrentHeadingHelper(orientation2);
    
    // expected output
    math::Quaternion expectedOrientation2(0, 0, 0.7071, 0.7071);

    CHECK_CLOSE(expectedOrientation2, result2, 0.001);
}
    

} // SUILTE(CONTROL_HELPERS)

