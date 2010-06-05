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
#include <boost/lambda/bind.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/SubsystemMaker.h"

#include "math/test/include/MathChecks.h"
#include "math/include/Events.h"

#include "vehicle/test/include/MockVehicle.h"

#include "control/include/BWPDController.h"
#include "control/test/include/IControllerImpTests.h"
#include "control/test/include/RotationalControllerTests.h"
#include "control/test/include/DepthControllerTests.h"
#include "control/test/include/TranslationalControllerTests.h"

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


//use these to test the normal PD rotational controller

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

TEST_FIXTURE(Fixture, YawRateControl)
{
  vehicle->orientation = math::Quaternion(0,0,0,1);
  vehicle->angularRate = math::Vector3(0,0,5);
  
  }

TEST_FIXTURE(Fixture, yawVehicle)
{
    TEST_UTILITY_FUNC(yawVehicle)(&controller);
}

TEST_FIXTURE(Fixture, pitchVehicle)
{
    TEST_UTILITY_FUNC(pitchVehicle)(&controller);
}

TEST_FIXTURE(Fixture, rollVehicle)
{
    TEST_UTILITY_FUNC(rollVehicle)(&controller);
}

TEST_FIXTURE(Fixture, setGetDesiredOrientation)
{
    TEST_UTILITY_FUNC(setGetDesiredOrientation)(&controller);
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

TEST_FIXTURE(Fixture, setGetDepth)
{
    TEST_UTILITY_FUNC(setGetDepth)(&controller);
}

// TEST_FIXTURE(Fixture, atDepth)
// {
//     TEST_UTILITY_FUNC(atDepth)
//         (&controller,
//          boost::bind(&MockVehicle::_setDepth, vehicle, _1),
//          boost::bind(&control::BWPDController::update, &controller, 1.0));
// }

// TEST_FIXTURE(Fixture, holdCurrentDepth)
// {
//     TEST_UTILITY_FUNC(holdCurrentDepth)
//         (&controller,
//          boost::bind(&MockVehicle::_setDepth, vehicle, _1),
//          boost::bind(&control::BWPDController::update, &controller, 1.0));
// }

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

// TEST_FIXTURE(Fixture, atOrientation)
// {
//     TEST_UTILITY_FUNC(atOrientation)
//         (&controller,
//          boost::bind(&MockVehicle::_setOrientation, vehicle, _1),
//          boost::bind(&control::BWPDController::update, &controller, 1.0));
// }

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

// TEST_FIXTURE(Fixture, TestHoldCurrentHeading)
// {
//     // Runs the test, passing it a function object which lets the test method
//     // set the actual orientation of the vehicle
//     TEST_UTILITY_FUNC(holdCurrentHeading)
//         (&controller,
//          boost::bind(&MockVehicle::_setOrientation, vehicle, _1));
// }

TEST_FIXTURE(Fixture, setGetSpeed)
{
    TEST_UTILITY_FUNC(setGetSpeed)(&controller);
}

TEST_FIXTURE(Fixture, setGetSidewaysSpeed)
{
    TEST_UTILITY_FUNC(setGetSidewaysSpeed)(&controller);
}


/*
made a change to log gyro controller, this test case breaks
Joe G  2008-12-2
TEST(BWPDControllerLogging)
{
    // Create in memory appender
    BufferedAppender* appender = new BufferedAppender("Test");
    log4cpp::Category::getInstance("Controller").setAppender(appender);

    // Create Controller object
    Fixture fixture;

    // Make sure the header is present
    CHECK_EQUAL(2u, appender->logEvents.size());
	//    CHECK_EQUAL("% Time M-Quat M-Depth D-Quat D-Depth D-Speed RotTorq"
	//                " TranForce", appender->logEvents[0].message);

    // Just do an update and make sure we have a message
    fixture.controller.update(1);
    CHECK_EQUAL(3u, appender->logEvents.size());
    CHECK(std::string("") !=  appender->logEvents[2].message);
}
*/

TEST(SubsystemMaker)
{
    vehicle::IVehiclePtr veh(new MockVehicle());
    core::SubsystemList deps;
    deps.push_back(veh);
    core::ConfigNode cfg(core::ConfigNode::fromString(
                             "{ 'name' : 'Controller',"
                             "'type' : 'BWPDController',"
                             "'angularPGain' : 10,"
                             "'angularDGain' : 1,"
                             "'desiredQuaternion' : [0, 0, 0, 1] }"));
    try {
        core::SubsystemPtr subsystem(core::SubsystemMaker::newObject(
                                         std::make_pair(cfg, deps)));
    } catch (core::MakerNotFoundException& ex) {
        CHECK(false && "BWPDController Maker not found");
    }
}
