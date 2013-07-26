/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IDevice.cpp
 */

// Project Includes
#include "vehicle/include/device/IDevice.h"

namespace ram {
namespace vehicle {
namespace device {

IDevice::IDevice(core::EventHubPtr eventHub, std::string name) :
    core::EventPublisher(eventHub, "Vehicle.Device." + name)
{
    corflg = false;
}

IDevice::~IDevice()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
