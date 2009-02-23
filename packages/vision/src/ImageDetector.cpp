/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 David Love
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/src/ImageDetector.cpp
 */

//Project Includes
#include "vision/include/ImageDetector.hpp"

namespace ram {
namespace vision {

<<<<<<< .mine
ImageDetector::ImageDetector(core::ConfigNode config, core::EventHubPtr eventHub) :
    Detector(eventHub), m_identifier (config["SavedImageIdentifierNetwork"].asString ("")), m_lastResult (-1)
=======
ImageDetector::ImageDetector(core::ConfigNode config,
                             core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_identifier(config["SavedImageIdentifierNetwork"].asString("")),
    m_lastResult (-1)
>>>>>>> .r4782
{
}

void ImageDetector::processImage(Image* input, Image* output)
{
    m_lastResult = m_identifier.run (input);
}

int ImageDetector::getResult()
{
	return m_lastResult;
}

} // namespace vision
} // namespace ram
