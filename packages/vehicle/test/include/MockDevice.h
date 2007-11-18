/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockDevice.h
 */

#ifndef RAM_VEHICLE_MOCKDEVICE_10_29_2007
#define RAM_VEHICLE_MOCKDEVICE_10_29_2007

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IDevice.h"
#include "core/include/ConfigNode.h"

class MockDevice : public ram::vehicle::device::IDevice,
                   public ram::vehicle::device::Device
{
public:
    MockDevice(std::string name) : Device(name) {}
    MockDevice(ram::core::ConfigNode config) : Device(config["name"].asString()) {}

    virtual std::string getName() {
        return ram::vehicle::device::Device::getName();
    }

    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

#endif // RAM_VEHICLE_MOCKDEVICE_10_29_2007
