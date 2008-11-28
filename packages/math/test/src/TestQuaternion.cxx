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
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"
#include "math/include/Math.h"
#include "math/include/Helpers.h"
#include "math/include/Vector3.h"

using namespace ram::math;

TEST(ScalarMult)
{
    Quaternion quat(1,2,3,4);
    Quaternion exp(3,6,9,12);

    CHECK_EQUAL(exp, quat * 3);
}

TEST(normalise)
{
    Quaternion quat(4,3,2,1);

    // Ensure proper length is returned
    CHECK_EQUAL(30, quat.normalise());

    // Make sure the length of the normalized quaternion is 1
    CHECK_CLOSE(1, quat.normalise(), 0.0001);

    // Ensure ratio is maintained
    CHECK_CLOSE(4, quat.x / quat.w, 0.001);
    CHECK_CLOSE(3, quat.y / quat.w, 0.001);
    CHECK_CLOSE(2, quat.z / quat.w, 0.001);
}

TEST(FromAngleAxis)
{
    Matrix3 mat;
    Quaternion expected;
    mat.FromAxisAngle(ram::math::Vector3::UNIT_X, ram::math::Degree(90));    
    expected.FromRotationMatrix(mat);

    Quaternion actual;
    actual.FromAngleAxis(Degree(90), Vector3::UNIT_X);
    CHECK_CLOSE(expected, actual, 0.0001);

    Quaternion actualConstructed(Degree(90), Vector3::UNIT_X);
    CHECK_CLOSE(expected, actualConstructed, 0.0001);    
}

TEST(getRoll)
{
    // In our reference frame, roll is about the X axis
    Quaternion quat(Degree(53), Vector3::UNIT_X);

    CHECK_CLOSE(0, quat.getYaw().valueDegrees(), 0.0001);
    CHECK_CLOSE(53, quat.getRoll().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getPitch().valueDegrees(), 0.0001);
}

TEST(getPitch)
{
    // In our reference frame, pitch is about the Y axis
    Quaternion quat(Degree(53), Vector3::UNIT_Y);

    CHECK_CLOSE(0, quat.getYaw().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getRoll().valueDegrees(), 0.0001);
    CHECK_CLOSE(53, quat.getPitch().valueDegrees(), 0.0001);
}

TEST(getYaw)
{
    // In our reference frame, yaw is about the Z axis
    Quaternion quat(Degree(53), Vector3::UNIT_Z);

    CHECK_CLOSE(53, quat.getYaw().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getRoll().valueDegrees(), 0.0001);
    CHECK_CLOSE(0, quat.getPitch().valueDegrees(), 0.0001);
}

TEST(getRollPitchYaw)
{
    // Yawed 53 degrees, pitched 15, and rolled 10
    Quaternion quat(Degree(53), Vector3::UNIT_Z);
    quat = quat * Quaternion(Degree(15), Vector3::UNIT_Y);
    quat = quat * Quaternion(Degree(10), Vector3::UNIT_X);

    // False is need to return the "intuitive" result other wise you get the
    // shortest yaw needed
    CHECK_CLOSE(53, quat.getYaw(false).valueDegrees(), 0.0001);
    CHECK_CLOSE(10, quat.getRoll(false).valueDegrees(), 0.0001);
    CHECK_CLOSE(15, quat.getPitch(false).valueDegrees(), 0.0001);
}

TEST(errorQuaternion)
{
  /*
    //i suspect this test case is "backwards"
    Quaternion quatA(Quaternion::IDENTITY);
    Quaternion quatB(Degree(45), Vector3::UNIT_Y);

    Quaternion expected;
    findErrorQuaternion(quatA.ptr(), quatB.ptr(), expected.ptr());
    
    Quaternion result = quatA.errorQuaternion(quatB);

    CHECK_CLOSE(expected, result, 0.0001);*/

    //simple test
    Quaternion quatA1(0,0,0.2756,0.9613);
    Quaternion quatB1(0,0,0,1);
    Quaternion result1 = quatA1.errorQuaternion(quatB1);
    Quaternion expected1(0,0,0.2756,0.9613);
    CHECK_CLOSE(expected1,result1,0.0001);
    

    //more complicated test
    Quaternion quatA2(-0.3320,0.6640,-0.6640,0.0890);
    Quaternion quatB2(-0.7913,0,0.1583,0.5906);
    Quaternion result2 = quatA2.errorQuaternion(quatB2);
    Quaternion expected2(-0.0206,0.9702,0.1192,0.2102);
    CHECK_CLOSE(expected2,result2,0.0001);

    //same test as above but q_meas and q_des are not unit quaternions
    
    Quaternion quatA3(-0.9659,1.9319,-1.9319,0.2588);
    Quaternion quatB3(-1.2941,0,0.2588,0.9659);
    Quaternion result3 = quatA3.errorQuaternion(quatB3);
    Quaternion expected3(-0.0206,0.9702,0.1192,0.2102);
    CHECK_CLOSE(expected3,result3,0.0001);
}


TEST(quaternionDerivative)
{
	Quaternion q(0.243,0.243,0.243,0.9071);
	Vector3 w(2,-4,-3);
	Quaternion expected(1.0286,-1.2067,-2.0897,0.6075);
	Quaternion result = q.derivative(w);
	CHECK_CLOSE(expected,result,0.0002);

	Quaternion q2(0,0,0.9659,0.2588);
	Vector3 w2(0,0,3);
	Quaternion expected2(0,0,0.3882,-1.4489);
	Quaternion result2 = q2.derivative(w2);
	CHECK_CLOSE(expected,result,0.0002);

	Quaternion q3(-0.9659,-2.8978,0.9659,0.2588);
	Vector3 w3(-2,-2,-3);
	Quaternion expected3(5.0538,-2.6736,-2.3201,-2.4148);
	Quaternion result3 = q3.derivative(w3);
	CHECK_CLOSE(expected,result,0.0002);
}

TEST(toQ)
{
  //create a result array that is too big
  MatrixN big(5,5);
  big.identity();
  CHECK_EQUAL(big.getRows(),5);
  CHECK_EQUAL(big.getCols(),5);

  //test trivial case (result should be resized as well)
  Quaternion q(0,0,0,1);
  q.toQ(&big);
  CHECK_EQUAL(big.getRows(),4);
  CHECK_EQUAL(big.getCols(),3);
  double expArray[] = {1,0,0,0,1,0,0,0,1,0,0,0};
  MatrixN expMat(expArray,4,3);
  CHECK_CLOSE(expMat,big,0.0005);
  
  //test yawed orientation case
  Quaternion q2(0,0,0.9659,0.2588);
  q2.toQ(&big);
  double expArray2[]={0.2588, -0.9659, 0, 0.9659,0.2588,0,0,0,0.2588,0,0,-0.9659};
  MatrixN expMat2(expArray2,4,3);
  CHECK_CLOSE(expMat2,big,0.0005);
  
  //test terribly confusing orientation
  Quaternion q3(-0.9659,1.9319,-1.9319,0.2588);
  q3.toQ(&big);
  double expArray3[]={0.2588,1.9319,1.9319,-1.9319,0.2588,0.9659,-1.9319,-0.9659,0.2588,0.9659,-1.9319,1.9319};
  MatrixN expMat3(expArray3,4,3);
  CHECK_CLOSE(expMat3,big,0.0005);

}
