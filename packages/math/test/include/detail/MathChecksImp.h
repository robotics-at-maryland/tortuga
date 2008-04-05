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
    bool AreClose(const ram::math::Vector2& expected,
                  const ram::math::Vector2& actual,
                  const double& tolerance)
    {
        return ArrayAreClose(expected.ptr(), actual.ptr(), 2, tolerance);
    }
    
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
    bool AreClose(const ram::math::Matrix2& expected,
                  const ram::math::Matrix2& actual,
                  const double& tolerance)
    {
        return ArrayAreClose(expected[0], actual[0], 4, tolerance);
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

	template<>
    bool AreClose(const ram::math::MatrixN& expected,
                  const ram::math::MatrixN& actual,
                  const double& tolerance)
    {
		if (expected.getRows() != actual.getRows() || expected.getCols() != actual.getCols())
			return false;
        return ArrayAreClose(expected[0], actual[0], 
			actual.getRows()*actual.getCols(), tolerance);
    }

    template<>
    bool AreClose(const ram::math::VectorN& expected,
                  const ram::math::VectorN& actual,
                  const double& tolerance)
    {
		if (expected.numElements() != actual.numElements())
			return false;
        return ArrayAreClose(expected.ptr(), actual.ptr(), 
			actual.numElements(), tolerance);
    }
    
} // namespace UnitTest

#endif // RAM_MATH_TEST_MATCHCHECKS_H_08_14_2007
