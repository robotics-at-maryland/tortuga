/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/src/TestnIMU.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/Common.h"

// Test Classes
class TestDevice : public ram::vehicle::device::IDevice
{
public:
    TestDevice() :
        ram::vehicle::device::IDevice(ram::core::EventHubPtr()) {};
    virtual void setPriority(ram::core::IUpdatable::Priority) {};
    virtual ram::core::IUpdatable::Priority getPriority() {
        return ram::core::IUpdatable::NORMAL_PRIORITY;
    };
    virtual void setAffinity(size_t) {};
    virtual int getAffinity() {
        return -1;
    };
    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

class DeviceA : public TestDevice
{
public:
    virtual std::string getName() { return "DeviceA"; }
};

class DeviceB : public TestDevice
{
public:
    virtual std::string getName() { return "DeviceB"; }
};
    
TEST(castTo)
{
    ram::vehicle::device::IDevicePtr deviceA(new DeviceA);
    ram::vehicle::device::IDevicePtr deviceB(new DeviceB);

    boost::shared_ptr<DeviceA> ptrA =
        ram::vehicle::device::IDevice::castTo<DeviceA>(deviceA);

    CHECK(ptrA);
    CHECK(ptrA.get());
    // This will assert
    //boost::shared_ptr<DeviceB> ptrB =
    //    ram::vehicle::device::IDevice::castTo<DeviceB>(deviceA);
}
