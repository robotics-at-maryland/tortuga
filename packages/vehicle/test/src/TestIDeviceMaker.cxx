/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/test/src/IDeviceMaker.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Test Includes
#include "vehicle/include/device/IDeviceMaker.h"
#include "vehicle/test/include/MockDevice.h"

RAM_VEHILCE_REGISTER_IDEVICE_MAKER(MockDevice, MockDevice);


using namespace ram::vehicle;
TEST(DeviceMaker)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'name' : 'IMU', 'type' : 'MockDevice' }"));
    device::IDevice* device = device::IDeviceMaker::newObject(config);
    
    CHECK_EQUAL("IMU", device->getName());
}
