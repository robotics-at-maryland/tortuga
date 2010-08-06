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

/* Tests moved */

SUITE(TranslationalControllerBase)
{

// TEST_FIXTURE(TranslationalControllerBaseFixture, controlMode)
// {
//     // Test default
//     CHECK_EQUAL(control::TranslationalControllerBase::ControlMode::OPEN_LOOP,
//                 controller.getMode());

//     // Make sure it goes to VELOCITY when you set velocity
//     controller.setVelocity(math::Vector2(5,5));
//     CHECK_EQUAL(control::TranslationalControllerBase::ControlMode::VELOCITY,
//                 controller.getMode());

//     // Make sure it goes back to OPEN_LOOP when you set speed
//     controller.setSpeed(5);
//     CHECK_EQUAL(control::TranslationalControllerBase::ControlMode::OPEN_LOOP,
//                 controller.getMode());

// }

} // SUITE(TranslationalControllerBase)

