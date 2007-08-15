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
#include "math/test/include/MathChecks.h"

// Bring Vector3, and other math classes into scope
using namespace ram::math;

TEST(Vector3AreClose)
{
    Vector3 a(0, 0, 1);
    Vector3 b(0, 0, 1.5);
    CHECK_CLOSE(a, b, 0.6);
}

TEST(Vector4AreClose)
{
    Vector4 a(0, 1, 1, 0);
    Vector4 b(0, 4, 1.5, 2);
    CHECK_CLOSE(a, b, 3.1);
}

TEST(QuaternionAreClose)
{
    Quaternion a(0, 1, 1, 0);
    Quaternion b(0, 4, 1.5, 2);
    CHECK_CLOSE(a, b, 3.1);
}

TEST(Matrix3AreClose)
{
    Matrix3 a(0, 2, 0, 1, 7, 3.3, 10, 8, 5.5);
    Matrix3 b(0, 2, 5, 1, 7, 3.3, 10, 8, 5.5);
    CHECK_CLOSE(a, b, 5.01);
}

TEST(Matrix4AreClose)
{
    Matrix4 a(0, 2, 0, 1, 7, 3.3, 10, 8, 5.5, 8.3, 9.2, 46.3, 5, 2, 1, 108.3);
    Matrix4 b(0, 2, 5, 1, 7, 3.3, 10, 8, 5.5, 8.3, 9.2, 46.3, 5, 2, 1, 108.3);
    CHECK_CLOSE(a, b, 5.01);
}

int main()
{
    return UnitTest::RunAllTests();
}
