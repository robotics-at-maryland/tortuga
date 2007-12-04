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
    CHECK_ARRAY_CLOSE(exp.ptr(), quat.ptr(), 4, 0.0001);
}

TEST(FromAngleAxis)
{
    Matrix3 mat;
    Quaternion expected;
    mat.FromAxisAngle(ram::math::Vector3::UNIT_X, ram::math::Degree(90));    
    expected.FromRotationMatrix(mat);

    Quaternion actual;
    actual.FromAngleAxis(Degree(90), Vector3::UNIT_X);
    CHECK_ARRAY_CLOSE(expected.ptr(), actual.ptr(), 4, 0.0001);

    Quaternion actualConstructed(Degree(90), Vector3::UNIT_X);
    CHECK_ARRAY_CLOSE(expected.ptr(), actualConstructed.ptr(), 4, 0.0001);    
}

TEST(getRoll)
{
    // In our reference frame, roll is about the X axis
    Quaternion quat(Degree(53), Vector3::UNIT_X);

    CHECK_CLOSE(0, quat.getYaw().valueDegrees(), 0.0001);
    CHECK_CLOSE(53, quat.getRoll().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getPitch().valueDegrees(), 0.0001);
}

TEST(getPitch)
{
    // In our reference frame, pitch is about the Y axis
    Quaternion quat(Degree(53), Vector3::UNIT_Y);

    CHECK_CLOSE(0, quat.getYaw().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getRoll().valueDegrees(), 0.0001);
    CHECK_CLOSE(53, quat.getPitch().valueDegrees(), 0.0001);
}

TEST(getYaw)
{
    // In our reference frame, yaw is about the Z axis
    Quaternion quat(Degree(53), Vector3::UNIT_Z);

    CHECK_CLOSE(53, quat.getYaw().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getRoll().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getPitch().valueDegrees(), 0.0001);
}

TEST(getRollPitchYaw)
{
    // Yawed 53 degrees, pitched 15, and rolled 10
    Quaternion quat(Degree(53), Vector3::UNIT_Z);
    quat = quat * Quaternion(Degree(15), Vector3::UNIT_Y);
    quat = quat * Quaternion(Degree(10), Vector3::UNIT_X);

    // False is need to return the "intuitive" result other wise you get the
    // shortest yaw needed
    CHECK_CLOSE(53, quat.getYaw(false).valueDegrees(), 0.0001);
    CHECK_CLOSE(10, quat.getRoll(false).valueDegrees(), 0.0001);
    CHECK_CLOSE(15, quat.getPitch(false).valueDegrees(), 0.0001);
}
