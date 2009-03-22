/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IPayloadSet.cpp
 */

// Project Includes
#include "vehicle/include/device/IPayloadSet.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IPayloadSet, OBJECT_RELEASED);

namespace ram {
namespace vehicle {
namespace device {

IPayloadSet::IPayloadSet(core::EventHubPtr eventHub) :
    IDevice(eventHub)
{
}

IPayloadSet::~IPayloadSet()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
