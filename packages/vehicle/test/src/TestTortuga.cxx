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
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IDevice.h"
#include "core/include/ConfigNode.h"


using namespace ram;

static const std::string CONFIG("{'depthCalibSlope' : 33.01,"
                                " 'depthCalibIntercept' : 94}");

class MockDevice : public vehicle::device::IDevice,
                   public vehicle::device::Device
{
public:
    MockDevice(std::string name) : Device(name) {}

    virtual std::string getName() {
        return vehicle::device::Device::getName();
    }

    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

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
