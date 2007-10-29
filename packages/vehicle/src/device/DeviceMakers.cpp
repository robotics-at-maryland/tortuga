/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/device/DeviceMakers.cpp
 */

// All registration are done in one file to keep the compilation dependency on 
// the factory system to a minimum

// Project Includes
#include "vehicle/include/device/IDeviceMaker.h"
#include "vehicle/include/device/IMU.h"
#include "vehicle/include/device/PSU.h"
#include "vehicle/include/device/Thruster.h"

// Register each device with the factor system
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(ram::vehicle::device::IMU, IMU);
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(ram::vehicle::device::PSU, PSU);
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(ram::vehicle::device::Thruster, Thruster);
