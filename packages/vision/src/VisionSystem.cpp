/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/VisionSystem.cpp
 */

// Project Includes
#include "vision/include/VisionSystem.h"

namespace ram {
namespace vision {

VisionSystem::VisionSystem(std::string name, core::EventHubPtr eventHub) :
    Subsystem(name, eventHub),vr()
{

}

VisionSystem::VisionSystem(std::string name, core::SubsystemList list) :
    Subsystem(name, list),vr()
{

}

void VisionSystem::redLightDetectorOn()
{
    vr.forward.lightDetectOn();
}

void VisionSystem::redLightDetectorOff()
{
    vr.forward.lightDetectOff();
}

} // namespace vision
} // namespace ram
