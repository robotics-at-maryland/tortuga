/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IPowerSource.cpp
 */

// Project Includes
#include "vehicle/include/device/IPowerSource.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IPowerSource, ENABLED);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IPowerSource, DISABLED);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IPowerSource, USING);
RAM_CORE_EVENT_TYPE(ram::vehicle::device::IPowerSource, NOT_USING);

namespace ram {
namespace vehicle {
namespace device {

IPowerSource::IPowerSource(core::EventHubPtr eventHub) :
    IDevice(eventHub)
{
}

IPowerSource::~IPowerSource()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
