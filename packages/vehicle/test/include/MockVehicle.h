/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/MockVehicle.h
 */

#ifndef RAM_VEHICLE_MOCKVEHICLE_10_28_2007
#define RAM_VEHICLE_MOCKVEHICLE_10_28_2007

// STD Includes
#include <cassert>

// Project Includes
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/device/IDevice.h"

class MockVehicle : public ram::vehicle::IVehicle
{
public:
    MockVehicle() :
        depth(0),
        linearAcceleration(0, 0, 0),
        angularRate(0, 0, 0),
        orientation(0,0,0,1),
        force(0,0,0),
        torque(0,0,0)
    {};
                
//    virtual ~MockVehicle {};
    
    virtual ram::vehicle::device::IDevice* getDevice(std::string name)
    {
        ram::vehicle::NameDeviceMapIter iter = devices.find(name);
        assert(iter != devices.end() && "Error Device not found");
        return (*iter).second.get();
    }

    virtual double getDepth()  { return depth; }

    virtual std::vector<std::string> getTemperatureNames()
    {
        assert(false && "Method not implemented");
        return std::vector<std::string>();
    }

    virtual std::vector<int> getTemperatures()
    {
        assert(false && "Method not implemented");
        return std::vector<int>();
    }

    virtual ram::math::Vector3 getLinearAcceleration() { return linearAcceleration;}
     
    virtual ram::math::Vector3 getAngularRate() { return angularRate; }
    
    virtual ram::math::Quaternion getOrientation() { return orientation; }
    
    virtual void safeThrusters() { assert(false && "Method not implemented"); }

    virtual void unsafeThrusters() {assert(false && "Method not implemented");}
    
    virtual void dropMarker() {assert(false && "Method not implemented");}

    virtual int startStatus()
    {
        assert(false && "Method not implemented");
        return 0;
    }

    virtual void printLine(int, std::string)
    {
        assert(false && "Method not implemented");
    }

    virtual void applyForcesAndTorques(const ram::math::Vector3& force_,
                                       const ram::math::Vector3& torque_)
    {
        force = force_;
        torque = torque_;
    }

    virtual void update(double) {};
    virtual void background(int) {};
    virtual void unbackground(bool) {};
    virtual bool backgrounded() { return false; }

    double depth;
    ram::math::Vector3 linearAcceleration;
    ram::math::Vector3 angularRate;
    ram::math::Quaternion orientation;
    ram::math::Vector3 force;
    ram::math::Vector3 torque;
    ram::vehicle::NameDeviceMap devices;
};
    
#endif // RAM_VEHICLE_MOCKVEHICLE_10_28_2007
