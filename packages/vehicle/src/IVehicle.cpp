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
