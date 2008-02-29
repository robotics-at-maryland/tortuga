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

using namespace ram::math;

TEST(addition)
{
	MatrixN n1(3, 3);
	n1.identity();
	
	MatrixN n2(3, 3);
	n2.identity();

	MatrixN n3 = n1 + n2;

	double data[] = {2,0,0, 0,2,0, 0,0,2};
	MatrixN expected(data, 3, 3);

    CHECK_CLOSE(n3, expected, 0.001);
}
