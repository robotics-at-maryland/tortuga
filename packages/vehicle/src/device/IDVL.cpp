/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vehicle/src/device/IDVL.cpp
 */

// Project Includes
#include "vehicle/include/device/IDVL.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IDVL, UPDATE);

namespace ram {
namespace vehicle {
namespace device {

IDVL::IDVL(core::EventHubPtr eventHub, std::string name) :
    IStateEstimatorDevice(eventHub, name)
{
}

IDVL::~IDVL()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
