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

namespace ram {
namespace vehicle {
namespace device {

IIMU::IIMU(core::EventHubPtr eventHub, std::string name) :
    IStateEstimatorDevice(eventHub, name)
{
}

IIMU::~IIMU()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
