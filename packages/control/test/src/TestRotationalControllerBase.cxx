/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestRotationalControllerBase.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "math/test/include/MathChecks.h"

#include "control/include/RotationalControllerBase.h"
#include "control/test/include/MockRotationalControllerBase.h"
#include "control/test/include/RotationalControllerTests.h"

using namespace ram;

struct RotationalControllerBaseFixture
{
    RotationalControllerBaseFixture() :
        controller(core::ConfigNode::fromString("{}"))
        {}

    MockRotationalControllerBase controller;
};


SUITE(RotationalControllerBase)
{

TEST_FIXTURE(RotationalControllerBaseFixture, yawVehicle)
{
    TEST_UTILITY_FUNC(yawVehicle)(&controller);
}

TEST_FIXTURE(RotationalControllerBaseFixture, pitchVehicle)
{
    TEST_UTILITY_FUNC(pitchVehicle)(&controller);
}

TEST_FIXTURE(RotationalControllerBaseFixture, rollVehicle)
{
    TEST_UTILITY_FUNC(rollVehicle)(&controller);
}

TEST_FIXTURE(RotationalControllerBaseFixture, setDesiredOrientation)
{
    TEST_UTILITY_FUNC(setDesiredOrientation)(&controller);
}

TEST_FIXTURE(RotationalControllerBaseFixture, holdCurrentHeading)
{
    TEST_UTILITY_FUNC(holdCurrentHeading)
        (&controller,
         boost::bind(&control::IRotationalControllerImp::rotationalUpdate,
                     &controller, 0.0, _1, math::Vector3::ZERO));
}

TEST_FIXTURE(RotationalControllerBaseFixture, atOrientation)
{
    TEST_UTILITY_FUNC(atOrientation)
        (&controller,
         boost::bind(&control::IRotationalControllerImp::rotationalUpdate,
                     &controller, 0.0, _1, math::Vector3::ZERO),
         boost::bind(&control::IRotationalControllerImp::yawVehicle,
                     &controller, 0.0));
}


} // SUITE(RotationalControllerBase)

