/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/test/include/MathChecks.h
 */

#ifndef RAM_MATH_TEST_MATCHCHECKS_H_08_14_2007
#define RAM_MATH_TEST_MATCHCHECKS_H_08_14_2007

// Library Includes
#include "UnitTest++/Checks.h"

// Project Includes
#include "math/include/Vector3.h"
#include "math/include/Vector4.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"
#include "math/include/Matrix4.h"
#include "math/include/MatrixN.h"
#include "math/include/VectorN.h"

// Place inside UnitTest++ namespace to override there usage
namespace UnitTest {
    template<>
    bool AreClose(const ram::math::Vector3& expected,
                  const ram::math::Vector3& actual,
                  const double& tolerance);

    template<>
    bool AreClose(const ram::math::Vector4& expected,
                  const ram::math::Vector4& actual,
                  const double& tolerance);

    template<>
    bool AreClose(const ram::math::Quaternion& expected,
                  const ram::math::Quaternion& actual,
                  const double& tolerance);
    
    template<>
    bool AreClose(const ram::math::Matrix3& expected,
                  const ram::math::Matrix3& actual,
                  const double& tolerance);

    template<>
    bool AreClose(const ram::math::Matrix4& expected,
                  const ram::math::Matrix4& actual,
                  const double& tolerance);

	template<>
    bool AreClose(const ram::math::MatrixN& expected,
                  const ram::math::MatrixN& actual,
                  const double& tolerance);

    template<>
    bool AreClose(const ram::math::VectorN& expected,
                  const ram::math::VectorN& actual,
                  const double& tolerance);
    
} // namespace UnitTest

#endif // RAM_MATH_TEST_MATCHCHECKS_H_08_14_2007
