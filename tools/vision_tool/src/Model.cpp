/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  tools/vision_viewer/include/Model.h
 */

// STD Includes
#include <utility>
#include <iostream>

// Library Includes
#include <wx/timer.h>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

// Core Includes
#include "Model.h"

#include "core/include/EventPublisher.h"
#include "core/include/PropertySet.h"

#include "vision/include/VisionSystem.h"
#include "vision/include/Camera.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/FFMPEGCamera.h"
#include "vision/include/RawFileCamera.h"
#include "vision/include/Detector.h"
#include "vision/include/DetectorMaker.h"
#include "vision/include/Events.h"


RAM_CORE_EVENT_TYPE(ram::tools::visionvwr::Model, IMAGE_SOURCE_CHANGED);
RAM_CORE_EVENT_TYPE(ram::tools::visionvwr::Model, NEW_IMAGE);
RAM_CORE_EVENT_TYPE(ram::tools::visionvwr::Model, DETECTOR_CHANGED);
RAM_CORE_EVENT_TYPE(ram::tools::visionvwr::Model, DETECTOR_PROPERTIES_CHANGED);

namespace bfs = boost::filesystem;

namespace ram {
namespace tools {
namespace visionvwr {

BEGIN_EVENT_TABLE(Model, wxEvtHandler)
END_EVENT_TABLE()
    
Model::Model() :
    m_camera(0),
    m_timer(new wxTimer(this)),
    m_latestImage(new vision::OpenCVImage(
                      640, 480, vision::Image::PF_BGR_8)),
    m_detectorInput(new vision::OpenCVImage(
                        640, 480, vision::Image::PF_BGR_8)),
    m_detectorOutput(new vision::OpenCVImage(
                         640, 480, vision::Image::PF_BGR_8)),
    m_imageToSend(m_latestImage),
    m_detector(vision::DetectorPtr()),
    m_detectorType(""),
    m_configPath("")
{
    Connect(m_timer->GetId(), wxEVT_TIMER,
            wxTimerEventHandler(Model::onTimer));
}

Model::~Model()
{
    m_timer->Stop();
    delete m_camera;
    delete m_timer;
    delete m_latestImage;
    delete m_detectorInput;
    delete m_detectorOutput;
}
    
void Model::setConfigPath(std::string configPath)
{
    m_configPath = configPath;
    if (m_detectorType.size() > 0)
        changeToDetector(m_detectorType);
}

void Model::openFile(std::string filename)
{
    if (m_camera)
        delete m_camera;

    std::string extension = bfs::path(filename).extension();

    if (".rmv" == extension)
        m_camera = new vision::RawFileCamera(filename);
    else
        m_camera = new vision::FFMPEGCamera(filename);
    
    sendImageSourceChanged();
    sendNewImage();
}
    
void Model::openCamera(int num)
{
    if (m_camera)
        delete m_camera;

    if (-1 == num)
        m_camera = new vision::OpenCVCamera();
    else
        m_camera = new vision::OpenCVCamera(num, true);
    
    sendImageSourceChanged();
    sendNewImage();
}


void Model::stop()
{
    m_timer->Stop();
}

void Model::start()
{
    if (running())
        stop();
    
    double fpsNum = fps();
    if (fpsNum == 0.0)
        fpsNum = 30;
    m_updateInterval = 1.0 / fpsNum;

    m_nextUpdate = core::TimeVal::timeOfDay() + core::TimeVal(m_updateInterval);
    
    m_timer->Start((int)(1000 / fpsNum), true);
}

bool Model::running()
{
    return m_timer->IsRunning();
}

double Model::fps()
{
    if (m_camera)
        return m_camera->fps();
    else
        return 0;
}
    
double Model::duration()
{
    if (m_camera)
        return m_camera->duration();
    else
        return 0;
}
    
void Model::seekToTime(double seconds)
{
    if (m_camera)
    {
        m_camera->seekToTime(seconds);
        m_nextUpdate = core::TimeVal::timeOfDay() +
            core::TimeVal(m_updateInterval * 2);
        sendNewImage();
    }
}
    

double Model::currentTime()
{
    if (m_camera)
        return m_camera->currentTime();
    else
        return 0;
}

std::vector<std::string> Model::getDetectorNames()
{
    return vision::DetectorMaker::getRegisteredKeys();
}

void Model::changeToDetector(std::string detectorType)
{
    // Make sure we are displaying the results of the detector
    m_imageToSend = m_detectorOutput;

    // Drop our detector
    m_detector = vision::DetectorPtr();

    // Make sure we have a valid detector
    if (!vision::DetectorMaker::isKeyRegistered(detectorType))
    {
        std::cerr << "Detector '" << detectorType
                  << "' is not a valid detector" << std::endl;
	assert(false && "Not a valid detector");
    }

    if (m_configPath.size() > 0)
    {
        std::string nodeUsed;
        core::ConfigNode cfg(core::ConfigNode::fromFile(m_configPath));

        // Attempt to find at the base level
        m_detector = createDetectorFromConfig(detectorType, cfg, nodeUsed);

        if (!m_detector)
        {
	    cfg = vision::VisionSystem::findVisionSystemConfig(cfg, nodeUsed);
	    if (nodeUsed.size() > 0)
	    {
	        m_detector = createDetectorFromConfig(detectorType, cfg, 
						      nodeUsed);
	    }
	}
    }

    if (!m_detector)
    {
        // Setup the configuration file
        m_detectorType = detectorType;
	core::ConfigNode config(core::ConfigNode::fromString("{}"));
	config.set("type", m_detectorType);

	// Make the detector
	m_detector = vision::DetectorMaker::newObject(
            std::make_pair(config, core::EventHubPtr()));
    }

    // Update display without getting a new image from the source
    sendNewImage(false);
    sendDetectorChanged();
}


void Model::disableDetector()
{
    // Drop reference to the detector, which delete it
    m_detector = vision::DetectorPtr();
    m_detectorType = "";
    // Change the image we are sending to the latest image form the image source
    m_imageToSend = m_latestImage;

    // Update display without getting a new image from the source
    sendNewImage(false);
    sendDetectorChanged();
}

void Model::detectorPropertiesChanged()
{
    // Send a new image, but just reprocess the currently captured one
    sendNewImage(false);

    // Tell everyone our properties have been updated
    publish(DETECTOR_PROPERTIES_CHANGED, core::EventPtr(new core::Event));
}

core::PropertySetPtr Model::getDetectorPropertySet()
{
    core::PropertySetPtr propSet;
    if (m_detector)
        propSet = m_detector->getPropertySet();
    return propSet;
}

vision::Image* Model::getLatestImage()
{
    if (m_timer->IsRunning())
	// Not thread safe if this is running
	return NULL;
    else
	return m_latestImage;
}

void Model::onTimer(wxTimerEvent &event)
{
    if (m_camera)
    {
        // Send an image
        sendNewImage();

        // Advance the update the standard ammount
        m_nextUpdate += core::TimeVal(m_updateInterval);

        // Advance out next update until its in the future
        while ((m_nextUpdate - core::TimeVal::timeOfDay()).get_double() < 0)
        {
            m_nextUpdate += core::TimeVal(m_updateInterval);
            m_camera->seekToTime(currentTime() + m_updateInterval);
        }

        // Sleep until that time
        core::TimeVal sleepTime(m_nextUpdate - core::TimeVal::timeOfDay());
        m_timer->Start((int)(sleepTime.get_double() * 1000), true);
    }
}
    
void Model::sendNewImage(bool grabFromSource)
{
    // Grab the latest image
    if (grabFromSource)
    {
        m_camera->update(0);
	m_camera->getImage(m_latestImage);
    }

    // If we have detector process the image so we can show the debug result
    if (m_detector)
    {
        m_detectorInput->copyFrom(m_latestImage);
        m_detector->processImage(m_detectorInput, m_detectorOutput);
    }
    
    // Send the event
    vision::ImageEventPtr event(new vision::ImageEvent(m_imageToSend));
    publish(NEW_IMAGE, event);
}
    
void Model::sendImageSourceChanged()
{
    publish(IMAGE_SOURCE_CHANGED, core::EventPtr(new core::Event));
}

void Model::sendDetectorChanged()
{
    publish(DETECTOR_CHANGED, core::EventPtr(new core::Event));
}

vision::DetectorPtr Model::createDetectorFromConfig(std::string detectorType,
						    core::ConfigNode cfg,
						    std::string& nodeUsed)
{
    core::NodeNameList nodeNames(cfg.subNodes());
    // Go through each section and check its type
    BOOST_FOREACH(std::string nodeName, nodeNames)
    {
        core::ConfigNode cfgSection(cfg[nodeName]);
        if ((detectorType == cfgSection["type"].asString("NONE"))
             || (nodeName == detectorType))
        {
            nodeUsed = nodeName;
            cfgSection.set("type", detectorType);
            return vision::DetectorMaker::newObject(
                vision::DetectorMakerParamType(cfgSection,
                                               core::EventHubPtr()));
        }
    }
    
    return vision::DetectorPtr();
}


} // namespace visionvwr
} // namespace tools
} // namespace ram
