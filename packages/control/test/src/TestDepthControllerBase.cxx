/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestDepthControllerBase.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "math/test/include/MathChecks.h"

#include "control/include/DepthControllerBase.h"
#include "control/test/include/MockDepthControllerBase.h"
#include "control/test/include/DepthControllerTests.h"

using namespace ram;

struct DepthControllerBaseFixture
{
    DepthControllerBaseFixture() :
        controller(core::ConfigNode::fromString("{}"))
        {}

    MockDepthControllerBase controller;
};


SUITE(DepthControllerBase)
{

TEST_FIXTURE(DepthControllerBaseFixture, setGetDepth)
{
    TEST_UTILITY_FUNC(setGetDepth)(&controller);
}

void dummy()
{
}

TEST_FIXTURE(DepthControllerBaseFixture, atDepth)
{
    TEST_UTILITY_FUNC(atDepth)
        (&controller,
         boost::bind(&control::IDepthControllerImp::depthUpdate,
                     &controller, 0.0, _1, math::Quaternion::IDENTITY),
         boost::bind(&dummy));
}

} // SUITE(RotationalControllerBase)

