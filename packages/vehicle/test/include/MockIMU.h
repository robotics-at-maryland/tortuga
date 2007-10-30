/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/packages/vehicle/test/include/MockDevice.cxx
 */

#ifndef RAM_VEHICLE_MOCKIMU_10_29_2007
#define RAM_VEHICLE_MOCKIMU_10_29_2007

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IIMU.h"

class MockIMU : public ram::vehicle::device::IIMU,
                public ram::vehicle::device::Device                
{
public:
	MockIMU(std::string name) : Device(name) {}
	
    virtual ram::math::Vector3 getLinearAcceleration() 
    	{ return linearAcceleration; }

    virtual ram::math::Vector3 getAngularRate()
    	{ return angularRate; }
    
    virtual ram::math::Quaternion getOrientation()
     	{ return orientation; }
    
    ram::math::Vector3 linearAcceleration;
    ram::math::Vector3 angularRate;
    ram::math::Quaternion orientation;
    
    virtual std::string getName() {
        return ram::vehicle::device::Device::getName();
    }

    virtual void update(double) {}
    virtual void background(int) {}
    virtual void unbackground(bool) {}
    virtual bool backgrounded() { return false; }
};

#endif // RAM_VEHICLE_MOCKIMU_10_29_2007
