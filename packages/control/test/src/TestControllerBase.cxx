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

// TEST_FIXTURE(ControlBaseFixture, update)
// {
//     // Values to passed off to the vehicle
//     mockController.translationalForceOut = math::Vector3(3, 7, 1);
//     mockController.rotationalTorqueOut = math::Vector3(11, 2, 5);

//     // Values to be passed to controller
//     double timestep = 0.892;
//     vehicle->linearAcceleration = math::Vector3(1,2,3);
//     vehicle->orientation = math::Quaternion(2.3, 4.12, 1.23, 1);
//     vehicle->angularRate = math::Vector3(0.123, 6.56, 3.123);
//     vehicle->depth = 2.123;

//     // Run the update
//     mockController.update(timestep);

//     // Check the values are passed to subclass properly
//     CHECK_EQUAL(timestep, mockController.timestep);
//     CHECK_EQUAL(vehicle->linearAcceleration, mockController.linearAcceleration);
//     CHECK_EQUAL(vehicle->orientation, mockController.orientation);
//     CHECK_EQUAL(vehicle->angularRate, mockController.angularRate);
//     CHECK_EQUAL(vehicle->depth, mockController.depth);

//     // Check that the vehicle gets passed the proper values
//     CHECK_EQUAL(mockController.translationalForceOut, vehicle->force);
//     CHECK_EQUAL(mockController.rotationalTorqueOut, vehicle->torque);
// }

// void depthHelper(double* result, ram::core::EventPtr event)
// {
//     ram::math::NumericEventPtr nevent =
//         boost::dynamic_pointer_cast<ram::math::NumericEvent>(event);
//     *result = nevent->number;
// }

// TEST_FIXTURE(ControlBaseFixture, newDepthSet)
// {
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
// }

} // SUITE(ControllerBase)
