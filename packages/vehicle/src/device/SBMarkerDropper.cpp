/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/SBMarkerDropper.cpp
 */

// STD Includes
#include <cassert>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/device/SBMarkerDropper.h"

namespace ram {
namespace vehicle {
namespace device {
    
SBMarkerDropper::SBMarkerDropper(core::ConfigNode config,
                                 core::EventHubPtr eventHub,
                                 IVehiclePtr vehicle) :
    Device(config["name"].asString("MarkerDropper")),
    IMarkerDropper(eventHub),
    m_sensorBoard(SensorBoardPtr())
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
}
    
SBMarkerDropper::~SBMarkerDropper()
{
}

void SBMarkerDropper::dropMarker()
{
    m_sensorBoard->dropMarker();
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
