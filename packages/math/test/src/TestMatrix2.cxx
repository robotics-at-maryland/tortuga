/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/test/src/TestMatrix2.h
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/test/include/MathChecks.h"
#include "math/include/Matrix2.h"

using namespace ram::math;

SUITE(TestMatrix2) {

TEST(fromAngle)
{
    Vector2 start(1,0);
    Matrix2 rotationMatrix;
    rotationMatrix.fromAngle(Degree(45));

    Vector2 expected(Vector2(1,1).normalisedCopy());
    Vector2 result = (start * rotationMatrix);
    
    CHECK_CLOSE(expected, result, 0.001);
}

TEST(transpose)
{
    Matrix2 start(1,2,3,4);
    Matrix2 expected(1,3,2,4);
    
    CHECK_EQUAL(true, start.Transpose() == expected);
}

TEST(determinant)
{
    Matrix2 start(1,2,3,4);
    double expected = -2;

    CHECK_CLOSE(expected, start.Determinant(), 0.001);
}

TEST(inverse)
{
    Matrix2 start(1,2,3,4);
    Matrix2 expected(-2, 1.5, 1, -0.5);
    Matrix2 result = start.Inverse();

    CHECK_CLOSE(expected[0][0], result[0][0], 0.001);
    CHECK_CLOSE(expected[0][1], result[0][1], 0.001);
    CHECK_CLOSE(expected[1][0], result[1][0], 0.001);
    CHECK_CLOSE(expected[1][1], result[1][1], 0.001);
}

} // SUITE(TestMatrix2) 
