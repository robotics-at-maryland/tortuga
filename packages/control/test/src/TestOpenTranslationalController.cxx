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

#include "control/include/OpenLoopTranslationalController.h"

using namespace ram;

struct OpenLoopTranslationalControllerFixture
{
    OpenLoopTranslationalControllerFixture() :
        controller(core::ConfigNode::fromString("{}"))
        {}

    control::OpenLoopTranslationalController controller;
};


SUITE(OpenLoopTranslationalController)
{

TEST_FIXTURE(OpenLoopTranslationalControllerFixture, update)
{
    // Forward while pitched down 
    math::Quaternion orien(math::Degree(45), math::Vector3::UNIT_Y);    
    controller.setSpeed(1);

    // The expected
    math::Vector3 result = math::Vector3::ZERO;
    math::Vector3 expected = math::Vector3(0.707106781, 0, 0.707106781);

    // Run the controller
    result = controller.translationalUpdate(0, math::Vector3::ZERO, orien,
                                            math::Vector2::ZERO,
                                            math::Vector2::ZERO);

    CHECK_CLOSE(expected, result, 0.0001);


    // Speed test with yaw

    // At 0 depth, pitch 0, yawed 45 degrees
    orien = math::Quaternion(math::Degree(45), math::Vector3::UNIT_Z);

    // The expected
    result = math::Vector3::ZERO;
    expected = math::Vector3(1, 0, 0);

    // Run the controller
    result = controller.translationalUpdate(0, math::Vector3::ZERO, orien,
                                            math::Vector2::ZERO,
                                            math::Vector2::ZERO);

    CHECK_CLOSE(expected, result, 0.0001);
}
    
} // SUITE(OpenLoopTranslationalController)    
