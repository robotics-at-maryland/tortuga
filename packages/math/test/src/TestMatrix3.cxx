/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Gland <jgland@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Gland <jgland@umd.edu>
 * File:  packages/math/test/src/TestMatrix3.h
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/test/include/MathChecks.h"
#include "math/include/Matrix3.h"

using namespace ram::math;

SUITE(TestMatrix3) {

TEST(skewSymmetricMatrix)
{
    Vector3 start(1,-2,3);
    Matrix3 result;
    result.ToSkewSymmetric(start);

    Matrix3 expected(0,-3,-2,3,0,-1,2,1,0);
    
    CHECK_CLOSE(expected, result, 0.001);
}
TEST(fromOuterProduct)
{
    Vector3 a(1,2,3);
    Vector3 b(4,5,6);
    Matrix3 test=Matrix3::fromOuterProduct(a,b);        
    Matrix3 expected(4,5,6,8,10,12,12,15,18);
    CHECK_CLOSE(expected, test, 0.001);

    
}

} // SUITE(TestMatrix2) 
