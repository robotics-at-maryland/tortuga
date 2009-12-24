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
#include "vehicle/test/include/MockIMU.h"
#include "vehicle/test/include/MockDepthSensor.h"
#include "vehicle/test/include/MockVelocitySensor.h"
#include "vehicle/test/include/MockPositionSensor.h"
#include "vehicle/test/include/MockStateEstimator.h"

RAM_VEHILCE_REGISTER_IDEVICE_MAKER(MockDevice, MockDevice);
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(MockIMU, MockIMU);
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(MockDepthSensor, MockDepthSensor);
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(MockVelocitySensor, MockVelocitySensor);
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(MockPositionSensor, MockPositionSensor);
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(MockStateEstimator, MockStateEstimator);


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
