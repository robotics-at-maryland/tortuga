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

namespace ram {
namespace vehicle {
namespace device {

IIMU::IIMU(core::EventHubPtr eventHub) :
    IDevice(eventHub)
{
}

IIMU::~IIMU()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
