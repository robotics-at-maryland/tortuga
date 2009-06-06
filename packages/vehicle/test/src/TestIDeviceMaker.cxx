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
#include <boost/tuple/tuple.hpp>

// Test Includes
#include "vehicle/include/device/IDeviceMaker.h"
#include "vehicle/test/include/MockDevice.h"

RAM_VEHILCE_REGISTER_IDEVICE_MAKER(MockDevice, MockDevice);


using namespace ram::vehicle;
TEST(DeviceMaker)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'name' : 'IMU', 'type' : 'MockDevice' }"));
    device::IDevicePtr device = device::IDeviceMaker::newObject( 
        boost::make_tuple(config,
                          ram::core::EventHubPtr(),
                          ram::vehicle::IVehiclePtr() ));
    
    CHECK_EQUAL("IMU", device->getName());
}
