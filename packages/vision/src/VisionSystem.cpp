/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/VisionSystem.cpp
 */

#include <iostream>

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/VisionSystem.h"
#include "vision/include/VisionRunner.h"
#include "vision/include/DC1394Camera.h"
#include "vision/include/FileRecorder.h"
#include "vision/include/NetworkRecorder.h"
#include "vision/include/LCHConverter.h"

#include "vision/include/RedLightDetector.h"
#include "vision/include/BuoyDetector.h"
#include "vision/include/BinDetector.h"

#include "vision/include/ChrisPipeDetector.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/DuctDetector.h"
#include "vision/include/TargetDetector2014.h"
#include "vision/include/WindowDetector.h"
#include "vision/include/CupidDetector.h"
#include "vision/include/BarbedWireDetector.h"
#include "vision/include/SafeDetector.h"
#include "vision/include/GateDetector.h"
#include "vision/include/VelocityDetector.h"
#include "vision/include/HedgeDetector.h"
#include "vision/include/LoversLaneDetector.h"
#include "vision/include/ChrisPipeDetector.h"

#include "core/include/EventHub.h"
#include "core/include/SubsystemMaker.h"
#include "core/include/Logging.h"

// Register controller in subsystem maker system
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(ram::vision::VisionSystem, VisionSystem);

