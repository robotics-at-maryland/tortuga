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
//    virtual ~MockVehicle {};
/*    
    virtual ram::device::IDevicePtr getDevice(std::string name)
    {
        assert(false && "Method not implemented");
        return ram::device::IDevicePtr();
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
        return std::vector<std::string>();
    }

    virtual math::Vector3 getLinearAcceleration() { return linearAcceleration;}
     
    virtual math::Vector3 getAngularRate() { return angularRate; }
    
    virtual math::Quaternion getOrientation() { return orientation; }
    
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

    virtual void applyForcesAndTorques(math::Vector3& force_,
                                       math::Vector3& torque_)
    {
        force = force_;
        torque = torque_;
    }
*/
    virtual void update(double) {};
    virtual void background(int) {};
    virtual void unbackground(bool) {};
    virtual bool backgrounded() { return false; }
/*
    double depth;
    math::Vector3 linearAcceleration;
    math::Vector3 angularRate;
    math::Quaternion orientation;
    math::Vector3 force;
    math::Vector3 torque;*/
};
    
#endif // RAM_VEHICLE_MOCKVEHICLE_10_28_2007
