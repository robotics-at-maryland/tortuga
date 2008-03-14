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

SUITE(MatrixNTest) {

TEST(transpose)
{
    double data[] = {1, 2,  3, 4,  5, 6,  7, 8};
    double dataExp[] = {1, 3, 5, 7,   2, 4, 6, 8};

    MatrixN reg(data, 4, 2);
    MatrixN exp(dataExp, 2, 4);

    CHECK_CLOSE(exp, reg.transpose(), 0.001);
}

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

TEST(multiplication)
{
	double data1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	MatrixN n1(data1, 3, 3);

	double data2[] = {11, 12, 13, 14, 15, 16, 17, 18, 19};
	MatrixN n2(data2, 3, 3);

	MatrixN n3 = n1 * n2;

	double dataExp[] = {90, 96, 102, 216, 231, 246, 342, 366, 390};
	MatrixN exp = MatrixN(dataExp, 3, 3);

	CHECK_CLOSE(n3, exp, 0.001);
}

TEST(invert)
{
	double data1[] = {10, 14, 2, 4};
	MatrixN n1(data1, 2, 2);

	n1.invert();

	double dataExp[] = {0.3333, -1.16666, -0.16666, 0.83333};
	MatrixN exp = MatrixN(dataExp, 2, 2);

	CHECK_CLOSE(n1, exp, 0.001);
}

TEST(cholesky)
{
	double data1[] = {
            1, 1, 1, 1, 1,
            1, 2, 3, 4, 5,
            1, 3, 6, 10, 15,
            1, 4, 10, 20, 35,
            1, 5, 15, 35, 70
        };
	MatrixN n1(data1, 5, 5);

        // The actual result is the bottom left diagonal of the matrix.  The in
        // place version of this function leaves old data in the upper right
        // section.
	double dataExp[] = {
            1, 1, 1, 1, 1,
            1, 1, 3, 4, 5,
            1, 2, 1, 10, 15,
            1, 3, 3, 1, 35,
            1, 4, 6, 4, 1
        };
	MatrixN exp(dataExp, 5, 5);

        n1.factorCholesky();
	CHECK_CLOSE(n1, exp, 0.001);
}

}