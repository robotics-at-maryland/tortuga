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

// This is an attempt to understand the function
TEST(quaternionFromnCb)
{
    Matrix3 mat;
    Quaternion expected;
    Quaternion result;
    
    // Simple rotation
    mat.FromAxisAngle(ram::math::Vector3::UNIT_X, ram::math::Degree(90));    
    expected.FromRotationMatrix(mat);
    
    quaternionFromnCb((double (*)[3])(mat[0]), result.ptr());
    CHECK_ARRAY_CLOSE(expected.ptr(), result.ptr(), 4, 0.0001);

    // Full rotation
    mat.FromAxisAngle(ram::math::Vector3::UNIT_Z, ram::math::Degree(180));    
    expected.FromRotationMatrix(mat);
    
    quaternionFromnCb((double (*)[3])(mat[0]), result.ptr());
    CHECK_ARRAY_CLOSE(expected.ptr(), result.ptr(), 4, 0.0001);
    
    // Rotation around a "complex" axis
    Vector3 axis(87, 9, 28);
    axis.normalise();
    mat.FromAxisAngle(axis, ram::math::Degree(56));
    expected.FromRotationMatrix(mat);
    
    quaternionFromnCb((double (*)[3])(mat[0]), result.ptr());
    CHECK_ARRAY_CLOSE(expected.ptr(), result.ptr(), 4, 0.0001);

    // Another complex axis
    axis = Vector3(0.365148, 0.912871, 0.182574);
    axis.normalise();
    mat.FromAxisAngle(axis, ram::math::Degree(33));
    expected.FromRotationMatrix(mat);

    quaternionFromnCb((double (*)[3])(mat[0]), result.ptr());
    CHECK_ARRAY_CLOSE(expected.ptr(), result.ptr(), 4, 0.0001);

    

    // Result: this appears to do this same thing as "FromRotationMatrix"
}

TEST(rotationMatrixFromQuaternion)
{
    // Pitch downward by 45 degrees
    Quaternion orien(Degree(45), Vector3::UNIT_Y);
    
    Vector3 start(0, 0, -1);
    Vector3 expected(-0.707106781, 0, -0.707106781);

    // This answer should be correct
    Vector3 resultOgre = orien * start;
    CHECK_CLOSE(expected, resultOgre, 0.0001);

    // Now find the answer from the function we are testing
    Vector3 resultHelpers;
    
    double rotationMatrix[3][3];
    rotationMatrixFromQuaternion(orien.ptr(), &rotationMatrix[0][0]);
    matrixMult3x1by3x3(rotationMatrix, start.ptr(), resultHelpers.ptr());

    CHECK_CLOSE(expected, resultHelpers, 0.0001);
}
