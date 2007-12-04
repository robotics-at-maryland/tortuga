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
class DeviceA : public ram::vehicle::device::IDevice
{
public:
    virtual ~DeviceA() {};
    virtual std::string getName() { return "DeviceA"; }
    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

class DeviceB : public ram::vehicle::device::IDevice
{
public:
    virtual ~DeviceB() {};
    virtual std::string getName() { return "DeviceB"; }
    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
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

int main()
{
    return UnitTest::RunAllTests();
}
