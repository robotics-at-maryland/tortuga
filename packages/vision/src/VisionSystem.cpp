/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/VisionSystem.cpp
 */

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/VisionSystem.h"
#include "vision/include/VisionRunner.h"
#include "vision/include/DC1394Camera.h"
#include "vision/include/FileRecorder.h"
#include "vision/include/NetworkRecorder.h"
#include "vision/include/FFMPEGRecorder.h"
#include "vision/include/FFMPEGNetworkRecorder.h"

#include "vision/include/RedLightDetector.h"
#include "vision/include/BinDetector.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/DuctDetector.h"
#include "vision/include/TargetDetector.h"
#include "vision/include/BarbedWireDetector.h"
#include "vision/include/SafeDetector.h"
#include "vision/include/GateDetector.h"

#include "core/include/EventHub.h"
#include "core/include/SubsystemMaker.h"
#include "core/include/Logging.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::vision::VisionSystem, VisionSystem);

namespace ram {
namespace vision {
    
VisionSystem::VisionSystem(core::ConfigNode config,
                           core::SubsystemList deps) :
    Subsystem(config["name"].asString("VisionSystem"), deps),
    m_forwardCamera(CameraPtr()),
    m_downwardCamera(CameraPtr()),
    m_forward(0),
    m_downward(0),
    m_redLightDetector(DetectorPtr()),
    m_binDetector(DetectorPtr()),
    m_pipelineDetector(DetectorPtr()),
    m_gateDetector(DetectorPtr()),
    m_targetDetector(DetectorPtr()),
    m_barbedWireDetector(DetectorPtr())
{
    init(config, core::Subsystem::getSubsystemOfType<core::EventHub>(deps));
}

VisionSystem::VisionSystem(CameraPtr forward, CameraPtr downward,
                           core::ConfigNode config, core::SubsystemList deps) :
    Subsystem("VisionSystem", deps),
    m_forwardCamera(forward),
    m_downwardCamera(downward),
    m_forward(0),
    m_downward(0),
    m_redLightDetector(DetectorPtr()),
    m_binDetector(DetectorPtr()),
    m_pipelineDetector(DetectorPtr()),
    m_ductDetector(DetectorPtr()),
    m_downwardSafeDetector(DetectorPtr()),
    m_gateDetector(DetectorPtr()),
    m_targetDetector(DetectorPtr())
{
    init(config, core::Subsystem::getSubsystemOfType<core::EventHub>(deps));
}
    
void VisionSystem::init(core::ConfigNode config, core::EventHubPtr eventHub)
{
    if (!m_forwardCamera)
        m_forwardCamera = CameraPtr(new DC1394Camera(config["ForwardCamera"],
                                                     (size_t)1));

    if (!m_downwardCamera)
        m_downwardCamera = CameraPtr(new DC1394Camera(config["DownwardCamera"],
                                                      (size_t)0));

    // Read int as bool
    m_testing = config["testing"].asInt(0) != 0;

    // Recorders
    if (config.exists("ForwardRecorders"))
        createRecorders(config["ForwardRecorders"], m_forwardCamera);
    if (config.exists("DownwardRecorders"))
        createRecorders(config["DownwardRecorders"], m_downwardCamera);
    
    // Detector runners (go as fast as possible)
    m_forward = new VisionRunner(m_forwardCamera.get(), Recorder::NEXT_FRAME);
    m_downward = new VisionRunner(m_downwardCamera.get(), Recorder::NEXT_FRAME);

    // Detectors
    m_redLightDetector = DetectorPtr(
        new RedLightDetector(config["RedLightDetector"], eventHub));
    m_binDetector = DetectorPtr(
        new BinDetector(config["BinDetector"], eventHub));
    m_pipelineDetector = DetectorPtr(
        new OrangePipeDetector(config["PipelineDetector"], eventHub));
    m_ductDetector = DetectorPtr(
        new DuctDetector(config["DuctDetector"], eventHub));
    m_downwardSafeDetector = DetectorPtr(
        new SafeDetector(config["SafeDetector"], eventHub));
    m_gateDetector = DetectorPtr(
        new GateDetector(config["GateDetector"], eventHub));
    m_targetDetector = DetectorPtr(
        new TargetDetector(config["TargetDetector"], eventHub));
    m_barbedWireDetector = DetectorPtr(
        new BarbedWireDetector(config["BarbedWireDetector"], eventHub));
    
    // Start camera in the background (at the fastest rate possible)
    m_forwardCamera->background(-1);
    m_downwardCamera->background(-1);
}
    
void VisionSystem::createRecorders(core::ConfigNode recorderCfg,
                                   CameraPtr camera)
{
    BOOST_FOREACH(std::string recorderString, recorderCfg.subNodes())
    {
        Recorder::RecordingPolicy policy = Recorder::MAX_RATE;

        // Get in the rate, or the signal to record every frame
        int policyArg = recorderCfg[recorderString].asInt();
        if (policyArg <= 0)
            policy = Recorder::NEXT_FRAME;

        // Create the actual recorder
        std::string message;
        Recorder* recorder = Recorder::createRecorderFromString(
            recorderString, camera.get(), message, policy, policyArg,
            core::Logging::getLogDir().string());
        std::cout << "RECORDING>>>> "  << message << std::endl;

        // Store it for later desctruction
        m_recorders.push_back(recorder);
    }
}
    
VisionSystem::~VisionSystem()
{
    m_forward->removeAllDetectors();
    m_downward->removeAllDetectors();
    
    // Stop camera capturing
    m_forwardCamera->unbackground(true);
    m_downwardCamera->unbackground(true);

    // Stop recorders
    BOOST_FOREACH(Recorder* recorder, m_recorders)
        delete recorder;
    
    // Shutdown our detectors running on our cameras
    delete m_forward;
    delete m_downward;
}

void VisionSystem::binDetectorOn()
{
    m_downward->addDetector(m_binDetector);
}

void VisionSystem::binDetectorOff()
{
    m_downward->removeDetector(m_binDetector);
}

void VisionSystem::pipeLineDetectorOn()
{
    m_downward->addDetector(m_pipelineDetector);
}

void VisionSystem::pipeLineDetectorOff()
{
    m_downward->removeDetector(m_pipelineDetector);
}

void VisionSystem::ductDetectorOn()
{
    m_forward->addDetector(m_ductDetector);
}

void VisionSystem::ductDetectorOff()
{
    m_forward->removeDetector(m_ductDetector);
}

void VisionSystem::downwardSafeDetectorOn()
{
    m_downward->addDetector(m_downwardSafeDetector);
}
    
void VisionSystem::downwardSafeDetectorOff()
{
    m_downward->removeDetector(m_downwardSafeDetector);
}
    
void VisionSystem::gateDetectorOn()
{
    m_forward->addDetector(m_gateDetector);
}

void VisionSystem::gateDetectorOff()
{
    m_forward->removeDetector(m_gateDetector);
}

void VisionSystem::redLightDetectorOn()
{
    m_forward->addDetector(m_redLightDetector);
}

void VisionSystem::redLightDetectorOff()
{
    m_forward->removeDetector(m_redLightDetector);
}

void VisionSystem::targetDetectorOn()
{
    m_forward->addDetector(m_targetDetector);
}

void VisionSystem::targetDetectorOff()
{
    m_forward->removeDetector(m_targetDetector);
}

void VisionSystem::barbedWireDetectorOn()
{
    m_forward->addDetector(m_barbedWireDetector);
}

void VisionSystem::barbedWireDetectorOff()
{
    m_forward->removeDetector(m_barbedWireDetector);
}
   
void VisionSystem::setPriority(core::IUpdatable::Priority priority)
{
//    assert(m_testing && "Can't background when not testing");
    m_forwardCamera->setPriority(priority);
    m_downwardCamera->setPriority(priority);

    m_forward->setPriority(priority);
    m_downward->setPriority(priority);
    
    BOOST_FOREACH(Recorder* recorder, m_recorders)
        recorder->setPriority(priority);
}
    
void VisionSystem::background(int interval)
{
    assert(m_testing && "Can't background when not testing");
    // Start cameras
    m_forwardCamera->background(interval);
    m_downwardCamera->background(interval);

    // Start detectors
    m_forward->background(interval);
    m_downward->background(interval);

    // Start recorders
    BOOST_FOREACH(Recorder* recorder, m_recorders)
        recorder->background(interval);
}
        
void VisionSystem::unbackground(bool join)
{
    //assert(m_testing && "Can't unbackground when not testing");
    
    // Stop cameras
    m_forwardCamera->unbackground(join);
    m_downwardCamera->unbackground(join);

    // Stop detectors
    m_forward->unbackground(join);
    m_downward->unbackground(join);

    // Stop recorders
    BOOST_FOREACH(Recorder* recorder, m_recorders)
        recorder->unbackground(join);
}

void VisionSystem::update(double timestep)
{
    assert(m_testing && "Can't update when not testing");
    
    // Grab data from the cameras
    m_forwardCamera->update(timestep);
    m_downwardCamera->update(timestep);

    // Process in the detectors
    m_forward->update(timestep);
    m_downward->update(timestep);

    // Update the recorders to record the data
    BOOST_FOREACH(Recorder* recorder, m_recorders)
        recorder->update(timestep);
}
    
} // namespace vision
} // namespace ram
