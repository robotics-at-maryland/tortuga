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

TEST_FIXTURE(CombineControllerFixture, translate)
{
    TEST_UTILITY_FUNC(translate)(&controller);
}

TEST_FIXTURE(CombineControllerFixture, changeDepth)
{
    TEST_UTILITY_FUNC(changeDepth)(&controller);
}

TEST_FIXTURE(CombineControllerFixture, rotate)
{
    TEST_UTILITY_FUNC(rotate)(&controller);
}

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
