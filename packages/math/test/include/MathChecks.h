/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/test/include/MathChecks.h
 */

#include <cstdio>

// Library Includes
#include "UnitTest++/Checks.h"

// Project Includes
#include "math/include/Vector3.h"
#include "math/include/Vector4.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"
#include "math/include/Matrix4.h"

// Place inside UnitTest++ namespace to override there usage
namespace UnitTest {
    template<>
    bool AreClose(const ram::math::Vector3& expected,
                  const ram::math::Vector3& actual,
                  const double& tolerance)
    {
        return ArrayAreClose(expected.ptr(), actual.ptr(), 3, tolerance);
    }

    template<>
    bool AreClose(const ram::math::Vector4& expected,
                  const ram::math::Vector4& actual,
                  const double& tolerance)
    {
        return ArrayAreClose(expected.ptr(), actual.ptr(), 4, tolerance);
    }

    template<>
    bool AreClose(const ram::math::Quaternion& expected,
                  const ram::math::Quaternion& actual,
                  const double& tolerance)
    {
        return ArrayAreClose(expected.ptr(), actual.ptr(), 4, tolerance);
    }
    
    template<>
    bool AreClose(const ram::math::Matrix3& expected,
                  const ram::math::Matrix3& actual,
                  const double& tolerance)
    {
        return ArrayAreClose(expected[0], actual[0], 9, tolerance);
    }

    template<>
    bool AreClose(const ram::math::Matrix4& expected,
                  const ram::math::Matrix4& actual,
                  const double& tolerance)
    {
        return ArrayAreClose(expected[0], actual[0], 16, tolerance);
    }
    
} // namespace UnitTest
