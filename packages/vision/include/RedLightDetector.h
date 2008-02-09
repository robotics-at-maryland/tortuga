/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/RedLightDetector.h
 */

#ifndef RAM_RED_LIGHT_DETECTOR_H_06_23_2007
#define RAM_RED_LIGHT_DETECTOR_H_06_23_2007

// Library Includes
#include "cv.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT RedLightDetector : public Detector
{
  public:
    RedLightDetector(core::ConfigNode config,
                     core::EventHubPtr eventHub = core::EventHubPtr());
    RedLightDetector(Camera* camera);
    ~RedLightDetector();

    void update();
    void processImage(Image* input, Image* output= 0);

    bool found;
    double getX();
    double getY();
    
    void show(char* window);
    IplImage* getAnalyzedImage();
    
  private:
    void init();
    
    int lightFramesOff;
    int lightFramesOn;
    int blinks;
    int spooky;
    bool startCounting;
    double redLightCenterX;
    double redLightCenterY;
    int minRedPixels;
    IplImage* image;
    IplImage* raw;
    IplImage* flashFrame;
    IplImage* saveFrame;
    CvPoint lightCenter;
    
    Image* frame;
    Camera* cam;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_RED_LIGHT_DETECTOR_H_06_23_2007
