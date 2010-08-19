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
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/SubsystemMaker.h"
#include "core/include/EventHub.h"

#include "math/test/include/MathChecks.h"
#include "math/include/Events.h"

#include "vehicle/test/include/MockVehicle.h"

#include "control/include/Common.h"
#include "control/include/CombineController.h"
#include "control/test/include/IControllerImpTests.h"
#include "control/test/include/MockDepthController.h"
#include "control/test/include/MockTranslationalController.h"
#include "control/test/include/MockRotationalController.h"

#include "estimation/test/include/MockEstimator.h"

//#include "core/test/include/BufferedAppender.h"


/* TODO
   Add test cases for the new translational functions.  Assure that the old test cases
   are still valid. Decide what to do about atDepth, atPosition, etc... and 
   getEstimatedDepth and getEstimatedDepthDot.
 */


using namespace ram;

struct CombineControllerFixture
{
    CombineControllerFixture() :
        vehicle(new MockVehicle()),
        estimator(new MockEstimator()),
        eventHub(core::EventHubPtr(new core::EventHub("eventHub"))),
        transController(0),
        depthController(0),
        rotController(0),
        controller(eventHub,
                   vehicle::IVehiclePtr(vehicle),
                   estimation::IStateEstimatorPtr(estimator),
                   core::ConfigNode::fromString("{"
                       "'name' : 'TestController',"
                       "'TranslationalController' : {"
                       "    'type' : 'MockTranslationalController'"
                       "},"
                       "'DepthController' : {"
                       "    'type' : 'MockDepthController'"
                       "},"
                       "'RotationalController' : {"
                       "    'type' : 'MockRotationalController'"
                       "}}"))
    {
        transController = dynamic_cast<MockTranslationalController*>(
            controller.getTranslationalController().get());
        depthController = dynamic_cast<MockDepthController*>(
            controller.getDepthController().get());
        rotController = dynamic_cast<MockRotationalController*>(
            controller.getRotationalController().get());
    }

    MockVehicle* vehicle;
    MockEstimator* estimator;
    core::EventHubPtr eventHub;
    MockTranslationalController* transController;
    MockDepthController* depthController;
    MockRotationalController* rotController;
    control::CombineController controller;
    
};

