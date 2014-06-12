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
#include "math/include/MatrixN.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"
#include "math/include/Math.h"
#include "math/include/Helpers.h"
#include "math/include/VectorN.h"

using namespace ram::math;

SUITE(VectorNTest) {

TEST(construct)
{
    double data[] = {40, 46};
    VectorN vec(data, 2);

    CHECK_EQUAL(40, vec[0]);
    CHECK_EQUAL(46, vec[1]);
}

TEST(matrixMultiply1)
{
    double vecData[] = {1, 2, 3};
    double matData[] = {4, 5,    6, 7,    8, 9};
    double expData[] = {40, 46};

    VectorN vec(vecData, 3);
    MatrixN mat(matData, 3, 2);

    VectorN exp(expData, 2);
    VectorN act = vec * mat;
  
    CHECK_CLOSE(exp, act, 0.001);
}

TEST(matrixMultiply2)
{
    double vecData[] = {-2, 4, 8, 30};
    double matData[] = {1, 2, 3, 4,    5, 6, 7, 8,    9, 10, 11, 12};
    double expData[] = {150, 310, 470};

    VectorN vec(vecData, 4);
    MatrixN mat(matData, 3, 4);

    VectorN exp(expData, 3);
    VectorN act = mat * vec;
  
    CHECK_CLOSE(exp, act, 0.001);
}

}
