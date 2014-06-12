/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/device/PosVelStateEstimator.h
 */

// Project Includes
#include "vehicle/include/device/PosVelStateEstimator.h"

namespace ram {
namespace vehicle {
namespace device {

PosVelStateEstimator::PosVelStateEstimator(core::ConfigNode config,
					   core::EventHubPtr eventHub,
					   IVehiclePtr vehicle) :
    LoopStateEstimator(config, eventHub, vehicle)
{
}

int PosVelStateEstimator::velocityUpdate(math::Vector2 velocity,
					  double timeStamp)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_mutex);
    m_velocity = velocity;

    // Find the position based on orientation
    return StateFlag::VEL | StateFlag::POS;
}

int PosVelStateEstimator::positionUpdate(math::Vector2 position,
					  double timeStamp)
{
    // Do nothing, position estimation is done on velocity update

    return 0;
}

} // namespace device
} // namespace vehicle
} // namespace ram
