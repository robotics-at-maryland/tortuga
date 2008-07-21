/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestControlFunctions.cxx
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
#include "control/include/BWPDController.h"
//#include "control/test/include/ControllerTests.h"

#include "core/test/include/BufferedAppender.h"

using namespace ram;

struct Fixture
{
    Fixture() : vehicle(new MockVehicle()),
                controller(ram::vehicle::IVehiclePtr(vehicle),
                           core::ConfigNode::fromString(
                               "{ 'name' : 'TestController',"
                               "'angularPGain' : 10,"
                               "'angularDGain' : 1,"
                               "'desiredQuaternion' : [0, 0, 0, 1] }"))
    {}

    MockVehicle* vehicle;
    control::BWPDController controller;
};

TEST_FIXTURE(Fixture, YawControl)
{
    // First is north along horizontal (desired)
    // Rotated 32 degrees from north to the west in horizontal (actual)
    vehicle->orientation = math::Quaternion(0, 0, 0.2756, 0.9613);
    
    math::Vector3 exp_rotTorque(0, 0, -3.5497);
    controller.update(1);
    CHECK_CLOSE(exp_rotTorque, vehicle->torque, 0.0001);
}

TEST_FIXTURE(Fixture, PitchControl)
{
    math::Vector3 exp_rotTorque(0, -2.7872, 0);
    vehicle->orientation = math::Quaternion(0, 0.2164, 0, 0.9763);

    controller.update(1);
    CHECK_CLOSE(exp_rotTorque, vehicle->torque, 0.0001);
}

TEST_FIXTURE(Fixture, RollControl)
{
    math::Vector3 exp_rotTorque(0.7692, 0, 0);
    vehicle->orientation = math::Quaternion(-0.3827, 0, 0, 0.9239);

    controller.update(1);
    CHECK_CLOSE(exp_rotTorque, vehicle->torque, 0.0001);
}

TEST_FIXTURE(Fixture, yawVehicle)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Z);
    controller.yawVehicle(30);
    CHECK_CLOSE(expected, controller.getDesiredOrientation(), 0.0001);
}

TEST_FIXTURE(Fixture, pitchVehicle)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_Y);
    controller.pitchVehicle(30);
    CHECK_CLOSE(expected, controller.getDesiredOrientation(), 0.0001);
}

TEST_FIXTURE(Fixture, rollVehicle)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
    controller.rollVehicle(30);
    CHECK_CLOSE(expected, controller.getDesiredOrientation(), 0.0001);
}

TEST_FIXTURE(Fixture, setDesiredOrientation)
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
    controller.setDesiredOrientation(expected);
    math::Quaternion actual(controller.getDesiredOrientation());
    CHECK_EQUAL(expected, actual);
}

TEST_FIXTURE(Fixture, DepthControl)
{
    math::Vector3 exp_tranForce(0, 0, 0);

    // Test at correct depth response
    controller.setDepth(0);
    vehicle->depth = 0;
    controller.update(1);
    CHECK_CLOSE(exp_tranForce, vehicle->force, 0.0001);

    controller.setDepth(3);
    vehicle->depth = 3;    
    controller.update(1);
    CHECK_CLOSE(exp_tranForce, vehicle->force, 0.0001);

    // Test we need to dive response
    exp_tranForce = math::Vector3(0,0,-5);
    
    controller.setDepth(5);
    vehicle->depth = 0;
    controller.update(1);
    CHECK_CLOSE(exp_tranForce, vehicle->force, 0.0001);
}

TEST_FIXTURE(Fixture, atDepth)
{
    // This assumes the default threshold for depth is 0.5
    vehicle->depth = 4;
    controller.update(1);
    
    controller.setDepth(5);
    CHECK_EQUAL(false, controller.atDepth());

    controller.setDepth(3);
    CHECK_EQUAL(false, controller.atDepth());

    controller.setDepth(4.3);
    CHECK_EQUAL(true, controller.atDepth());

    controller.setDepth(3.7);
    CHECK_EQUAL(true, controller.atDepth());
    
    controller.setDepth(4);
    CHECK(controller.atDepth());
}

void depthHelper(double* result, ram::core::EventPtr event)
{
    ram::math::NumericEventPtr nevent =
        boost::dynamic_pointer_cast<ram::math::NumericEvent>(event);
    *result = nevent->number;
}

TEST_FIXTURE(Fixture, Event_DESIRED_DEPTH_UPDATE)
{
    double actualDesiredDepth = 0;

    // Subscribe to the event
    controller.subscribe(ram::control::IController::DESIRED_DEPTH_UPDATE,
                         boost::bind(depthHelper, &actualDesiredDepth, _1));
    
    // Test at correct depth response
    controller.setDepth(5);

    CHECK_EQUAL(5, actualDesiredDepth);
}

TEST_FIXTURE(Fixture, Event_AT_DEPTH)
{
    double actualDepth = 0;

    // Subscribe to the event
    controller.subscribe(ram::control::IController::AT_DEPTH,
                         boost::bind(depthHelper, &actualDepth, _1));

    // Default Depth Threshold is 0.5
    vehicle->depth = 4;
    controller.update(1);
    controller.setDepth(3.7);
    CHECK_EQUAL(true, controller.atDepth());

    // Make sure we get event if we are in range we set it
    CHECK_EQUAL(4, actualDepth);
    actualDepth = 0;
    
    // Make sure the event doesn't go off when we are at and loose it depth
    controller.setDepth(5);
    controller.update(1);
    CHECK_EQUAL(0, actualDepth);

    // Ensure it does go off
    controller.setDepth(4.3);
    controller.update(1);
    CHECK_EQUAL(4, actualDepth);

    // Make sure it doesn't go off again until we have left the depth range
    actualDepth = 0;
    controller.update(1);
    CHECK_EQUAL(0, actualDepth);

    // Make sure once we leave, and come back it will go off again
    vehicle->depth = 6;
    controller.update(1);
    CHECK_EQUAL(0, actualDepth);

    vehicle->depth = 4;
    controller.setDepth(3.7);
    controller.update(1);
    CHECK_EQUAL(4, actualDepth);
}


