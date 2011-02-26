/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/estimation/test/src/TestnIMU.cxx
 */

#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "estimation/include/Utility.h"
#include "math/test/include/MathChecks.h"
#include "math/include/Math.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "math/include/Vector2.h"

using namespace ram;

// Mag + Accel in the interial frame  
static math::Vector3 interialMag(0.1813, 0, 0.0845);
static math::Vector3 interialAccel(0, 0, 1);

// Testing methodology:
/*
  Given the inertial magnetometer "IM" and accelerometer vectors "IA", and a
  quaternion "R".  If you rotate IM (getting "VM") and IA (getting "VA") by R
  then quaternionFromIMU function should feed you inverse of R, "IR" when given
  IM and IA.  Then if you rotate VM, and VA with IR you get back the original
  IM, and IA vectors.
*/
TEST_UTILITY(quatIMU, (math::Vector3 expVehicleMag,
                       math::Vector3 expVehicleAccel,
                       math::Quaternion magAccelRotation))
{
    // Now rotate the Mag + Accel into vehicle frame
    math::Vector3 vehicleMag = magAccelRotation * interialMag;
    math::Vector3 vehicleAccel = magAccelRotation * interialAccel;
    
    // Check to make sure the rotation was made properly
    // if zero vectors are passed in, it means we aren't double checking the
    // transformed mag and accel values.
    if (expVehicleMag != math::Vector3::ZERO)
        CHECK_CLOSE(expVehicleMag, vehicleMag, 0.0001);
    if (expVehicleAccel != math::Vector3::ZERO)
        CHECK_CLOSE(expVehicleAccel, vehicleAccel, 0.0001);

    // Now build the expected result.  The returned quaternion should be the
    // oposite of the quaternion we used to rotate the mag/accel vectors from
    // the interial frame.  This is because the vehicle is rotates in the
    // interial frame in the opposite direction the mag/accel vectors rotate in
    // the vehicle frame
    math::Quaternion expected(magAccelRotation.UnitInverse());

    // Now check to make sure our expected quaternion puts the mag and accel
    // vectors back into the interial frame
    CHECK_CLOSE(interialMag, expected * vehicleMag, 0.0001);
    CHECK_CLOSE(interialAccel, expected * vehicleAccel, 0.0001);

    // Finially test the function
    math::Quaternion quatResult(
        estimation::Utility::quaternionFromMagAccel(vehicleMag, vehicleAccel));

    // Check to make sure the result rotates the vehicle frame mag and accel
    // vectors back to the interial frame
    CHECK_CLOSE(interialMag, quatResult * vehicleMag, 0.0001);
    CHECK_CLOSE(interialAccel, quatResult * vehicleAccel, 0.0001);

    // Finially check the quaternion
    CHECK_ARRAY_CLOSE(expected.ptr(), quatResult.ptr(), 4, 0.0001);
}

TEST(quaternionFromRate)
{
    //given an old quaternion value
    math::Quaternion qOld(0, 0, 0, 1);
    //an angular rate
    math::Vector3 w(1, 2, 1);
    //and a timestep
    double deltaT = 1.2;
    //it should look like this
    math::Quaternion qExp(0.3375, 0.6751, 0.3375, 0.5625);
    //run the function
    math::Quaternion qOut = 
        ram::estimation::Utility::quaternionFromRate(qOld,w,deltaT);
    //check if close, test case data from MATLAB using 4 sig figs
    CHECK_ARRAY_CLOSE(qExp,qOut,4,0.001);


    //another test case with different data
    math::Quaternion qOld2(0.4082, 0.8165, 0.4082, 0.0000);
    math::Vector3 w2(-2, 3, -1);
    double deltaT2  = 0.3;
    math::Quaternion qExp2(0.0890, 0.6586, 0.7298, -0.1602);
    math::Quaternion qOut2 = 
        estimation::Utility::quaternionFromRate(qOld2, w2, deltaT2);
    CHECK_ARRAY_CLOSE(qExp2, qOut2, 4, 0.001);
}
