/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/test/src/DesiredStateTests.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "core/include/EventHub.h"
#include "core/include/ConfigNode.h"
#include "core/include/EventConnection.h"

#include "control/include/DesiredState.h"
#include "control/include/IController.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

using namespace ram;

struct DesiredStateFixture
{
    DesiredStateFixture() :
        eventHub(core::EventHubPtr(new core::EventHub("eventHub"))),
        desiredState(control::DesiredStatePtr(
                         new control::DesiredState(
                             core::ConfigNode::fromString("{ 'name' : 'TestDesiredState'}"),
                             core::EventHubPtr(eventHub))))

    {}
    
    core::EventHubPtr eventHub;
    control::DesiredStatePtr desiredState;
};

// helper for testing whether an event occurred
void invertBool(bool *orig, ram::core::EventPtr event)
{
    *orig = !(*orig);
}   
    
TEST_FIXTURE(DesiredStateFixture, setGetDepth)
{
    bool testEvent = false;

    // listen for depth update events
    desiredState->subscribe(control::IController::DESIRED_DEPTH_UPDATE,
                            boost::bind(invertBool, &testEvent, _1));

    double depth = 6.7;
    desiredState->setDesiredDepth(depth);
    CHECK_EQUAL(depth, desiredState->getDesiredDepth());
    CHECK_EQUAL(true, testEvent);
}

TEST_FIXTURE(DesiredStateFixture, setGetOrientation)
{
    bool testEvent = false;

    // listen for depth update events
    desiredState->subscribe(control::IController::DESIRED_ORIENTATION_UPDATE,
                            boost::bind(invertBool, &testEvent, _1));

    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
    desiredState->setDesiredOrientation(expected);
    math::Quaternion actual(desiredState->getDesiredOrientation());
    CHECK_EQUAL(expected, actual);
    CHECK_EQUAL(true, testEvent);
}

TEST_FIXTURE(DesiredStateFixture, setGetAngularRate)
{
    math::Vector3 expected(5.3,1.4,9.6);
    desiredState->setDesiredAngularRate(expected);
    math::Vector3 actual(desiredState->getDesiredAngularRate());
    CHECK_EQUAL(expected, actual);
}

TEST_FIXTURE(DesiredStateFixture, setGetPosition)
{
    bool testEvent = false;

    // listen for depth update events
    desiredState->subscribe(control::IController::DESIRED_POSITION_UPDATE,
                            boost::bind(invertBool, &testEvent, _1));

    math::Vector2 expected(-5.3,2.5);
    desiredState->setDesiredPosition(expected);
    math::Vector2 actual(desiredState->getDesiredPosition());
    CHECK_EQUAL(expected, actual);
    CHECK_EQUAL(true, testEvent);
}

TEST_FIXTURE(DesiredStateFixture, setGetVelocity)
{
    bool testEvent = false;

    // listen for depth update events
    desiredState->subscribe(control::IController::DESIRED_VELOCITY_UPDATE,
                            boost::bind(invertBool, &testEvent, _1));

    math::Vector2 expected(2.5,-5.3);
    desiredState->setDesiredVelocity(expected);
    math::Vector2 actual(desiredState->getDesiredVelocity());
    CHECK_EQUAL(expected, actual);
    CHECK_EQUAL(true, testEvent);
}

