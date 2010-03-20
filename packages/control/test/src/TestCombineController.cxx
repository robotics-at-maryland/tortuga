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
#include "core/include/Subsystem.h"
#include "core/include/SubsystemMaker.h"
#include "math/test/include/MathChecks.h"
#include "math/include/Events.h"

#include "vehicle/test/include/MockVehicle.h"

#include "control/include/CombineController.h"
#include "control/test/include/MockDepthController.h"
#include "control/test/include/MockTranslationalController.h"
#include "control/test/include/MockRotationalController.h"

//#include "control/test/include/ControllerTests.h"

//#include "core/test/include/BufferedAppender.h"

using namespace ram;

struct CombineControllerFixture
{
    CombineControllerFixture() :
        vehicle(new MockVehicle()),
        transController(0),
        depthController(0),
        rotController(0),
        controller(vehicle::IVehiclePtr(vehicle),
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
TEST_FIXTURE(CombineControllerFixture, setSpeed)
{
    double speed = 5.8;
    controller.setSpeed(speed);
    CHECK_EQUAL(speed, transController->speedSet);
}

TEST_FIXTURE(CombineControllerFixture, setSidewaysSpeed)
{
    double sidewaysSpeed = 2.8;
    controller.setSidewaysSpeed(sidewaysSpeed);
    CHECK_EQUAL(sidewaysSpeed, transController->sidewaysSpeedSet);
}

TEST_FIXTURE(CombineControllerFixture, getSpeed)
{
    double speed = 5.8;
    transController->speed = speed;
    CHECK_EQUAL(speed, controller.getSpeed());
}

TEST_FIXTURE(CombineControllerFixture, getSidewaysSpeed)
{
    double sidewaysSpeed = 2.8;
    transController->sidewaysSpeed = sidewaysSpeed;
    CHECK_EQUAL(sidewaysSpeed, controller.getSidewaysSpeed());
}

// Depth methods
TEST_FIXTURE(CombineControllerFixture, setDepth)
{
    double depth = 1.67;
    controller.setDepth(depth);
    CHECK_EQUAL(depth, depthController->depthSet);
}

TEST_FIXTURE(CombineControllerFixture, getDepth)
{
    double depth = 5.8;
    depthController->depth = depth;
    CHECK_EQUAL(depth, controller.getDepth());
}

TEST_FIXTURE(CombineControllerFixture, getEstimatedDepth)
{
    double estimatedDepth = 11.8;
    depthController->estimatedDepth = estimatedDepth;
    CHECK_EQUAL(estimatedDepth, controller.getEstimatedDepth());
}

TEST_FIXTURE(CombineControllerFixture, getEstimatedDepthDot)
{
    double estimatedDepthDot = 0.568;
    depthController->estimatedDepthDot = estimatedDepthDot;
    CHECK_EQUAL(estimatedDepthDot, controller.getEstimatedDepthDot());
}

TEST_FIXTURE(CombineControllerFixture, atDepth)
{
    double atDepth = true;
    depthController->atDepthValue = atDepth;
    CHECK_EQUAL(atDepth, controller.atDepth());
}

TEST_FIXTURE(CombineControllerFixture, holdCurrentDepth)
{
    CHECK_EQUAL(0, depthController->holdCurrentDepthCount);
    controller.holdCurrentDepth();
    CHECK_EQUAL(1, depthController->holdCurrentDepthCount);
}

// Translational methods
TEST_FIXTURE(CombineControllerFixture, yawVehicle)
{
    double degrees = 78.2;
    controller.yawVehicle(degrees);
    CHECK_EQUAL(degrees, rotController->yaw);
}

TEST_FIXTURE(CombineControllerFixture, pitchVehicle)
{
    double degrees = 45.2;
    controller.pitchVehicle(degrees);
    CHECK_EQUAL(degrees, rotController->pitch);
}

TEST_FIXTURE(CombineControllerFixture, rollVehicle)
{
    double degrees = 16.8;
    controller.rollVehicle(degrees);
    CHECK_EQUAL(degrees, rotController->roll);
}

TEST_FIXTURE(CombineControllerFixture, setDesiredOrientation)
{
    math::Quaternion orientation(0.2, 1.2, 2.5, 1);
    controller.setDesiredOrientation(orientation);
    CHECK_EQUAL(orientation, rotController->desiredOrientationSet);
}

TEST_FIXTURE(CombineControllerFixture, getDesiredOrientation)
{
    math::Quaternion orientation(8.2, 1.2, 0.5, 1);
    rotController->desiredOrientation = orientation;
    CHECK_EQUAL(orientation, controller.getDesiredOrientation());
}

TEST_FIXTURE(CombineControllerFixture, atOrientation)
{
    double atOrientation = true;
    rotController->atOrientationValue = atOrientation;
    CHECK_EQUAL(atOrientation, controller.atOrientation());
}

TEST_FIXTURE(CombineControllerFixture, holdCurrentHeading)
{
    CHECK_EQUAL(0, rotController->holdCurrentHeadingCount);
    controller.holdCurrentHeading();
    CHECK_EQUAL(1, rotController->holdCurrentHeadingCount);
}

// Update
TEST_FIXTURE(CombineControllerFixture, update)
{
    // Forces and torques to be returned
    rotController->torque = math::Vector3(3, 7, 1);
    transController->force = math::Vector3(11, 2, 5);
    depthController->force = math::Vector3(1, 2, 3);

    // Conditions for the controllers to act on
    double timestep = 0.892;
    math::Vector3 linearAcceleration(1,2,3);
    math::Quaternion orientation(2.3, 4.12, 1.23, 1);
    math::Vector3 angularRate(0.123, 6.56, 3.123);
    double depth = 2.123;

    // Set values in mock vehicle to be read
    vehicle->linearAcceleration = linearAcceleration;
    vehicle->orientation = orientation;
    vehicle->angularRate = angularRate;
    vehicle->depth = depth;

    // Run the update
    controller.update(timestep);

    // Check depth controller
    CHECK_EQUAL(timestep, depthController->timestep);
    CHECK_EQUAL(depth, depthController->updateDepth);
    CHECK_EQUAL(orientation, depthController->orientation);
    
    // Check translational controller
    CHECK_EQUAL(timestep, transController->timestep);
    CHECK_EQUAL(linearAcceleration, transController->linearAcceleration);
    CHECK_EQUAL(orientation, transController->orientation);

    // Check rotational controller
    CHECK_EQUAL(timestep, rotController->timestep);
    CHECK_EQUAL(angularRate, rotController->angularRate);
    CHECK_EQUAL(orientation, rotController->orientation);

    // Check force returned
    math::Vector3 expectedForce =
        transController->force + depthController->force;
    math::Vector3 expectedTorque = rotController->torque;

    CHECK_EQUAL(expectedForce, vehicle->force);
    CHECK_EQUAL(expectedTorque, vehicle->torque);
}


TEST(SubsystemMaker)
{
    vehicle::IVehiclePtr veh(new MockVehicle());
    core::SubsystemList deps;
    deps.push_back(veh);
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
    } catch (core::MakerNotFoundException& ex) {
        CHECK(false && "CombineController Maker not found");
    }
}


} // SUITE(CombineController)
