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
#include "vehicle/include/Vehicle.h"
#include "core/include/ConfigNode.h"
#include "vehicle/test/include/MockDevice.h"

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

TEST_FIXTURE(VehicleFixture, _addDevice)
{
    boost::shared_ptr<MockDevice> mockDevice(new MockDevice("TestName"));
    veh->_addDevice(mockDevice);

    CHECK_EQUAL(mockDevice.get(), veh->getDevice("TestName").get());
    CHECK_EQUAL("TestName", veh->getDevice("TestName")->getName());
}
