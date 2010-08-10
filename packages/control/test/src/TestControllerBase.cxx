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
#include "estimation/test/include/MockEstimator.h"

using namespace ram;

struct ControlBaseFixture
{
    ControlBaseFixture() : 
        vehicle(new MockVehicle()),
        estimator(new MockEstimator()),
        mockController(ram::vehicle::IVehiclePtr(vehicle),
                       ram::estimation::IStateEstimatorPtr(estimator),
                       core::ConfigNode::fromString(
                           "{ 'name' : 'TestController'}"))
    {}

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

TEST_FIXTURE(ControlBaseFixture, newDepthSet)
{
//     double actualDesiredDepth = 0;
//     double actualDepth = 0;
    
//     // Subscribe to the events
//     mockController.subscribe(ram::control::IController::DESIRED_DEPTH_UPDATE,
//                              boost::bind(depthHelper, &actualDesiredDepth, _1));
//     mockController.subscribe(ram::control::IController::AT_DEPTH,
//                              boost::bind(depthHelper, &actualDepth, _1));

//     // Set atDepth and the current value
//     mockController.atDepthValue = false;
//     mockController.desiredDepth = 0;
    
//     // Test desired depth update
//     mockController._newDepthSet(5.6);
//     CHECK_EQUAL(5.6, actualDesiredDepth);
//     CHECK_EQUAL(0, actualDepth);

//     // Test at depth update
//     mockController.atDepthValue = true;
//     mockController._newDepthSet(2.8);
//     CHECK_EQUAL(2.8, actualDesiredDepth);
//     CHECK_EQUAL(2.8, actualDepth);
}

TEST_FIXTURE(ControlBaseFixture, setGetDepth)
{
}

TEST_FIXTURE(ControlBaseFixture, setGetSpeed)
{
}

TEST_FIXTURE(ControlBaseFixture, setGetSidewaysSpeed)
{
}

TEST_FIXTURE(ControlBaseFixture, setGetVelocity)
{
}

TEST_FIXTURE(ControlBaseFixture, setGetPosition)
{
}

TEST_FIXTURE(ControlBaseFixture, setGetOrientation)
{
}

TEST_FIXTURE(ControlBaseFixture, holdCurrentPosition)
{
}

TEST_FIXTURE(ControlBaseFixture, holdCurrentOrientation)
{
}

TEST_FIXTURE(ControlBaseFixture, holdCurrentHeading)
{
}

TEST_FIXTURE(ControlBaseFixture, atDepthUpdate)
{
}

TEST_FIXTURE(ControlBaseFixture, atPositionUpdate)
{
}

TEST_FIXTURE(ControlBaseFixture, atVelocityUpdate)
{
}

TEST_FIXTURE(ControlBaseFixture, atOrientationUpdate)
{
}


} // SUITE(ControllerBase)
