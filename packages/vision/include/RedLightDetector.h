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

#include <iostream>
#include <sstream>
#include <math.h>
#include <cstdlib>
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include <string>
#include "vision/include/main.h"
#include "vision/include/ProcessList.h"
#include "vision/include/VisionCommunication.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Image.h"
#include "vision/include/Camera.h"
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT RedLightDetector
{
  public:
    bool found;
    RedLightDetector(OpenCVCamera*);
    ~RedLightDetector();
    void update();
    void processImage(Image* input, Image* output);
    double getX();
    double getY();
    void show(char* window);
    IplImage* getAnalyzedImage();
    
  private:
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
    
    ram::vision::Image* frame;
    ram::vision::OpenCVCamera* cam;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_RED_LIGHT_DETECTOR_H_06_23_2007
