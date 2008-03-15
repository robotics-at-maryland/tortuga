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

    /** Center of the red light in the local horizontal, -4/3 to 4/3 */
    double getX();
    
    /** Center of the red light in the local vertical, -1 to 1 */
    double getY();
    
    void show(char* window);
    IplImage* getAnalyzedImage();
    
  private:
    void init(core::ConfigNode config);
    
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

    /** Initial level of minimum red pixels (and lower bound) */
    int m_initialMinRedPixels;

    /** Factor to change minRedPixels by for each found frame */
    double m_foundMinPixelScale;

    /** Factor to change minRedPixels by for each found frame */
    double m_lostMinPixelScale;

    /** Thresdhol for almost hitting the red light */
    int m_almostHitPixels;

    /** Percentage of the image to remove from the top */
    double m_topRemovePercentage;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_RED_LIGHT_DETECTOR_H_06_23_2007
