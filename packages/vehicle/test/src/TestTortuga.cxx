/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestTortuga.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
//#include "math/test/include/MathChecks.h"
#include "vehicle/include/Vehicle.h"
#include "core/include/ConfigNode.h"
#include "vehicle/test/include/MockDevice.h"
#include "vehicle/test/include/MockIMU.h"

using namespace ram;

static const std::string CONFIG("{'depthCalibSlope' : 33.01,"
                                " 'depthCalibIntercept' : 94}");

struct VehicleFixture

{
    VehicleFixture() {
        veh = new vehicle::Vehicle(core::ConfigNode::fromString(CONFIG));
    }
    ~VehicleFixture() {
        delete veh;
    }

    vehicle::Vehicle* veh;
};

TEST(DeviceCreation)
{
	std::string config =  "{'depthCalibSlope':33.01,'depthCalibIntercept':94,"
			"'Devices' : {"
				"'IMU' : {'type' : 'MockDevice'},"
				"'PSU' : {'type' : 'MockDevice'}"
			"} }";
	vehicle::IVehicle* veh = 
		new vehicle::Vehicle(core::ConfigNode::fromString(config));
	
	CHECK_EQUAL("IMU", veh->getDevice("IMU")->getName());
	CHECK_EQUAL("PSU", veh->getDevice("PSU")->getName());
}

TEST_FIXTURE(VehicleFixture, IMU)
{
	MockIMU* imu = new MockIMU("IMU");
    veh->_addDevice(vehicle::device::IDevicePtr(imu));
    
    math::Vector3 accel(1,2,3);
    math::Vector3 angularRate(4,5,6);
    math::Quaternion orientation(7,8,9,10);
    
    imu->linearAcceleration = accel;
    imu->angularRate = angularRate;
    imu->orientation = orientation;
    
    CHECK_EQUAL(accel, veh->getLinearAcceleration());
    CHECK_EQUAL(angularRate, veh->getAngularRate());
    CHECK_EQUAL(orientation, veh->getOrientation());
}

TEST_FIXTURE(VehicleFixture, _addDevice)
{
    boost::shared_ptr<MockDevice> mockDevice(new MockDevice("TestName"));
    veh->_addDevice(mockDevice);

    CHECK_EQUAL(mockDevice.get(), veh->getDevice("TestName").get());
    CHECK_EQUAL("TestName", veh->getDevice("TestName")->getName());
}
