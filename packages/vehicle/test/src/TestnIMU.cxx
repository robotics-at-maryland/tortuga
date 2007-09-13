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

#define private public
#include "vehicle/include/device/IMU.h"
#undef private

using namespace ram::math;

struct IMU
{
    IMU() { /* some setup */ }
    ~IMU() { /* some teardown */ }
    
};


TEST_FIXTURE(IMU, quaternionFromIMU)
{
    Vector3 mag1(0.1813, 0, 0.0845);
    Vector3 accel1(0, 0, 1);
    double quat_exp1[4] = {1, 0, 0, 0};
    double quat_res[4] = {0};
    
    ram::vehicle::device::IMU::quaternionFromIMU(mag1.ptr(), accel1.ptr(),
                                                 quat_res);
    CHECK_ARRAY_CLOSE(quat_exp1, quat_res, 4, 0.0001);

    Vector3 mag2(-0.0518, -0.0335, -0.1903);
    Vector3 accel2(0.766, -0.1116, -0.6330);
    double quat_exp2[4] = {0.0032, 0.0790, 0.9029, 0.0368};

    ram::vehicle::device::IMU::quaternionFromIMU(mag2.ptr(), accel2.ptr(),
                                                 quat_res);
    CHECK_ARRAY_CLOSE(quat_exp2, quat_res, 4, 0.0001);
}

int main()
{
    return UnitTest::RunAllTests();
}
