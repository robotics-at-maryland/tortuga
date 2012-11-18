/*
 * Copyright (C) 2012 Robotics at Maryland
 * Copyright (C) 2012 Gary Sullivan
 * All rights reserved.
 *
 * Author: Gary Sullivan <gsulliva@umd.edu>
 * File:  packages/vision/include/NullDetector.h
 */

#ifndef RAM_NULL_DETECTOR_H_07_04_2012
#define RAM_NULL_DETECTOR_H_07_04_2012

// STD Includes
#include <list>
#include <map>

// Project Includes
#include "core/include/ConfigNode.h"
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT NullDetector : public Detector
{
  public:
    NullDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    NullDetector(Camera* camera);
    ~NullDetector();

    void update();
    void processImage(Image* input, Image* output = 0);
    
    void setImageLogging(bool value);
  
  private:

    /** Logs the image to file */
    void logImage(Image* image);

    void init(core::ConfigNode config);

    Camera *cam;

    /** Working Images */
    Image *frame;

    /**Working images for detection algorithm*/
    bool m_logImages;
    
};
	
} // namespace vision
} // namespace ram

#endif // RAM_NULL_DETECTOR_H_07_04_2012
