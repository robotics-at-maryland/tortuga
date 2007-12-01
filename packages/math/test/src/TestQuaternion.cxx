/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/test/src/TestChecks.h
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
//#include "math/test/include/MathChecks.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"

using namespace ram::math;

TEST(ScalarMult)
{
    Quaternion quat(1,2,3,4);
    Quaternion exp(3,6,9,12);

    CHECK_EQUAL(exp, quat * 3);
}

TEST(normalise)
{
    Quaternion quat(4,3,2,1);
    Quaternion exp(0.73029674334022143,
                   0.54772255750516607,
                   0.36514837167011072,
                   0.18257418583505536);
    
    CHECK_EQUAL(30, quat.normalise());
    CHECK_EQUAL(exp, quat);
}

TEST(getRoll)
{
    // In our reference frame, roll is about the X axis
    ram::math::Matrix3 mat;
    mat.FromAxisAngle(ram::math::Vector3::UNIT_X, ram::math::Degree(53));    
    ram::math::Quaternion quat;
    quat.FromRotationMatrix(mat);

    CHECK_CLOSE(0, quat.getYaw().valueDegrees(), 0.0001);
    CHECK_CLOSE(53, quat.getRoll().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getPitch().valueDegrees(), 0.0001);
}

TEST(getPitch)
{
    // In our reference frame, pitch is about the Y axis
    ram::math::Matrix3 mat;
    mat.FromAxisAngle(ram::math::Vector3::UNIT_Y, ram::math::Degree(53));    
    ram::math::Quaternion quat;
    quat.FromRotationMatrix(mat);

    CHECK_CLOSE(0, quat.getYaw().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getRoll().valueDegrees(), 0.0001);
    CHECK_CLOSE(53, quat.getPitch().valueDegrees(), 0.0001);
}

TEST(getYaw)
{
    // In our reference frame, yaw is about the Z axis
    ram::math::Matrix3 mat;
    mat.FromAxisAngle(ram::math::Vector3::UNIT_Z, ram::math::Degree(53));    
    ram::math::Quaternion quat;
    quat.FromRotationMatrix(mat);

    CHECK_CLOSE(53, quat.getYaw().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getRoll().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getPitch().valueDegrees(), 0.0001);
}
