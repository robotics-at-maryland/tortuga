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
#include "math/include/Helpers.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"

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
