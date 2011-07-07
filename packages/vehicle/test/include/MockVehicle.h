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

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/IVehicle.h"
#include "math/include/Vector2.h"
#include "math/include/Quaternion.h"

// Must Be Included last
//#include "vehicle/include/Export.h"

class MockVehicle : public ram::vehicle::IVehicle
{
public:
    MockVehicle() :
        IVehicle("MockVehicle"),
        force(0,0,0),
        torque(0,0,0)
    {}
                
//    virtual ~MockVehicle {};
    
    virtual ram::vehicle::device::IDevicePtr getDevice(std::string name)
    {
        ram::vehicle::NameDeviceMapIter iter = devices.find(name);
        assert(iter != devices.end() && "Error Device not found");
        return (*iter).second;
    }

    std::vector<std::string> getDeviceNames()
    {
        std::vector<std::string> names;
        BOOST_FOREACH(ram::vehicle::NameDeviceMap::value_type pair,
                      devices)
        {
            names.push_back(pair.first);
        }
        
        return names;
    }

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

    virtual bool hasObject(std::string obj)
        { return obj == "vehicle"; }
    
    virtual void safeThrusters() { assert(false && "Method not implemented"); }

    virtual void unsafeThrusters() {assert(false && "Method not implemented");}
    
    virtual void dropMarker() {assert(false && "Method not implemented");}

    virtual void fireTorpedo() {assert(false && "Method not implemented");}

    virtual void dropMarkerIndex(int index) {assert(false && "Method not implemented");}

    virtual void fireTorpedoIndex(int index) {assert(false && "Method not implemented");}

    virtual void releaseGrabber() {assert(false && "Method not implemented");}

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

    virtual void setPriority(ram::core::IUpdatable::Priority) {};
    virtual ram::core::IUpdatable::Priority getPriority() {
        return ram::core::IUpdatable::NORMAL_PRIORITY;
    };
    virtual void setAffinity(size_t) {};
    virtual int getAffinity() {
        return -1;
    };
    virtual void update(double) {};
    virtual void background(int) {};
    virtual void unbackground(bool) {};
    virtual bool backgrounded() { return false; }

    ram::math::Vector3 force;
    ram::math::Vector3 torque;
    ram::vehicle::NameDeviceMap devices;
};
    
#endif // RAM_VEHICLE_MOCKVEHICLE_10_28_2007
