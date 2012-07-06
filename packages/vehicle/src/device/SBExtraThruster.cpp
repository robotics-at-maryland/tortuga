/*
 * Copyright (C) 2012 Robotics at Maryland
 * Copyright (C) 2012 Gary Sullivan <gsulliva@umd.edu>
 * All rights reserved.
 *
 * Author: Gary Sullivan <gsulliva@umd.edu>
 * File:  packages/vehicle/src/device/SBThruster.cpp
 */

// Library Includes
#include <boost/bind.hpp>

// Project Includes
#include "vehicle/include/Vehicle.h"
#include "vehicle/include/Events.h"
#include "vehicle/include/device/SBExtraThruster.h"
#include "vehicle/include/device/SensorBoard.h"

#include "math/include/Events.h"

namespace ram {
namespace vehicle {
namespace device {
    
SBExtraThruster::SBExtraThruster(core::ConfigNode config, core::EventHubPtr eventHub,
                   IVehiclePtr vehicle) :
    Device(config["name"].asString()),
    IThruster(eventHub, config["name"].asString()),
    m_sensorBoard(SensorBoardPtr())
{
    // A little hack to determine the offset based on thruster type
    std::string name(getName());

    m_sensorBoard = IDevice::castTo<SensorBoard>(
        vehicle->getDevice("SensorBoard"));

}

SBExtraThruster::~SBExtraThruster()
{
    // Zero forces
    setForce(0);
}

void SBExtraThruster::setForce(double force)
{
    

    {
        core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
        m_force = force;
    }

    // Send the computed value to the hardware
    m_sensorBoard->setExtraThrusterSpeed(force);
}

double SBExtraThruster::getForce()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_force;
}

void SBExtraThruster::update(double)
{
}
    
void SBExtraThruster::background(int)
{
}

void SBExtraThruster::unbackground(bool join)
{
}

bool SBExtraThruster::backgrounded()
{
    return false;
}

    
} // namespace device
} // namespace vehicle
} // namespace ram
