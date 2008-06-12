/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockThruster.h
 */

#ifndef RAM_VEHICLE_MOCKTHRUSTER_11_18_2007
#define RAM_VEHICLE_MOCKTHRUSTER_11_18_2007

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IThruster.h"

class MockThruster : public ram::vehicle::device::IThruster,
                     public ram::vehicle::device::Device
{
public:
    MockThruster(std::string name) :
        IThruster(ram::core::EventHubPtr()),
        Device(name),
        force(0.0),
        offset(0.0),
        enabled(false)
        {}

    virtual std::string getName() {
        return ram::vehicle::device::Device::getName();
    }

    virtual void setForce(double newtons) { force = newtons; }

    virtual double getForce() { return force; }

    virtual double getMaxForce() { return 10; }

    virtual double getMinForce() { return -10; }

    virtual bool isEnabled() { return enabled; }

    virtual void setEnabled(bool state) { enabled = state; }

    virtual double getOffset() { return offset; }
    
    double force;
    double offset;
    bool enabled;
    
    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

#endif // RAM_VEHICLE_MOCKTHRUSTER_11_18_2007