SUITE(CombineController) {

// Tests creation of combine controller seperate from fixture
TEST(Create)
{
    CombineControllerFixture fixture;
    // Ensure that objects of the right type were created
    CHECK(fixture.transController);
    CHECK(fixture.depthController);
    CHECK(fixture.rotController);
}

TEST_FIXTURE(CombineControllerFixture, Create)
{
    // Ensure that objects of the right type were created
    CHECK(transController);
    CHECK(depthController);
    CHECK(rotController);
}

// Translational methods
TEST_FIXTURE(CombineControllerFixture, setGetSpeed)
{
    TEST_UTILITY_FUNC(setGetSpeed)(&controller);
}

TEST_FIXTURE(CombineControllerFixture, setGetSidewaysSpeed)
{
    TEST_UTILITY_FUNC(setGetSidewaysSpeed)(&controller);
}

TEST_FIXTURE(CombineControllerFixture, setGetVelocity)
{
    TEST_UTILITY_FUNC(setGetVelocity)(&controller);
}

TEST_FIXTURE(CombineControllerFixture, setGetDesiredVelocity)
{
    TEST_UTILITY_FUNC(setGetDesiredVelocity)(&controller);
}

TEST_FIXTURE(CombineControllerFixture, setGetDesiredPosition)
{
    TEST_UTILITY_FUNC(setGetDesiredPosition)(&controller);
}

// Depth methods
TEST_FIXTURE(CombineControllerFixture, setGetDepth)
{
    TEST_UTILITY_FUNC(setGetDepth)(&controller);
}

// TEST_FIXTURE(CombineControllerFixture, getEstimatedDepth)
// {
//     double estimatedDepth = 11.8;
//     depthController->estimatedDepth = estimatedDepth;
//     CHECK_EQUAL(estimatedDepth, controller.getEstimatedDepth());
// }

// TEST_FIXTURE(CombineControllerFixture, getEstimatedDepthDot)
// {
//     double estimatedDepthDot = 0.568;
//     depthController->estimatedDepthDot = estimatedDepthDot;
//     CHECK_EQUAL(estimatedDepthDot, controller.getEstimatedDepthDot());
// }

// TEST_FIXTURE(CombineControllerFixture, atDepth)
// {
//     double atDepth = true;
//     depthController->atDepthValue = atDepth;
//     CHECK_EQUAL(atDepth, controller.atDepth());
// }

// TEST_FIXTURE(CombineControllerFixture, holdCurrentDepth)
// {
//     CHECK_EQUAL(0, depthController->holdCurrentDepthCount);
//     controller.holdCurrentDepth();
//     CHECK_EQUAL(1, depthController->holdCurrentDepthCount);
// }

// Rotational methods
TEST_FIXTURE(CombineControllerFixture, yawVehicle)
{
    TEST_UTILITY_FUNC(yawVehicle)(&controller);
}

TEST_FIXTURE(CombineControllerFixture, pitchVehicle)
{
    TEST_UTILITY_FUNC(pitchVehicle)(&controller);
}

TEST_FIXTURE(CombineControllerFixture, rollVehicle)
{
    TEST_UTILITY_FUNC(rollVehicle)(&controller);
}

TEST_FIXTURE(CombineControllerFixture, setGetDesiredOrientation)
{
    TEST_UTILITY_FUNC(setGetDesiredOrientation)(&controller);
}

// TEST_FIXTURE(CombineControllerFixture, atOrientation)
// {
//     double atOrientation = true;
//     rotController->atOrientationValue = atOrientation;
//     CHECK_EQUAL(atOrientation, controller.atOrientation());
// }

// TEST_FIXTURE(CombineControllerFixture, holdCurrentHeading)
// {
//     CHECK_EQUAL(0, rotController->holdCurrentHeadingCount);
//     controller.holdCurrentHeading();
//     CHECK_EQUAL(1, rotController->holdCurrentHeadingCount);
// }

// Update
// TEST_FIXTURE(CombineControllerFixture, update)
// {
//     // Forces and torques to be returned
//     rotController->torque = math::Vector3(3, 7, 1);
//     transController->force = math::Vector3(11, 2, 5);
//     depthController->force = math::Vector3(1, 2, 3);

//     // Conditions for the controllers to act on
//     double timestep = 0.892;
//     math::Vector3 linearAcceleration(1,2,3);
//     math::Quaternion orientation(2.3, 4.12, 1.23, 1);
//     math::Vector3 angularRate(0.123, 6.56, 3.123);
//     double depth = 2.123;

//     // Set values in mock vehicle to be read
//     vehicle->linearAcceleration = linearAcceleration;
//     vehicle->orientation = orientation;
//     vehicle->angularRate = angularRate;
//     vehicle->depth = depth;

//     // Run the update
//     controller.update(timestep);

//     // Check depth controller
//     CHECK_EQUAL(timestep, depthController->timestep);
//     CHECK_EQUAL(depth, depthController->updateDepth);
//     CHECK_EQUAL(orientation, depthController->orientation);
    
//     // Check translational controller
//     CHECK_EQUAL(timestep, transController->timestep);
//     CHECK_EQUAL(linearAcceleration, transController->linearAcceleration);
//     CHECK_EQUAL(orientation, transController->orientation);

//     // Check rotational controller
//     CHECK_EQUAL(timestep, rotController->timestep);
//     CHECK_EQUAL(angularRate, rotController->angularRate);
//     CHECK_EQUAL(orientation, rotController->orientation);

//     // Check force returned
//     math::Vector3 expectedForce =
//         transController->force + depthController->force;
//     math::Vector3 expectedTorque = rotController->torque;

//     CHECK_EQUAL(expectedForce, vehicle->force);
//     CHECK_EQUAL(expectedTorque, vehicle->torque);
// }

TEST(SubsystemMaker)
{
    vehicle::IVehiclePtr veh(new MockVehicle());
    estimation::IStateEstimatorPtr est(new MockEstimator());
    core::EventHubPtr evt(new core::EventHub("eHub"));
    core::SubsystemList deps;
    deps.push_back(veh);
    deps.push_back(est);
    deps.push_back(evt);
    core::ConfigNode cfg(core::ConfigNode::fromString(
                             "{"
                             "'name' : 'Controller',"
                             "'type' : 'CombineController',"
                             "'TranslationalController' : {"
                             "    'type' : 'MockTranslationalController'"
                             "},"
                             "'DepthController' : {"
                             "    'type' : 'MockDepthController'"
                             "},"
                             "'RotationalController' : {"
                             "    'type' : 'MockRotationalController'"
                             "}}"));
    try {
        core::SubsystemPtr subsystem(core::SubsystemMaker::newObject(
                                         std::make_pair(cfg, deps)));
        boost::shared_ptr<control::CombineController> controller =
            boost::dynamic_pointer_cast<control::CombineController>(subsystem);
        CHECK(controller != NULL);
    } catch (core::MakerNotFoundException& ex) {
        CHECK(false && "CombineController Maker not found");
    }
}

} // SUITE(CombineController)
