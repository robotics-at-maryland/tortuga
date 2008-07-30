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
#include "vision/include/FileRecorder.h"
#include "vision/include/NetworkRecorder.h"
#include "vision/include/FFMPEGRecorder.h"
#include "vision/include/FFMPEGNetworkRecorder.h"

#include "vision/include/RedLightDetector.h"
#include "vision/include/BinDetector.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/DuctDetector.h"
#include "vision/include/SafeDetector.h"
#include "vision/include/GateDetector.h"

#include "core/include/EventHub.h"
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
    m_forwardFileRecorder(0),
    m_downwardFileRecorder(0),
    m_forwardNetworkRecorder(0),
    m_downwardNetworkRecorder(0),
    m_forward(0),
    m_downward(0),
    m_redLightDetector(DetectorPtr()),
    m_binDetector(DetectorPtr()),
    m_pipelineDetector(DetectorPtr()),
    m_gateDetector(DetectorPtr())
{
    init(config, core::Subsystem::getSubsystemOfType<core::EventHub>(deps));
}

VisionSystem::VisionSystem(CameraPtr forward, CameraPtr downward,
                           core::ConfigNode config, core::SubsystemList deps) :
    Subsystem("VisionSystem", deps),
    m_forwardCamera(forward),
    m_downwardCamera(downward),
    m_forwardFileRecorder(0),
    m_downwardFileRecorder(0),
    m_forwardNetworkRecorder(0),
    m_downwardNetworkRecorder(0),
    m_forward(0),
    m_downward(0),
    m_redLightDetector(DetectorPtr()),
    m_binDetector(DetectorPtr()),
    m_pipelineDetector(DetectorPtr()),
    m_ductDetector(DetectorPtr()),
    m_downwardSafeDetector(DetectorPtr()),
    m_gateDetector(DetectorPtr())
{
    init(config, core::Subsystem::getSubsystemOfType<core::EventHub>(deps));
}
    
void VisionSystem::init(core::ConfigNode config, core::EventHubPtr eventHub)
{
    if (!m_forwardCamera)
        m_forwardCamera = CameraPtr(new OpenCVCamera(1, true));

    if (!m_downwardCamera)
        m_downwardCamera = CameraPtr(new OpenCVCamera(0, false));

    // Read int as bool
    m_testing = config["testing"].asInt(0) != 0;

    // Max number of frames per second to record
    int maxRecordRate = config["maxRecordRate"].asInt(5);
    int maxStreamRate = config["maxStreamRate"].asInt(5);
    
    // Recorders
    if (config.exists("forwardFile"))
    {
        m_forwardFileRecorder = new FileRecorder( //new FFMPEGRecorder(
            m_forwardCamera.get(),
            Recorder::MAX_RATE,
            config["forwardFile"].asString(),
            maxRecordRate);
    }
    if (config.exists("forwardPort"))
    {
        m_forwardNetworkRecorder = new NetworkRecorder(
            m_forwardCamera.get(),
            Recorder::MAX_RATE,
            config["forwardPort"].asInt(),
            maxStreamRate);
    }
        

    if (config.exists("downwardFile"))
    {
        m_downwardFileRecorder = new FileRecorder( //new FFMPEGRecorder(
            m_downwardCamera.get(),
            Recorder::MAX_RATE,
            config["downwardFile"].asString(),
            maxRecordRate);

    }
    if (config.exists("downwardPort"))
    {
        m_downwardNetworkRecorder = new NetworkRecorder(
            m_downwardCamera.get(),
            Recorder::MAX_RATE,
            config["downwardPort"].asInt(),
            maxStreamRate);
    }
    
    
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

    delete m_forwardFileRecorder;
    delete m_downwardFileRecorder;
    delete m_forwardNetworkRecorder;
    delete m_downwardNetworkRecorder;
    
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

void VisionSystem::setPriority(core::IUpdatable::Priority priority)
{
    assert(m_testing && "Can't background when not testing");
    m_forwardCamera->setPriority(priority);
    m_downwardCamera->setPriority(priority);

    m_forward->setPriority(priority);
    m_downward->setPriority(priority);

    if (m_forwardFileRecorder)
        m_forwardFileRecorder->setPriority(priority);
    if (m_downwardFileRecorder)
        m_downwardFileRecorder->setPriority(priority);
    if (m_forwardNetworkRecorder)
        m_forwardNetworkRecorder->setPriority(priority);
    if (m_downwardNetworkRecorder)
        m_downwardNetworkRecorder->setPriority(priority);        
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
    if (m_forwardFileRecorder)
        m_forwardFileRecorder->background(interval);
    if (m_downwardFileRecorder)
        m_downwardFileRecorder->background(interval);
    if (m_forwardNetworkRecorder)
        m_forwardNetworkRecorder->background(interval);
    if (m_downwardNetworkRecorder)
        m_downwardNetworkRecorder->background(interval);    
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
    if (m_forwardFileRecorder)
        m_forwardFileRecorder->unbackground(join);
    if (m_downwardFileRecorder)
        m_downwardFileRecorder->unbackground(join);
    if (m_forwardNetworkRecorder)
        m_forwardNetworkRecorder->unbackground(join);
    if (m_downwardNetworkRecorder)
        m_downwardNetworkRecorder->unbackground(join);    
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
    if (m_forwardFileRecorder)
        m_forwardFileRecorder->update(timestep);
    if (m_downwardFileRecorder)
        m_downwardFileRecorder->update(timestep);
    if (m_forwardNetworkRecorder)
        m_forwardNetworkRecorder->update(timestep);
    if (m_downwardNetworkRecorder)
        m_downwardNetworkRecorder->update(timestep);
}
    
} // namespace vision
} // namespace ram
