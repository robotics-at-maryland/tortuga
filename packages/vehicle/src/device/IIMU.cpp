/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IIMU.cpp
 */

// Project Includes
#include "vehicle/include/device/IIMU.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IIMU, UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IIMU, RAW_UPDATE);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IIMU, INIT);

namespace ram {
namespace vehicle {
namespace device {

IIMU::IIMU(core::EventHubPtr eventHub, std::string name) :
    IDevice(eventHub, name)
{
}

IIMU::~IIMU()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
