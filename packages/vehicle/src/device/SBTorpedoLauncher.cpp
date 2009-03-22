/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/SBTorpedoLauncher.cpp
 */

// STD Includes
#include <cassert>

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/IVehicle.h"
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/device/SBTorpedoLauncher.h"

namespace ram {
namespace vehicle {
namespace device {
    
SBTorpedoLauncher::SBTorpedoLauncher(core::ConfigNode config,
                                 core::EventHubPtr eventHub,
                                 IVehiclePtr vehicle) :
    Device(config["name"].asString("TorpedoLauncher")),
    IPayloadSet(eventHub),
    m_sensorBoard(SensorBoardPtr()),
    m_torpedosFired(0)
{
    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));
}
    
SBTorpedoLauncher::~SBTorpedoLauncher()
{
}

void SBTorpedoLauncher::releaseObject()
{
    if (-1 != m_sensorBoard->fireTorpedo())
    {
        m_torpedosFired++;
        publish(OBJECT_RELEASED, core::EventPtr(new core::Event()));
    }
}

int SBTorpedoLauncher::objectCount()
{
    return SensorBoard::NUMBER_OF_TORPEDOS - m_torpedosFired;
}

int SBTorpedoLauncher::initialObjectCount()
{
    return SensorBoard::NUMBER_OF_TORPEDOS;
}

    
} // namespace device
} // namespace vehicle
} // namespace ram
