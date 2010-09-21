/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/estimation/test/src/TestTortuga.cxx
 */


// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "estimation/include/Utility.h"

#include "math/test/include/MathChecks.h"

using namespace ram;

SUITE(EstimationUtility) {

TEST(quaternionFromMagAccel)
{
    // Estimation pictched downward 45 degrees
    math::Vector3 accel(0.707106781, 0, -0.707106781); // normally (0, 0, -1)
    math::Vector3 mag(0.707106781, 0, 0.707106781); // normally (1, 0, 0)
    math::Quaternion expectedOrientation(math::Degree(45),
                                         math::Vector3::UNIT_Y);

    math::Quaternion result(
        estimation::Utility::quaternionFromMagAccel(mag, accel));
    CHECK_CLOSE(expectedOrientation, result, 0.0001);


    // Estimation rotated 45 degrees left
    math::Vector3 accel2(0, 0, -1); // normally (0, 0, -1)
    math::Vector3 mag2(0.707106781, -0.707106781, 0); // normally (1, 0, 0)
    math::Quaternion expectedOrientation2(math::Degree(45),
                                          math::Vector3::UNIT_Z);

    math::Quaternion result2(
        estimation::Utility::quaternionFromMagAccel(mag2, accel2));
    CHECK_CLOSE(expectedOrientation2, result2, 0.0001);

    // Estimation rolled 45 degrees right
    math::Vector3 accel3(0, -0.707106781, -0.707106781); // normally (0, 0, -1)
    math::Vector3 mag3(1, 0, 0); // normally (1, 0, 0)
    math::Quaternion expectedOrientation3(math::Degree(45),
                                          math::Vector3::UNIT_X);

    math::Quaternion result3(
        estimation::Utility::quaternionFromMagAccel(mag3, accel3));
    CHECK_CLOSE(expectedOrientation3, result3, 0.0001);
}

} // SUITE(EstimationUtility)