namespace ram {
namespace vision {
using namespace vision2014;

static math::Degree FUJINON_TF28DA8_HFOV(89 + 8.0 / 60);
static math::Degree FUJINON_TF28DA8_VFOV(69 + 20.0 / 60);
static math::Degree FISHEYE_HFO(107);
static math::Degree FISHEYE_VFO(78);
static math::Degree WIDE_HFO(150);
static math::Degree WIDE_VFO(81);
    
math::Degree VisionSystem::s_frontHorizontalFieldOfView = FUJINON_TF28DA8_HFOV;
math::Degree VisionSystem::s_frontVeritcalFieldOfView = FUJINON_TF28DA8_VFOV;
int VisionSystem::s_frontHorizontalPixelResolution = 640;
int VisionSystem::s_frontVerticalPixelResolution = 480;
math::Degree VisionSystem::s_downHorizontalFieldOfView = FUJINON_TF28DA8_HFOV;
math::Degree VisionSystem::s_downVeritcalFieldOfView = FUJINON_TF28DA8_VFOV;
int VisionSystem::s_downHorizontalPixelResolution = 640;
int VisionSystem::s_downVerticalPixelResolution = 480;
    
VisionSystem::VisionSystem(core::ConfigNode config,
                           core::SubsystemList deps) :
    Subsystem(config["name"].asString("VisionSystem"), deps),
    m_forwardCamera(CameraPtr()),
    m_downwardCamera(CameraPtr()),
    m_forward(0),
    m_downward(0),
    m_buoyDetector(DetectorPtr()),
    m_binDetector(DetectorPtr()),
    m_pipelineDetector(DetectorPtr()),
    m_gateDetector(DetectorPtr()),
    m_cupidDetector(DetectorPtr()),
    m_loversLaneDetector(DetectorPtr())
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
    m_buoyDetector(DetectorPtr()),
    m_binDetector(DetectorPtr()),
    m_pipelineDetector(DetectorPtr()),
    m_gateDetector(DetectorPtr()),
    m_cupidDetector(DetectorPtr()),
    m_loversLaneDetector(DetectorPtr())
{
    init(config, core::Subsystem::getSubsystemOfType<core::EventHub>(deps));
}
    
void VisionSystem::init(core::ConfigNode config, core::EventHubPtr eventHub)
{
    if (!m_forwardCamera)
    {
        core::ConfigNode cameraConfig(config["ForwardCamera"]);
        std::string cameraStr = cameraConfig["typeStr"].asString("100");
        std::string message;
        m_forwardCamera = CameraPtr(Camera::createCamera(cameraStr,
                                                         cameraConfig,
                                                         message,
                                                         eventHub));
    }

    if (!m_downwardCamera)
    {
        core::ConfigNode cameraConfig(config["DownwardCamera"]);
        std::string cameraStr = cameraConfig["typeStr"].asString("101");
        std::string message;
        m_downwardCamera = CameraPtr(Camera::createCamera(cameraStr,
                                                          cameraConfig,
                                                          message,
                                                          eventHub));
    }

    // Read int as bool
    m_testing = config["testing"].asInt(0) != 0;

    // Load the lookup table if necessary
    int lchLookupTable = config["loadLCHLookupTable"].asInt(0);
    if (lchLookupTable) {
        bool loaded = LCHConverter::loadLookupTable();
        assert(loaded && "Failed to load lookup table");
    }

    // Recorders
    if (config.exists("ForwardRecorders"))
        createRecordersFromConfig(config["ForwardRecorders"], m_forwardCamera);

    if (config.exists("DownwardRecorders"))
        createRecordersFromConfig(config["DownwardRecorders"], m_downwardCamera);
    
    // Detector runners (go as fast as possible)
    m_forward = new VisionRunner(m_forwardCamera.get(), Recorder::NEXT_FRAME);
    m_downward = new VisionRunner(m_downwardCamera.get(), Recorder::NEXT_FRAME);

    // Detectors
    m_buoyDetector = DetectorPtr(
        new BuoyDetector(getConfig(config, "BuoyDetector"), eventHub));
    m_binDetector = DetectorPtr(
        new BinDetector(getConfig(config, "BinDetector"), eventHub));

    m_pipelineDetector = DetectorPtr(
        new ChrisPipeDetector(getConfig(config, "ChrisPipeDetector"),eventHub));

  // m_pipelineDetector = DetectorPtr(
  //      new OrangePipeDetector(getConfig(config, "OrangePipeDetector"),eventHub));

    m_forwardpipelineDetector = DetectorPtr(
        new ChrisPipeDetector(getConfig(config, "FowardPipeDetector"),eventHub));

    m_pipelineDetector = DetectorPtr( 
        new ChrisPipeDetector(getConfig(config,"ChrisPipeDetector"),eventHub));
    m_downwardSafeDetector = DetectorPtr(
        new  ChrisPipeDetector(getConfig(config, "SonarChrisPipeDetector"), eventHub));
    m_gateDetector = DetectorPtr(
        new GateDetector(getConfig(config, "GateDetector"), eventHub));
    m_cupidDetector = DetectorPtr(
        new TargetDetector(getConfig(config, "TargetDetector"), eventHub));
    m_loversLaneDetector = DetectorPtr(
        new GateDetector(getConfig(config, "HedgeDetector"), eventHub));

    // Start camera in the background (at the fastest rate possible)
    m_forwardCamera->background(-1);
    m_downwardCamera->background(-1);
}
    
void VisionSystem::createRecordersFromConfig(core::ConfigNode recorderCfg,
                                             CameraPtr camera)
{
    BOOST_FOREACH(std::string recorderString, recorderCfg.subNodes())
    {
        // Get in the rate, or the signal to record every frame
        int frameRate = recorderCfg[recorderString].asInt();

        // Create the recorder
        createRecorder(camera, recorderString, frameRate, true);
    }
}
    
void VisionSystem::createRecorder(CameraPtr camera, std::string recorderString,
                                  int frameRate, bool debugPrint)
{
    Recorder::RecordingPolicy policy = Recorder::MAX_RATE;

    // Get in the rate, or the signal to record every frame
    int policyArg = frameRate;
    if (policyArg <= 0)
        policy = Recorder::NEXT_FRAME;

    // Create the actual recorder
    std::string message;
    Recorder* recorder = Recorder::createRecorderFromString(
        recorderString, camera.get(), message, policy, policyArg,
        core::Logging::getLogDir().string());
    if (debugPrint)
        std::cout << "RECORDING>>>> "  << message << std::endl;

    // Store it for later desctruction
    m_recorders[recorderString] = recorder;
}
    
void VisionSystem::addForwardDetector(DetectorPtr detector)
{
    assert(detector && "Can't use a NULL detector");
    m_forward->addDetector(detector);
}

void VisionSystem::addDownwardDetector(DetectorPtr detector)
{
    assert(detector && "Can't use a NULL detector");
    m_downward->addDetector(detector);
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
    BOOST_FOREACH(StrRecorderMapPair pair, m_recorders)
        delete pair.second;
    
    // Shutdown our detectors running on our cameras
    delete m_forward;
    delete m_downward;
}

void VisionSystem::binDetectorOn()
{
    addDownwardDetector(m_binDetector);
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
    addDownwardDetector(m_pipelineDetector);
    publish(EventType::PIPELINE_DETECTOR_ON,
            core::EventPtr(new core::Event()));
}

void VisionSystem::pipeLineDetectorOff()
{
    m_downward->removeDetector(m_pipelineDetector);
    publish(EventType::PIPELINE_DETECTOR_OFF,
            core::EventPtr(new core::Event()));
}


void VisionSystem::forwardpipeLineDetectorOn()
{
  addForwardDetector(m_forwardpipelineDetector);
    publish(EventType::PIPELINE_DETECTOR_ON,
            core::EventPtr(new core::Event()));
}

void VisionSystem::forwardpipeLineDetectorOff()
{
	printf("\n forwardpipeLine is OFF");
      m_forward->removeDetector(m_forwardpipelineDetector);
    publish(EventType::PIPELINE_DETECTOR_OFF,
            core::EventPtr(new core::Event()));
}

void VisionSystem::downwardSafeDetectorOn()
{
    addDownwardDetector(m_downwardSafeDetector);
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
    addForwardDetector(m_gateDetector);
    publish(EventType::GATE_DETECTOR_ON,
            core::EventPtr(new core::Event()));
}

void VisionSystem::gateDetectorOff()
{
    m_forward->removeDetector(m_gateDetector);
    publish(EventType::GATE_DETECTOR_OFF,
            core::EventPtr(new core::Event()));
}

void VisionSystem::buoyDetectorOn()
{
    addForwardDetector(m_buoyDetector);
    publish(EventType::BUOY_DETECTOR_ON,
            core::EventPtr(new core::Event()));
}

void VisionSystem::buoyDetectorOff()
{
    m_forward->removeDetector(m_buoyDetector);
    publish(EventType::BUOY_DETECTOR_OFF,
            core::EventPtr(new core::Event()));
}

void VisionSystem::cupidDetectorOn()
{
    addForwardDetector(m_cupidDetector);
    publish(EventType::CUPID_DETECTOR_ON,
            core::EventPtr(new core::Event()));
}

void VisionSystem::cupidDetectorOff()
{
    m_forward->removeDetector(m_cupidDetector);
    publish(EventType::CUPID_DETECTOR_OFF,
            core::EventPtr(new core::Event()));
}

void VisionSystem::loversLaneDetectorOn()
{
    addForwardDetector(m_loversLaneDetector);
    publish(EventType::LOVERSLANE_DETECTOR_ON,
            core::EventPtr(new core::Event()));
}

void VisionSystem::loversLaneDetectorOff()
{
    m_forward->removeDetector(m_loversLaneDetector);
    publish(EventType::LOVERSLANE_DETECTOR_OFF,
            core::EventPtr(new core::Event()));
}

void VisionSystem::addForwardRecorder(std::string recorderString, int frameRate,
                                      bool debugPrint)
{
    if (m_recorders.end() == m_recorders.find(recorderString))
    {
        if (m_forwardCamera)
        {
            createRecorder(m_forwardCamera, recorderString, frameRate,
                           debugPrint);
        }
    }
}

void VisionSystem::addDownwardRecorder(std::string recorderString,
                                       int frameRate,
                                       bool debugPrint)
{
    if (m_recorders.end() == m_recorders.find(recorderString))
    {
        if (m_downwardCamera)
        {
            createRecorder(m_downwardCamera, recorderString, frameRate,
                           debugPrint);
        }
    }
}
    
void VisionSystem::removeForwardRecorder(std::string recorderString)
{
    if (m_recorders.end() != m_recorders.find(recorderString))
    {
        delete m_recorders[recorderString];
        m_recorders.erase(recorderString);
    }
    else
    {
        std::cerr<<"Could not remove forward recorder: "<<
	  recorderString<<std::endl;
    }
}

void VisionSystem::removeDownwardRecorder(std::string recorderString)
{
    if (m_recorders.end() != m_recorders.find(recorderString))
    {
        delete m_recorders[recorderString];
        m_recorders.erase(recorderString);
    }
    else
    {
        std::cerr<<"Could not remove downward recorder: "<<
	  recorderString<<std::endl;
    }
}
    
void VisionSystem::setPriority(core::IUpdatable::Priority priority)
{
//    assert(m_testing && "Can't background when not testing");
    m_forwardCamera->setPriority(priority);
    m_downwardCamera->setPriority(priority);

    m_forward->setPriority(priority);
    m_downward->setPriority(priority);

    BOOST_FOREACH(StrRecorderMapPair pair, m_recorders)
        pair.second->setPriority(priority);
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
    BOOST_FOREACH(StrRecorderMapPair pair, m_recorders)
        pair.second->background(interval);
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
    BOOST_FOREACH(StrRecorderMapPair pair, m_recorders)
        pair.second->unbackground(join);
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
    BOOST_FOREACH(StrRecorderMapPair pair, m_recorders)
        pair.second->update(timestep);
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

core::ConfigNode VisionSystem::findVisionSystemConfig(core::ConfigNode cfg,
                                                      std::string& nodeUsed)
{
    core::ConfigNode config(core::ConfigNode::fromString("{}"));
    // Attempt to find the section deeper in the file
    if (cfg.exists("Subsystems"))
    {
        cfg = cfg["Subsystems"];
        
        // Attempt to find a VisionSystem subsystem
        core::NodeNameList nodeNames(cfg.subNodes());
        BOOST_FOREACH(std::string nodeName, nodeNames)
        {
            core::ConfigNode subsysCfg(cfg[nodeName]);
            if (("VisionSystem" == subsysCfg["type"].asString("NONE"))||
                ("SimVision" == subsysCfg["type"].asString("NONE")))
            {
                config = subsysCfg;
                std::stringstream ss;
                ss << "Subsystem:" << nodeName << ":" << nodeUsed;
                nodeUsed = ss.str();
            }
        }
    }

    return config;
}
    
} // namespace vision
} // namespace ram
