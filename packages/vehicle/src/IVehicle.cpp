/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Vehicle.cpp
 */

// Project Includes
#include "vehicle/include/IVehicle.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, ORIENTATION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, LINEAR_ACCEL_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, ANGULAR_RATE_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::IVehicle, DEPTH_UPDATE);

namespace ram {
namespace vehicle {
    
IVehicle::IVehicle(std::string name, core::EventHubPtr eventHub) :
    core::Subsystem(name, eventHub)
{
}

device::IDevicePtr IVehicle::getDevice(std::string name)
{
    return device::IDevicePtr();
}
    
    
IVehicle::~IVehicle()
{    
}

} // namespace vehicle
} // namespace ram
