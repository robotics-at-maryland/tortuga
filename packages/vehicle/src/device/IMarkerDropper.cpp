/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IMarkerDropper.cpp
 */

// Project Includes
#include "vehicle/include/device/IMarkerDropper.h"

RAM_CORE_EVENT_TYPE(ram::vehicle::device::IMarkerDropper, MARKER_DROPPED);

namespace ram {
namespace vehicle {
namespace device {

IMarkerDropper::IMarkerDropper(core::EventHubPtr eventHub) :
    IDevice(eventHub)
{
}

IMarkerDropper::~IMarkerDropper()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
