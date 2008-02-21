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
#include "vision/include/Recorder.h"

#include "core/include/EventHub.h"
//To register it as a subsystem.
#include "core/include/SubsystemMaker.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::vision::VisionSystem, VisionSystem);

namespace ram {
namespace vision {

VisionSystem::VisionSystem(core::ConfigNode config,
                           core::SubsystemList deps) :
    Subsystem(config["name"].asString("VisionSystem"), deps),
    m_forwardCamera(CameraPtr()),
    m_downwardCamera(CameraPtr()),
    m_forwardRecorder(0),
    m_downwardRecorder(0),
    m_forward(0),
    m_downward(0),
    m_redLightDetector(DetectorPtr())
{
    init(config, core::Subsystem::getSubsystemOfType<core::EventHub>(deps));
}

VisionSystem::VisionSystem(CameraPtr forward, CameraPtr downward,
                           core::ConfigNode config, core::SubsystemList deps) :
    Subsystem("VisionSystem", deps),
    m_forwardCamera(forward),
    m_downwardCamera(downward),
    m_forwardRecorder(0),
    m_downwardRecorder(0),
    m_forward(0),
    m_downward(0),
    m_redLightDetector(DetectorPtr())
{
    init(config, core::Subsystem::getSubsystemOfType<core::EventHub>(deps));
}
    
void VisionSystem::init(core::ConfigNode config, core::EventHubPtr eventHub)
{
    if (!m_forwardCamera)
        m_forwardCamera = CameraPtr(new OpenCVCamera(0, true));

    if (!m_downwardCamera)
        m_downwardCamera = CameraPtr(new OpenCVCamera(1, false));

    // Recorders
    if (config.exists("forwardFile"))
    {
        m_forwardRecorder = new Recorder(m_forwardCamera.get(),
                                         Recorder::NEXT_FRAME,
                                         config["forwardFile"].asString());
    }

    if (config.exists("downwardFile"))
    {
        m_downwardRecorder = new Recorder(m_downwardCamera.get(),
                                          Recorder::NEXT_FRAME,
                                          config["downwardFile"].asString());

    }
    
    // Detectors
    m_forward = new VisionRunner(m_forwardCamera.get());
    m_downward = new VisionRunner(m_downwardCamera.get());
    m_redLightDetector = DetectorPtr(
        new RedLightDetector(config["RedLightDetector"], eventHub));

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

    delete m_forwardRecorder;
    delete m_downwardRecorder;
    
    // Shutdown our detectors running on our cameras
    delete m_forward;
    delete m_downward;
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
