/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestnIMU.cxx
 */

#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/test/include/MathChecks.h"
#include "math/include/Math.h"

#define private public
#include "vehicle/include/device/IMU.h"
#undef private

using namespace ram::math;

struct IMU
{
    IMU() { /* some setup */ }
    ~IMU() { /* some teardown */ }
    
};



// Mag + Accel in the interial frame  
static Vector3 interialMag(0.1813, 0, 0.0845);
static Vector3 interialAccel(0, 0, 1);


// Testing methodology:
/*
  Given the inertial magnetometer "IM" and accelerometer vectors "IA", and a
  quaternion "R".  If you rotate IM (getting "VM") and IA (getting "VA") by R
  then quaternionFromIMU function should feed you inverse of R, "IR".  Then if
  you rotate VM, and VA with IR you get back the original IM, and IA vectors.
*/
TEST_UTILITY(quatIMU, (Vector3 expVehicleMag, Vector3 expVehicleAccel,
                       Quaternion magAccelRotation))
{
    // Now rotate the Mag + Accel into vehicle frame
    Vector3 vehicleMag = magAccelRotation * interialMag;
    Vector3 vehicleAccel = magAccelRotation * interialAccel;
    
    // Check to make sure the rotation was made properly
    // if zero vectors are passed in, it means we aren't double checking the
    // transformed mag and accel values.
    if (expVehicleMag != Vector3::ZERO)
        CHECK_CLOSE(expVehicleMag, vehicleMag, 0.0001);
    if (expVehicleAccel != Vector3::ZERO)
        CHECK_CLOSE(expVehicleAccel, vehicleAccel, 0.0001);

    // Now build the expected result.  The returned quaternion should be the
    // oposite of the quaternion we used to rotate the mag/accel vectors from
    // the interial frame.  This is because the vehicle is rotates in the
    // interial frame in the opposite direction the mag/accel vectors rotate in
    // the vehicle frame
    Quaternion expected(magAccelRotation.UnitInverse());

    // Now check to make sure our expected quaternion puts the mag and accel
    // vectors back into the interial frame
    CHECK_CLOSE(interialMag, expected * vehicleMag, 0.0001);
    CHECK_CLOSE(interialAccel, expected * vehicleAccel, 0.0001);

    // Finially test the function
    double result[4] = {0, 0, 0, 0};
    ram::vehicle::device::IMU::quaternionFromIMU(vehicleMag.ptr(),
                                                 vehicleAccel.ptr(),
                                                 result);

    Quaternion quatResult(result);

    // Check to make sure the result rotates the vehicle frame mag and accel
    // vectors back to the interial frame
    CHECK_CLOSE(interialMag, quatResult * vehicleMag, 0.0001);
    CHECK_CLOSE(interialAccel, quatResult * vehicleAccel, 0.0001);

    // Finially check the quaternion
    CHECK_ARRAY_CLOSE(expected.ptr(), result, 4, 0.0001);
}


TEST_FIXTURE(IMU, quaternionFromIMU)
{
    // Used for axis angle conversions
    Quaternion magAccelRotation(Quaternion::IDENTITY);

    // No rotation
    Vector3 expVehicleMag(interialMag);
    Vector3 expVehicleAccel(interialAccel);
                            
    TEST_UTILITY_FUNC(quatIMU)(expVehicleMag, expVehicleAccel,
                               magAccelRotation);
    
    // 90 Degree right yaw
    // This means vehicle frame Mag + Accel vectors will be 90 degrees left
    expVehicleMag = Vector3(0, 0.1813, 0.0845);
    expVehicleAccel = Vector3(0, 0, 1);
    // Build a quaternion which rotates them 90 degees left
    magAccelRotation.FromAngleAxis(Degree(90), Vector3::UNIT_Z);

    TEST_UTILITY_FUNC(quatIMU)(expVehicleMag, expVehicleAccel,
                               magAccelRotation);

    // 180 Degree right yaw (ie south)
    // This means vehicle frame Mag + Accel vectors will be 180 degrees left
    expVehicleMag = Vector3(-0.1813, 0, 0.0845);
    expVehicleAccel = Vector3(0, 0, 1);
    // Build a quaternion which rotates them 180 degees left
    magAccelRotation.FromAngleAxis(Degree(180), Vector3::UNIT_Z);

    TEST_UTILITY_FUNC(quatIMU)(expVehicleMag, expVehicleAccel,
                               magAccelRotation);

    // Vehicle 90 Pitch up
    // This means vehicle frame Mag + Accel vectors will be 90 degrees down
    expVehicleMag = Vector3(0.0845, 0, -0.1813);
    expVehicleAccel = Vector3(1, 0, 0);
    magAccelRotation.FromAngleAxis(Degree(90), Vector3::UNIT_Y);
    
    TEST_UTILITY_FUNC(quatIMU)(expVehicleMag, expVehicleAccel,
                               magAccelRotation);

    // Vehicle 90 Roll left
    // This means vehicle frame Mag + Accel vectors will be 90 deg roll right
    expVehicleMag = Vector3(0.1813, -0.0845, 0);
    expVehicleAccel = Vector3(0, -1, 0);
    magAccelRotation.FromAngleAxis(Degree(90), Vector3::UNIT_X);
    
    TEST_UTILITY_FUNC(quatIMU)(expVehicleMag, expVehicleAccel,
                               magAccelRotation);

    // These next ones aren't derived manually, I just combine some odd
    // rotations to try and break the method
    magAccelRotation = Quaternion(Degree(21), Vector3::UNIT_X) *
        Quaternion(Degree(87), Vector3::UNIT_Z);
    TEST_UTILITY_FUNC(quatIMU)(Vector3::ZERO, Vector3::ZERO, magAccelRotation);

    magAccelRotation.FromAngleAxis(Degree(33),
                                   Vector3(0.365148, 0.912871, 0.182574));
    TEST_UTILITY_FUNC(quatIMU)(Vector3::ZERO, Vector3::ZERO, magAccelRotation);
    
    // This test doesn't seem to make sense
/*
    Vector3 mag2(-0.0518, -0.0335, -0.1903);
    Vector3 accel2(0.766, -0.1116, -0.6330);
    double quat_exp2[4] = {0.0032, 0.0790, 0.9029, 0.0368};

    ram::vehicle::device::IMU::quaternionFromIMU(mag2.ptr(), accel2.ptr(),
    quat_res);
    
    CHECK_ARRAY_CLOSE(quat_exp2, quat_res, 4, 0.0001);
*/
}