void orientationHelper(math::Quaternion* result, ram::core::EventPtr event)
{
    math::OrientationEventPtr oevent =
        boost::dynamic_pointer_cast<ram::math::OrientationEvent>(event);
    *result = oevent->orientation;
}

TEST_FIXTURE(Fixture, Event_DESIRED_ORIENTATION_UPDATE)
{
    math::Quaternion actualDesiredOrientation = math::Quaternion::IDENTITY;

    // Subscribe to the event
    controller.subscribe(ram::control::IController::DESIRED_ORIENTATION_UPDATE,
                         boost::bind(orientationHelper,
                                     &actualDesiredOrientation, _1));
    
    // Change vehicle orientation by yawing
    controller.yawVehicle(15);

    math::Quaternion expectedOrientation;
    expectedOrientation.FromAngleAxis(math::Degree(15), math::Vector3::UNIT_Z);
    CHECK_EQUAL(expectedOrientation, actualDesiredOrientation);


    // Full reset
    expectedOrientation.FromAngleAxis(math::Degree(30), math::Vector3::UNIT_X);
    controller.setDesiredOrientation(expectedOrientation);
    CHECK_EQUAL(expectedOrientation, actualDesiredOrientation);
}

TEST_FIXTURE(Fixture, atOrientation)
{
    // Yawed 15 degrees left
    math::Quaternion orientation(math::Degree(15), math::Vector3::UNIT_Z);
    vehicle->orientation = orientation;
    controller.update(1);

    // 15 degrees left of desired
    controller.yawVehicle(30);
    CHECK_EQUAL(false, controller.atOrientation());

    // 15 degrees right of desired
    controller.yawVehicle(-30);
    CHECK_EQUAL(false, controller.atOrientation());

    // 2.5 degrees right of desired
    controller.yawVehicle(12.5);
    CHECK_EQUAL(true, controller.atOrientation());

    // 2.5 degrees left of desired
    controller.yawVehicle(5);
    CHECK_EQUAL(true, controller.atOrientation());

    // Desired = Actual
    controller.yawVehicle(-2.5);
    CHECK_EQUAL(true, controller.atOrientation());
}

TEST_FIXTURE(Fixture, Event_AT_ORIENTATION)
{
    math::Quaternion actualOrientation = math::Quaternion::IDENTITY;

    // Subscribe to the event
    controller.subscribe(ram::control::IController::AT_ORIENTATION,
                         boost::bind(orientationHelper,
                                     &actualOrientation, _1));
    
    // Default Orientation threshold is about 2.5 degrees
    math::Quaternion orientation(math::Degree(15), math::Vector3::UNIT_Z);
    vehicle->orientation = orientation;
    controller.update(1);
    // 1 Degree difference
    controller.yawVehicle(16);
    CHECK_EQUAL(true, controller.atOrientation());

    // Make sure when we set a value in range we still get the event
    CHECK_EQUAL(orientation, actualOrientation);
    actualOrientation = math::Quaternion::IDENTITY;
    
    // Make sure the event doesn't go off when we are at orientation and then
    // move out
    // 4 degrees left of desired
    controller.yawVehicle(3);
    controller.update(1);
    CHECK_EQUAL(math::Quaternion::IDENTITY, actualOrientation);

    // Ensure it does go off when we move back
    // 1.5 degrees left of desired
    controller.yawVehicle(-2.5);
    controller.update(1);
    CHECK_EQUAL(orientation, actualOrientation);

    // Make sure it doesn't go off again until we have left the depth range
    actualOrientation = math::Quaternion::IDENTITY;
    controller.update(1);
    CHECK_EQUAL(math::Quaternion::IDENTITY, actualOrientation);

    // Make sure once we leave, and come back it will go off again
    vehicle->orientation = math::Quaternion(math::Degree(30),
                                            math::Vector3::UNIT_Z);;
    controller.update(1);
    CHECK_EQUAL(math::Quaternion::IDENTITY, actualOrientation);

    vehicle->orientation = math::Quaternion(math::Degree(15),
                                            math::Vector3::UNIT_Z);
    controller.update(1);
    CHECK_EQUAL(orientation, actualOrientation);
}


TEST(BWPDControllerLogging)
{
    // Create in memory appender
    BufferedAppender* appender = new BufferedAppender("Test");
    log4cpp::Category::getInstance("Controller").setAppender(appender);

    // Create Controller object
    Fixture fixture;

    // Make sure the header is present
    CHECK_EQUAL(1u, appender->logEvents.size());
	//    CHECK_EQUAL("% Time M-Quat M-Depth D-Quat D-Depth D-Speed RotTorq"
	//                " TranForce", appender->logEvents[0].message);

    // Just do an update and make sure we have a message
    fixture.controller.update(1);
    CHECK_EQUAL(2u, appender->logEvents.size());
    CHECK(std::string("") !=  appender->logEvents[1].message);
}
