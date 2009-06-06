/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 David Love
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/include/ImageDetector.hpp
 */

#ifndef RAM_VISION_IMAGE_DETECTOR_HPP
#define RAM_VISION_IMAGE_DETECTOR_HPP

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/ImageIdentifier.hpp"

#include "core/include/ConfigNode.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT ImageDetector : public Detector
{
private:
    ImageIdentifier m_identifier;
    int m_lastResult;
public:
    /** Make a whatever this is **/
    ImageDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    /** Run the detector onn an image **/
    void processImage(Image* input, Image* output= 0);
    /** Gets the most recent result index **/
    int getResult();
};
	
} // namespace vision
} // namespace ram

#endif // RAM_SUIT_DETECTOR_H_04_29_2008
