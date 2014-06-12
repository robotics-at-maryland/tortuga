/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/test/src/TestChecks.h
 */
#include <iostream>
// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/test/include/MathChecks.h"
#include "math/include/Helpers.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

using namespace ram::math;

TEST(sign)
{
    double x = -3.572;

    CHECK(sign(x) == -1);

    x = 1.256;

    CHECK(sign(x) == 1);

    x = 0;
    CHECK(sign(x) == 0);

}

TEST(factorial)
{
    int n = 4;
    int exp = 4 * 3 * 2 * 1;
    CHECK(factorial(n) == exp);


    n = 6;
    exp = 6 * 5 * 4 * 3 * 2 * 1;
    CHECK(factorial(n) == exp);
}
