/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IPSU.cpp
 */

// Project Includes
#include "vehicle/include/device/IPSU.h"

namespace ram {
namespace vehicle {
namespace device {

IPSU::IPSU(core::EventHubPtr eventHub, std::string name) :
    IDevice(eventHub, name)
{
}

IPSU::~IPSU()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
