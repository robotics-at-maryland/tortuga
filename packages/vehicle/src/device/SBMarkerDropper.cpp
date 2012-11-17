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
    IPayloadSet(eventHub, config["name"].asString("MarkerDropper")),
    m_sensorBoard(SensorBoardPtr()),
    m_markersDropped(0)
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
}
    
SBMarkerDropper::~SBMarkerDropper()
{
}

void SBMarkerDropper::releaseObject()
{
    if (-1 != m_sensorBoard->dropMarker())
    {
        m_markersDropped++;
        publish(OBJECT_RELEASED, core::EventPtr(new core::Event()));
    }
}

void SBMarkerDropper::releaseObjectIndex(int index)
{
    m_sensorBoard->dropMarkerIndex(index);
    publish(OBJECT_RELEASED, core::EventPtr(new core::Event()));
}

int SBMarkerDropper::objectCount()
{
    return SensorBoard::NUMBER_OF_MARKERS - m_markersDropped;
}

int SBMarkerDropper::initialObjectCount()
{
    return SensorBoard::NUMBER_OF_MARKERS;
}

    
} // namespace device
} // namespace vehicle
} // namespace ram
