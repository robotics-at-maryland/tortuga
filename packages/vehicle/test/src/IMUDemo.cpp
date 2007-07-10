/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/test/src/IMUDemo.cpp
 */

// UNIX Includes
#include <unistd.h>

// Project Includes
#include "vehicle/include/device/IMU.h"
#include "core/include/ConfigNode.h"

using namespace ram::core;
using namespace ram::vehicle::device;

int main()
{
    // Create IMU Device
    IMU imu(0, ConfigNode::fromString("{}"));

    // Start IMU running in the background
    imu.background(5);

    // Sleep (and watch internal prints)
    sleep(1000);
    return 0;
}
