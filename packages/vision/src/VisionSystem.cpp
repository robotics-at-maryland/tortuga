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
#include "vision/include/VisionRunner.h"

//To register it as a subsystem.
#include "core/include/SubsystemMaker.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::vision::VisionSystem, VisionSystem);

namespace ram {
namespace vision {

VisionSystem::VisionSystem(std::string name, core::EventHubPtr eventHub) :
    Subsystem(name, eventHub),
    vr(new VisionRunner())
{

}

VisionSystem::VisionSystem(std::string name, core::SubsystemList list) :
    Subsystem(name, list),
    vr(new VisionRunner())
{

}

VisionSystem::VisionSystem(core::ConfigNode config, core::SubsystemList deps) : 
    Subsystem(config["date"].asString("VisionSystem"), deps),
    vr(new VisionRunner())
{
}


VisionSystem::~VisionSystem()
{
    delete vr;
}

void VisionSystem::redLightDetectorOn()
{
    vr->forward.lightDetectOn();
}

void VisionSystem::redLightDetectorOff()
{
    vr->forward.lightDetectOff();
}

} // namespace vision
} // namespace ram
