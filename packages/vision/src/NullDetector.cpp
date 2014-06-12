/*
 * Copyright (C) 2012 Robotics at Maryland
 * Copyright (C) 2012 Gary Sullivan
 * All rights reserved.
 *
 * Author: Gary Sullivan <gsulliva@umd.edu>
 * File:  packages/vision/src/NullDetector.cpp
 */

// STD Includes
#include <iostream>
#include <string>


// Library Includes
#include "cv.h"
#include "highgui.h"
#include <log4cpp/Category.hh>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// Project Includes
#include "core/include/Logging.h"
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

#include "vision/include/NullDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Color.h"
#include "vision/include/Events.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Utility.h"
#include "vision/include/VisionSystem.h"


static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("NullDetector"));

namespace ram {
namespace vision {

NullDetector::NullDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_logImages(false)
{
    init(config);
}

NullDetector::NullDetector(Camera* camera) :
    cam(camera),
    m_logImages(false)
{
    init(core::ConfigNode::fromString("{}"));
}

void NullDetector::init(core::ConfigNode config)
{
    LOGGER.info("Initializing Null Detector");

    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());


    propSet->addProperty(config, false, "logImages",
        "Log all the images passed to the detector to disk",
        false, &m_logImages);
    
    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);
        
    // Working images
    frame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    
        
}

NullDetector::~NullDetector()
{

}

void NullDetector::update()
{
    cam->getImage(frame);
    processImage(frame);
}

void NullDetector::setImageLogging(bool value)
{
    m_logImages = value;
}


void NullDetector::processImage(Image* input, Image* output)
{
    
    frame->copyFrom(input);

    if(m_logImages)
    {
        logImage(frame);
    }
    output->copyFrom(input);
}


void NullDetector::logImage(Image* image)
{
    static int saveCount = 1;
    
    if (saveCount == 1)
    {
        // First run, make sure all the directories are created
        boost::filesystem::path base = core::Logging::getLogDir() / "loggedImages";

        if (!boost::filesystem::exists(base))
            boost::filesystem::create_directories(base);
    }

    // Determine the directory to place the image based on symbol
    boost::filesystem::path baseDir = core::Logging::getLogDir() / "loggedImages";

    // Determine the final path
    std::stringstream ss;
    ss << saveCount << ".png";
    boost::filesystem::path fullPath =  baseDir / ss.str();
    
    // Save the image and increment our counter
    Image::saveToFile(image, fullPath.string());
    saveCount++;
}

} // namespace vision
} // namespace ram
