/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockTempSensor.h
 */

#ifndef RAM_VEHICLE_MOCKTEMPSENSOR_06_24_2008
#define RAM_VEHICLE_MOCKTEMPSENSOR_06_24_2008

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/ITempSensor.h"

class MockTempSensor : public ram::vehicle::device::ITempSensor,
                        public ram::vehicle::device::Device
{
public:
    MockTempSensor(std::string name) :
        ITempSensor(ram::core::EventHubPtr()),
        Device(name),
        temp(0)
        {}

    virtual std::string getName() {
        return ram::vehicle::device::Device::getName();
    }

    virtual int getTemp() { return temp; }

    int temp;
    
    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

#endif // RAM_VEHICLE_MOCKTEMPSENSOR_06_24_2008
