/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestTranslationalControllerBase.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/test/include/MathChecks.h"

#include "control/include/TranslationalControllerBase.h"
#include "control/test/include/MockTranslationalControllerBase.h"
#include "control/test/include/TranslationalControllerTests.h"

using namespace ram;

struct TranslationalControllerBaseFixture
{
    TranslationalControllerBaseFixture() :
        controller(core::ConfigNode::fromString("{}"))
        {}

    MockTranslationalControllerBase controller;
};


SUITE(TranslationalControllerBase)
{
    
TEST_FIXTURE(TranslationalControllerBaseFixture, setGetVelocity)
{
    TEST_UTILITY_FUNC(setGetVelocity)(&controller);
}
    
TEST_FIXTURE(TranslationalControllerBaseFixture, setGetSpeed)
{
    TEST_UTILITY_FUNC(setGetSpeed)(&controller);
}

TEST_FIXTURE(TranslationalControllerBaseFixture, setGetSidewaysSpeed)
{
    TEST_UTILITY_FUNC(setGetSidewaysSpeed)(&controller);
}

} // SUITE(TranslationalControllerBase)

