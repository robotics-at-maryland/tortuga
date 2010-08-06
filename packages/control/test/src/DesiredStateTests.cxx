/*
 * Copyright (C) 2010 Robotics at Maryland
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/test/src/DesiredStateTests.cxx
 */


/* Make test utilities for all functions in control/include/DesiredState.h */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/test/include/DesiredStateTests.h"

using namespace ram;


TEST_UTILITY_IMP(setGetDesiredDepth, 
                 (ram::controltest::DesiredStatePtr desiredState))
{
    double depth = 6.7;
    desiredState->setDesiredDepth(depth);
    CHECK_EQUAL(depth,desiredState->getDesiredDepth());
}

TEST_UTILITY_IMP(setGetDesiredOrientation, 
                 (ram::controltest::DesiredStatePtr desiredState))
{
    math::Quaternion expected(math::Degree(30), math::Vector3::UNIT_X);
    desiredState->setDesiredOrientation(expected);
    math::Quaternion actual(desiredState->getDesiredOrientation());
    CHECK_EQUAL(expected, actual);
}

TEST_UTILITY_IMP(setGetDesiredAngularRate, 
                 (ram::controltest::DesiredStatePtr desiredState))
{
    math::Vector3 expected(5.3,1.4,9.6);
    desiredState->setDesiredAngularRate(expected);
    math::Vector3 actual(desiredState->getDesiredAngularRate());
    CHECK_EQUAL(expected,actual);
}

TEST_UTILITY_IMP(setGetDesiredPosition,
                 (ram::controltest::DesiredStatePtr desiredState))
{
    math::Vector2 expected(-5.3,2.5);
    desiredState->setDesiredPosition(expected);
    math::Vector2 actual(desiredState->getDesiredPosition());
    CHECK_EQUAL(expected,actual);
}

TEST_UTILITY_IMP(setGetDesiredVelocity,
                 (ram::controltest::DesiredStatePtr desiredState))
{
    math::Vector2 expected(2.5,-5.3);
    desiredState->setDesiredVelocity(expected);
    math::Vector2 actual(desiredState->getDesiredVelocity());
    CHECK_EQUAL(expected,actual);
}
