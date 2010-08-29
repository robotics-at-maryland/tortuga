/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/src/IStateEstimator.c
 */

// Project Includes
#include "estimation/include/IStateEstimator.h"

RAM_CORE_EVENT_TYPE(ram::estimation::IStateEstimator, ESTIMATED_DEPTH_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimation::IStateEstimator, ESTIMATED_ORIENTATION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimation::IStateEstimator, ESTIMATED_VELOCITY_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimation::IStateEstimator, ESTIMATED_POSITION_UPDATE);

RAM_CORE_EVENT_TYPE(ram::estimation::IStateEstimator, ESTIMATED_LINEARACCELERATION_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimation::IStateEstimator, ESTIMATED_DEPTHDOT_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimation::IStateEstimator, ESTIMATED_ANGULARRATE_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimation::IStateEstimator, ESTIMATED_FORCES_UPDATE);
RAM_CORE_EVENT_TYPE(ram::estimation::IStateEstimator, ESTIMATED_TORQUES_UPDATE);

namespace ram {
namespace estimation {

IStateEstimator::IStateEstimator(std::string name, core::EventHubPtr eventHub) :
    core::Subsystem(name, eventHub)
{
}

IStateEstimator::~IStateEstimator()
{
}

} // namespace vehicle
} // namespace ram
