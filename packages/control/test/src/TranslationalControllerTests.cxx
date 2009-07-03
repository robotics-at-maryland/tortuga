/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TranslationalControllerTests.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "control/test/include/TranslationalControllerTests.h"

using namespace ram;

TEST_UTILITY_IMP(setGetVelocity,
                 (ram::control::ITranslationalController* controller))
{
    ram::math::Vector2 velocity(1.5, 2.9);
    controller->setVelocity(velocity);
    CHECK_EQUAL(velocity, controller->getVelocity());
}

TEST_UTILITY_IMP(setGetSpeed,
                 (ram::control::ITranslationalController* controller))
{
    double speed = 1.5;
    controller->setSpeed(speed);
    CHECK_EQUAL(speed, controller->getSpeed());
}

TEST_UTILITY_IMP(setGetSidewaysSpeed,
                 (ram::control::ITranslationalController* controller))
{
    double sidewaysSpeed = 1.5;
    controller->setSidewaysSpeed(sidewaysSpeed);
    CHECK_EQUAL(sidewaysSpeed, controller->getSidewaysSpeed());
}
