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
    m_sensorBoard(SensorBoardPtr()),
    m_markersDropped(0)
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
}
    
SBMarkerDropper::~SBMarkerDropper()
{
}

void SBMarkerDropper::dropMarker()
{
    if (-1 != m_sensorBoard->dropMarker())
    {
        m_markersDropped++;
        publish(MARKER_DROPPED, core::EventPtr(new core::Event()));
    }
}

int SBMarkerDropper::markersLeft()
{
    return SensorBoard::NUMBER_OF_MARKERS - m_markersDropped;
}

int SBMarkerDropper::initalMarkerCount()
{
    return SensorBoard::NUMBER_OF_MARKERS;
}

    
} // namespace device
} // namespace vehicle
} // namespace ram
