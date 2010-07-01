/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/device/SBGrabber.cpp
 */

// STD Includes
#include <cassert>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/device/SBGrabber.h"

namespace ram {
namespace vehicle {
namespace device {
    
SBGrabber::SBGrabber(core::ConfigNode config,
                     core::EventHubPtr eventHub,
                     IVehiclePtr vehicle) :
    Device(config["name"].asString("Grabber")),
    IPayloadSet(eventHub, config["name"].asString("Grabber")),
    m_sensorBoard(SensorBoardPtr()),
    m_released(0)
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
}
    
SBGrabber::~SBGrabber()
{
}

void SBGrabber::releaseObject()
{
    if (-1 != m_sensorBoard->releaseGrabber())
    {
        m_released = 1;
        publish(OBJECT_RELEASED, core::EventPtr(new core::Event()));
    }
}

int SBGrabber::objectCount()
{
    // 0 = not released, -1 = released
    // not released = -1, released = 0
    return !m_released*-1;
}

int SBGrabber::initialObjectCount()
{
    return 1;
}

    
} // namespace device
} // namespace vehicle
} // namespace ram
