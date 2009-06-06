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

} // SUITE(TestMatrix2) 
