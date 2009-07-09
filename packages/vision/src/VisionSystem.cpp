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
#include "vision/include/VelocityDetector.h"

#include "core/include/EventHub.h"
#include "core/include/SubsystemMaker.h"
#include "core/include/Logging.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::vision::VisionSystem, VisionSystem);

namespace ram {
namespace vision {

static math::Degree FISHEYE_HFO(107);
static math::Degree FISHEYE_VFO(78);
static math::Degree WIDE_HFO(150);
static math::Degree WIDE_VFO(81);
    
math::Degree VisionSystem::s_frontHorizontalFieldOfView = FISHEYE_HFO;
math::Degree VisionSystem::s_frontVeritcalFieldOfView = FISHEYE_VFO;
int VisionSystem::s_frontHorizontalPixelResolution = 640;
int VisionSystem::s_frontVerticalPixelResolution = 480;
math::Degree VisionSystem::s_downHorizontalFieldOfView = WIDE_HFO;
math::Degree VisionSystem::s_downVeritcalFieldOfView = WIDE_VFO;
int VisionSystem::s_downHorizontalPixelResolution = 640;
int VisionSystem::s_downVerticalPixelResolution = 480;
    
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
    m_barbedWireDetector(DetectorPtr()),
    m_velocityDetector(DetectorPtr())
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
    m_targetDetector(DetectorPtr()),
    m_velocityDetector(DetectorPtr())
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
        new RedLightDetector(getConfig(config, "RedLightDetector"), eventHub));
    m_binDetector = DetectorPtr(
        new BinDetector(getConfig(config, "BinDetector"), eventHub));
    m_pipelineDetector = DetectorPtr(
        new OrangePipeDetector(getConfig(config, "PipelineDetector"),
                                         eventHub));
    m_ductDetector = DetectorPtr(
        new DuctDetector(getConfig(config, "DuctDetector"), eventHub));
    m_downwardSafeDetector = DetectorPtr(
        new SafeDetector(getConfig(config, "SafeDetector"), eventHub));
    m_gateDetector = DetectorPtr(
        new GateDetector(getConfig(config, "GateDetector"), eventHub));
    m_targetDetector = DetectorPtr(
        new TargetDetector(getConfig(config, "TargetDetector"), eventHub));
    m_barbedWireDetector = DetectorPtr(
        new BarbedWireDetector(getConfig(config, "BarbedWireDetector"),
                               eventHub));
    
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

core::ConfigNode VisionSystem::getConfig(core::ConfigNode config,
                                         std::string name)
{
    if (config.exists(name))
        return config[name];
    else
        return core::ConfigNode::fromString("{}");
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
    publish(EventType::BIN_DETECTOR_ON,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::binDetectorOff()
{
    m_downward->removeDetector(m_binDetector);
    publish(EventType::BIN_DETECTOR_OFF,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::pipeLineDetectorOn()
{
    m_downward->addDetector(m_pipelineDetector);
    publish(EventType::PIPELINE_DETECTOR_ON,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::pipeLineDetectorOff()
{
    m_downward->removeDetector(m_pipelineDetector);
    publish(EventType::PIPELINE_DETECTOR_OFF,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::ductDetectorOn()
{
    m_forward->addDetector(m_ductDetector);
    publish(EventType::DUCT_DETECTOR_ON,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::ductDetectorOff()
{
    m_forward->removeDetector(m_ductDetector);
    publish(EventType::DUCT_DETECTOR_OFF,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::downwardSafeDetectorOn()
{
    m_downward->addDetector(m_downwardSafeDetector);
    publish(EventType::SAFE_DETECTOR_ON,
	    core::EventPtr(new core::Event()));
}
    
void VisionSystem::downwardSafeDetectorOff()
{
    m_downward->removeDetector(m_downwardSafeDetector);
    publish(EventType::SAFE_DETECTOR_OFF,
	    core::EventPtr(new core::Event()));
}
    
void VisionSystem::gateDetectorOn()
{
    m_forward->addDetector(m_gateDetector);
    publish(EventType::GATE_DETECTOR_ON,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::gateDetectorOff()
{
    m_forward->removeDetector(m_gateDetector);
    publish(EventType::GATE_DETECTOR_OFF,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::redLightDetectorOn()
{
    m_forward->addDetector(m_redLightDetector);
    publish(EventType::RED_LIGHT_DETECTOR_ON,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::redLightDetectorOff()
{
    m_forward->removeDetector(m_redLightDetector);
    publish(EventType::RED_LIGHT_DETECTOR_OFF,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::targetDetectorOn()
{
    m_forward->addDetector(m_targetDetector);
    publish(EventType::TARGET_DETECTOR_ON,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::targetDetectorOff()
{
    m_forward->removeDetector(m_targetDetector);
    publish(EventType::TARGET_DETECTOR_OFF,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::barbedWireDetectorOn()
{
    m_forward->addDetector(m_barbedWireDetector);
    publish(EventType::BARBED_WIRE_DETECTOR_ON,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::barbedWireDetectorOff()
{
    m_forward->removeDetector(m_barbedWireDetector);
    publish(EventType::BARBED_WIRE_DETECTOR_OFF,
	    core::EventPtr(new core::Event()));
}

void VisionSystem::velocityDetectorOn()
{
    m_downward->addDetector(m_velocityDetector);
}
    
void VisionSystem::velocityDetectorOff()
{
    m_downward->removeDetector(m_velocityDetector);
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

math::Degree VisionSystem::getFrontHorizontalFieldOfView()
{
    return s_frontHorizontalFieldOfView;
}

math::Degree VisionSystem::getFrontVerticalFieldOfView()
{
    return s_frontVeritcalFieldOfView;
}

int VisionSystem::getFrontHorizontalPixelResolution()
{
    return s_frontHorizontalPixelResolution;
}

int VisionSystem::getFrontVerticalPixelResolution()
{
    return s_frontVerticalPixelResolution;
}

math::Degree VisionSystem::getDownHorizontalFieldOfView()
{
    return s_frontHorizontalFieldOfView;
}

math::Degree VisionSystem::getDownVerticalFieldOfView()
{
    return s_frontVeritcalFieldOfView;
}

int VisionSystem::getDownHorizontalPixelResolution()
{
    return s_frontHorizontalPixelResolution;
}

int VisionSystem::getDownVerticalPixelResolution()
{
    return s_frontVerticalPixelResolution;
}
    
void VisionSystem::_setFrontHorizontalFieldOfView(math::Degree degree)
{
    s_frontHorizontalFieldOfView = degree;
}

void VisionSystem::_setFrontVerticalFieldOfView(math::Degree degree)
{
    s_frontVeritcalFieldOfView = degree;
}

void VisionSystem::_setFrontHorizontalPixelResolution(int pixels)
{
    s_frontHorizontalPixelResolution = pixels;
}

void VisionSystem::_setFrontVerticalPixelResolution(int pixels)
{
    s_frontVerticalPixelResolution = pixels;    
}

void VisionSystem::_setDownHorizontalFieldOfView(math::Degree degree)
{
    s_frontHorizontalFieldOfView = degree;
}

void VisionSystem::_setDownVerticalFieldOfView(math::Degree degree)
{
    s_frontVeritcalFieldOfView = degree;
}

void VisionSystem::_setDownHorizontalPixelResolution(int pixels)
{
    s_frontHorizontalPixelResolution = pixels;
}

void VisionSystem::_setDownVerticalPixelResolution(int pixels)
{
    s_frontVerticalPixelResolution = pixels;    
}
    
} // namespace vision
} // namespace ram
