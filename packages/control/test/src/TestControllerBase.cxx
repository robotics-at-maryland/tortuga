/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestControllerBase.cxx
 */

// Library Includes
#include <iostream>
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "math/test/include/MathChecks.h"
#include "math/include/Events.h"
#include "vehicle/test/include/MockVehicle.h"
#include "control/include/ControllerBase.h"
#include "control/test/include/MockControllerBaseImp.h"

using namespace ram;

struct ControlBaseFixture
{
    ControlBaseFixture() : vehicle(new MockVehicle()),
                mockController(ram::vehicle::IVehiclePtr(vehicle),
                           core::ConfigNode::fromString(
                               "{ 'name' : 'TestController'}"))
    {}

    MockVehicle* vehicle;
    MockControllerBaseImp mockController;
};

SUITE(ControllerBase) {

TEST_FIXTURE(ControlBaseFixture, yawVehicleHelper)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Z);
    math::Quaternion result(control::ControllerBase::yawVehicleHelper(
                                math::Quaternion::IDENTITY, 30));
    CHECK_CLOSE(expected, result, 0.0001);
}

TEST_FIXTURE(ControlBaseFixture, pitchVehicleHelper)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Y);
    math::Quaternion result(control::ControllerBase::pitchVehicleHelper(
                                math::Quaternion::IDENTITY, 30));
    CHECK_CLOSE(expected, result, 0.0001);
}

TEST_FIXTURE(ControlBaseFixture, rollVehicleHelper)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
    math::Quaternion result(control::ControllerBase::rollVehicleHelper(
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
        control::ControllerBase::holdCurrentHeadingHelper(orientation);

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
        control::ControllerBase::holdCurrentHeadingHelper(orientation2);
    
    // expected output
    math::Quaternion expectedOrientation2(0, 0, 0.7071, 0.7071);

    CHECK_CLOSE(expectedOrientation2, result2, 0.001);
}

TEST_FIXTURE(ControlBaseFixture, update)
{
    // Values to passed off to the vehicle
    mockController.translationalForceOut = math::Vector3(3, 7, 1);
    mockController.rotationalTorqueOut = math::Vector3(11, 2, 5);

    // Values to be passed to controller
    double timestep = 0.892;
    vehicle->linearAcceleration = math::Vector3(1,2,3);
    vehicle->orientation = math::Quaternion(2.3, 4.12, 1.23, 1);
    vehicle->angularRate = math::Vector3(0.123, 6.56, 3.123);
    vehicle->depth = 2.123;

    // Run the update
    mockController.update(timestep);

    // Check the values are passed to subclass properly
    CHECK_EQUAL(timestep, mockController.timestep);
    CHECK_EQUAL(vehicle->linearAcceleration, mockController.linearAcceleration);
    CHECK_EQUAL(vehicle->orientation, mockController.orientation);
    CHECK_EQUAL(vehicle->angularRate, mockController.angularRate);
    CHECK_EQUAL(vehicle->depth, mockController.depth);

    // Check that the vehicle gets passed the proper values
    CHECK_EQUAL(mockController.translationalForceOut, vehicle->force);
    CHECK_EQUAL(mockController.rotationalTorqueOut, vehicle->torque);
}

void depthHelper(double* result, ram::core::EventPtr event)
{
    ram::math::NumericEventPtr nevent =
        boost::dynamic_pointer_cast<ram::math::NumericEvent>(event);
    *result = nevent->number;
}

TEST_FIXTURE(ControlBaseFixture, newDepthSet)
{
    double actualDesiredDepth = 0;
    double actualDepth = 0;
    
    // Subscribe to the events
    mockController.subscribe(ram::control::IController::DESIRED_DEPTH_UPDATE,
                             boost::bind(depthHelper, &actualDesiredDepth, _1));
    mockController.subscribe(ram::control::IController::AT_DEPTH,
                             boost::bind(depthHelper, &actualDepth, _1));

    // Set atDepth and the current value
    mockController.atDepthValue = false;
    mockController.desiredDepth = 0;
    
    // Test desired depth update
    mockController._newDepthSet(5.6);
    CHECK_EQUAL(5.6, actualDesiredDepth);
    CHECK_EQUAL(0, actualDepth);

    // Test at depth update
    mockController.atDepthValue = true;
    mockController._newDepthSet(2.8);
    CHECK_EQUAL(2.8, actualDesiredDepth);
    CHECK_EQUAL(2.8, actualDepth);
}

} // SUITE(ControllerBase)
