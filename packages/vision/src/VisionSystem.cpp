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
#include "vision/include/OpenCVCamera.h"
#include "vision/include/RedLightDetector.h"

//To register it as a subsystem.
#include "core/include/SubsystemMaker.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::vision::VisionSystem, VisionSystem);

namespace ram {
namespace vision {

VisionSystem::VisionSystem(core::ConfigNode config,
                           core::SubsystemList deps) :
    Subsystem(config["date"].asString("VisionSystem"), deps),
    m_forwardCamera(0),
    m_downwardCamera(0),
    m_forward(0),
    m_downward(0),
    m_redLightDetector(DetectorPtr())
{
    init();
}

VisionSystem::VisionSystem(Camera* forward, Camera* downward,
                           core::SubsystemList list) :
    Subsystem("VisionSystem", list),
    m_forwardCamera(forward),
    m_downwardCamera(downward),
    m_forward(0),
    m_downward(0),
    m_redLightDetector(DetectorPtr())
{
    init();
}
    
void VisionSystem::init()
{
    if (!m_forwardCamera)
        m_forwardCamera = new OpenCVCamera(0, true);

    if (!m_downwardCamera)
        m_downwardCamera = new OpenCVCamera(1, false);

    m_forward = new VisionRunner(m_forwardCamera);
    m_downward = new VisionRunner(m_downwardCamera);
    m_redLightDetector = DetectorPtr(new RedLightDetector(0));

    // Start camera in the background (at the fastest rate possible)
    m_forwardCamera->background(-1);
    m_downwardCamera->background(-1);
}
    
VisionSystem::~VisionSystem()
{
    m_forward->removeAllDetectors();
    m_downward->removeAllDetectors();
    
    // Stop camera capturing
    m_forwardCamera->unbackground(true);
    m_downwardCamera->unbackground(true);

    // Shutdown our detectors running on our cameras
    delete m_forward;
    delete m_downward;

    delete m_forwardCamera;
    delete m_downwardCamera;
}

void VisionSystem::redLightDetectorOn()
{
    m_forward->addDetector(m_redLightDetector);
}

void VisionSystem::redLightDetectorOff()
{
    m_forward->removeDetector(m_redLightDetector);
}

} // namespace vision
} // namespace ram
