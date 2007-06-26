/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Vehicle.h
 */

#ifndef RAM_VEHICLE_VEHICLE_06_11_2007
#define RAM_VEHICLE_VEHICLE_06_11_2007

// Project Includes
#include "core/include/ReadWriteMutex.h"

#include "vehicle/include/Common.h"
#include "vehicle/include/IVehicle.h"

namespace ram {
namespace vehicle {

class Vehicle : public IVehicle
{
public:
    Vehicle();
    
    virtual ~Vehicle() {}
    
    /** Gets the device associated with the given name */
    virtual device::IDevice* getDevice(std::string name);
    
    virtual math::Vector3 getLinearAcceleration();

    virtual math::Vector3 getAngularRate();

    virtual math::Quaternion getOrientation();

    virtual double getDepth();

    virtual double getVoltage();

    /** This is <b>NOT</b> thread safe */
    virtual void _addDevice(std::string name, device::IDevice* device);

private:
    struct VehicleState
    {
        math::Vector3 linear_acceleration;
        math::Vector3 angular_rate;
        math::Quaternion orientation;
        double depth;
        double voltage;
    };
    
protected:
    void getState(Vehicle::VehicleState* state);
    void setState(Vehicle::VehicleState* state);
    
private:
    core::ReadWriteMutex m_state_mutex;
    VehicleState m_state;
    
    NameDeviceMap m_devices;
};
    
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_VEHICLE_06_11_2007
