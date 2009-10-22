/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/slam/src/ISlam.cpp
 */

// Project Includes
#include "slam/include/ISlam.h"

RAM_CORE_EVENT_TYPE(ram::slam::ISlam, MAP_UPDATE);

namespace ram {
namespace slam {

ISlam::ISlam(std::string name, core::EventHubPtr eventHub) :
    core::Subsystem(name, eventHub)
{
}

} // namespace slam
} // namespace ram
