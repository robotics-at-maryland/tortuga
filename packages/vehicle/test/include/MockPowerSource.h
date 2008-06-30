/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockPowerSource.h
 */

#ifndef RAM_VEHICLE_MOCKPOWERSOURCE_11_18_2007
#define RAM_VEHICLE_MOCKPOWERSOURCE_11_18_2007

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IPowerSource.h"

class MockPowerSource : public ram::vehicle::device::IPowerSource,
                        public ram::vehicle::device::Device
{
public:
    MockPowerSource(std::string name) :
        IPowerSource(ram::core::EventHubPtr()),
        Device(name),
        voltage(0.0),
        current(0.0),
        enabled(false),
        used(false)
        {}

    virtual std::string getName() {
        return ram::vehicle::device::Device::getName();
    }

    virtual double getVoltage() { return voltage; }

    virtual double getCurrent() { return current; }

    virtual bool isEnabled() { return enabled; }

    virtual bool inUse() { return used; }

    //virtual void setEnabled(bool state) { enabled = state; }
    
    double voltage;
    double current;
    bool enabled;
    bool used;

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

#endif // RAM_VEHICLE_MOCKPOWERSOURCE_11_18_2007
