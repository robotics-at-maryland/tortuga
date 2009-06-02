/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IPowerBus.cpp
 */

// Project Includes
#include "vehicle/include/device/IPowerBus.h"

namespace ram {
namespace vehicle {
namespace device {

IPowerBus::IPowerBus(core::EventHubPtr eventHub, std::string name) :
    IDevice(eventHub, name)
{
}

IPowerBus::~IPowerBus()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
