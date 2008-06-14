/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockDepthSensor.h
 */

#ifndef RAM_VEHICLE_MOCKDEPTHSENSOR_06_13_2008
#define RAM_VEHICLE_MOCKDEPTHSENSOR_06_13_2008

// Project Includes
#include "vehicle/include/device/IDepthSensor.h"
#include "vehicle/include/device/Device.h"

class MockDepthSensor : public ram::vehicle::device::IDepthSensor,
                        public ram::vehicle::device::Device
{
public:
    MockDepthSensor(std::string name) :
        Device(name),
        depth(0.0)
        {}

    virtual double getDepth() { return depth; }
    
    double depth;
    
    virtual std::string getName() {
        return ram::vehicle::device::Device::getName();
    }

    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

#endif // RAM_VEHICLE_MOCKDEPTHSENSOR_06_13_2008
