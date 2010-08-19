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
#include <boost/lambda/bind.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "math/test/include/MathChecks.h"
#include "math/include/Events.h"
#include "vehicle/test/include/MockVehicle.h"
#include "control/include/ControllerBase.h"
#include "control/test/include/MockControllerBaseImp.h"
#include "control/test/include/IControllerImpTests.h"
#include "estimation/test/include/MockEstimator.h"
#include "core/include/EventHub.h"

using namespace ram;

struct ControlBaseFixture
{
    ControlBaseFixture() : 
        eventHub(core::EventHubPtr(new core::EventHub("eventHub"))),
        vehicle(new MockVehicle()),
        estimator(new MockEstimator()),
        mockController(eventHub,
                       ram::vehicle::IVehiclePtr(vehicle),
                       ram::estimation::IStateEstimatorPtr(estimator),
                       core::ConfigNode::fromString(
                           "{ 'name' : 'TestController'}"))
    {}

    core::EventHubPtr eventHub;
    MockVehicle* vehicle;
    MockEstimator* estimator;
    MockControllerBaseImp mockController;

};

SUITE(ControllerBase) {

TEST_FIXTURE(ControlBaseFixture, update)
{
    /* Make sure that update is able to correctly call doUpdate of the
     * implementation.  Also make sure that update is able to correctly
     * set forces and torques of the mock vehicle */

    // Values to passed off to the vehicle
    mockController.translationalForceOut = math::Vector3(3, 7, 1);
    mockController.rotationalTorqueOut = math::Vector3(11, 2, 5);

    // Values to be passed to controller
    double timestep = 0.892;

    // Run the update
    mockController.update(timestep);

    // Check the values are passed to subclass properly
    CHECK_EQUAL(timestep, mockController.timestep);
     
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


TEST_FIXTURE(ControlBaseFixture, setGetDepth)
{
    TEST_UTILITY_FUNC(setGetDepth)(&mockController);
}

TEST_FIXTURE(ControlBaseFixture, setGetSpeed)
{
    TEST_UTILITY_FUNC(setGetSpeed)(&mockController);
}

TEST_FIXTURE(ControlBaseFixture, setGetSidewaysSpeed)
{
    TEST_UTILITY_FUNC(setGetSidewaysSpeed)(&mockController);
}

TEST_FIXTURE(ControlBaseFixture, setGetVelocity)
{
    TEST_UTILITY_FUNC(setGetDesiredVelocity)(&mockController);
}

TEST_FIXTURE(ControlBaseFixture, setGetPosition)
{
    TEST_UTILITY_FUNC(setGetDesiredPosition)(&mockController);
}

TEST_FIXTURE(ControlBaseFixture, setGetOrientation)
{
    TEST_UTILITY_FUNC(setGetDesiredOrientation)(&mockController);
}

TEST_FIXTURE(ControlBaseFixture, holdCurrentPosition)
{
    math::Vector2 currentPos(5,3);
    estimator->estPosition = currentPos;
    mockController.setDesiredPosition(math::Vector2(2,7),
                                      control::IController::INERTIAL_FRAME);
    CHECK_EQUAL(false, mockController.atPosition());

    mockController.holdCurrentPosition();
    CHECK_EQUAL(mockController.getDesiredPosition(control::IController::INERTIAL_FRAME),
                currentPos);
    CHECK_EQUAL(true, mockController.atPosition());
}

TEST_FIXTURE(ControlBaseFixture, holdCurrentOrientation)
{
    math::Quaternion normQuat(2,5,3,6);
    normQuat.normalise();

    estimator->estOrientation = normQuat;
    mockController.setDesiredOrientation(math::Quaternion::IDENTITY);
    CHECK_EQUAL(false, mockController.atOrientation());

    mockController.holdCurrentOrientation();
    CHECK_ARRAY_CLOSE(mockController.getDesiredOrientation(), normQuat, 4, 0.0001);
    CHECK_EQUAL(true, mockController.atOrientation());
}

TEST_FIXTURE(ControlBaseFixture, holdCurrentHeading)
{
    // Test 1

    // Set the current "measured" orientation
    math::Quaternion orientation(math::Degree(15), math::Vector3::UNIT_Z);
    estimator->estOrientation = orientation;

    CHECK_EQUAL(false, mockController.atOrientation());

    // Tell the controller to hold current heading (ignoring roll and pitch)
    mockController.holdCurrentHeading();

    // Create the expected orientation and make sure the desired orientation
    // was set properly based on the vehicle current orientation
    math::Quaternion expectedOrientation(math::Degree(15), 
					 math::Vector3::UNIT_Z);

    CHECK_EQUAL(expectedOrientation, mockController.getDesiredOrientation());
    CHECK_EQUAL(true, mockController.atOrientation());

    // Test 2
    
    // set the current "measured" orientation
    math::Quaternion orientation2(0.0028, 0.0028, 0.7071, 0.7071);
    estimator->estOrientation = orientation2;

    CHECK_EQUAL(false, mockController.atOrientation());

    // tell the controller to hold current heading
    mockController.holdCurrentHeading();
    
    // expected output
    math::Quaternion expectedOrientation2(0, 0, 0.7071, 0.7071);

    CHECK_CLOSE(expectedOrientation2, mockController.getDesiredOrientation(),
                0.001);
    CHECK_EQUAL(true, mockController.atOrientation());
}

TEST_FIXTURE(ControlBaseFixture, holdCurrentDepth)
{
    estimator->estDepth = 4.5;
    mockController.setDepth(8.6);

    CHECK_EQUAL(false, mockController.atDepth());

    mockController.holdCurrentDepth();

    CHECK_EQUAL(4.5, mockController.getDepth());
    CHECK_EQUAL(true, mockController.atDepth());
}

void invertBool(bool *orig, ram::core::EventPtr event)
{
    *orig = !(*orig);
}

TEST_FIXTURE(ControlBaseFixture, atDepthUpdate)
{
    bool eventHappened = false;
    estimator->estDepth = 4.5;
    mockController.setDepth(8.6);

    CHECK_EQUAL(false, mockController.atDepth());

    mockController.subscribe(control::IController::AT_DEPTH,
                             boost::bind(invertBool, &eventHappened, _1));

    mockController.setDepth(4.5);
    
    CHECK_EQUAL(true, mockController.atDepth());
    CHECK_EQUAL(true, eventHappened);

}

TEST_FIXTURE(ControlBaseFixture, atPositionUpdate)
{
    bool eventHappened = false;
    estimator->estPosition = math::Vector2(3,7);
    mockController.setDesiredPosition(math::Vector2(0,0),
                                      control::IController::INERTIAL_FRAME);

    CHECK_EQUAL(false, mockController.atPosition());

    mockController.subscribe(control::IController::AT_POSITION,
                             boost::bind(invertBool, &eventHappened, _1));

    mockController.setDesiredPosition(math::Vector2(3,7),
                                      control::IController::INERTIAL_FRAME);

    CHECK_EQUAL(true, mockController.atPosition());
    CHECK_EQUAL(true, eventHappened);
}

TEST_FIXTURE(ControlBaseFixture, atVelocityUpdate)
{
    bool eventHappened = false;
    estimator->estVelocity = math::Vector2(2.6, 5.4);
    mockController.setDesiredVelocity(math::Vector2(0,0),
                                      control::IController::INERTIAL_FRAME);

    CHECK_EQUAL(false, mockController.atVelocity());

    mockController.subscribe(control::IController::AT_VELOCITY,
                             boost::bind(invertBool, &eventHappened, _1));

    mockController.setDesiredVelocity(math::Vector2(2.6, 5.4),
                                      control::IController::INERTIAL_FRAME);

    CHECK_EQUAL(true, mockController.atVelocity());
    CHECK_EQUAL(true, eventHappened);
}

TEST_FIXTURE(ControlBaseFixture, atOrientationUpdate)
{
    bool eventHappened = false;
    estimator->estOrientation = math::Quaternion(0, 0, 0.2756, 0.9613);
    mockController.setDesiredOrientation(math::Quaternion(0,0,0,1));

    CHECK_EQUAL(false, mockController.atOrientation());

    mockController.subscribe(control::IController::AT_ORIENTATION,
                             boost::bind(invertBool, &eventHappened, _1));

    mockController.setDesiredOrientation(math::Quaternion(0, 0, 0.2756, 0.9613));

    CHECK_EQUAL(true, mockController.atOrientation());
    CHECK_EQUAL(true, eventHappened);
}

TEST_FIXTURE(ControlBaseFixture, yawVehicle)
{
    TEST_UTILITY_FUNC(yawVehicle)(&mockController);
}

TEST_FIXTURE(ControlBaseFixture, pitchVehicle)
{
    TEST_UTILITY_FUNC(pitchVehicle)(&mockController);
}

TEST_FIXTURE(ControlBaseFixture, rollVehicle)
{
    TEST_UTILITY_FUNC(rollVehicle)(&mockController);
}


} // SUITE(ControllerBase)
