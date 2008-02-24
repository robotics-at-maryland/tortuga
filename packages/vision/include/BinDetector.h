/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/BinDetector.h
 */

#ifndef RAM_BIN_DETECTOR_H_06_23_2007
#define RAM_BIN_DETECTOR_H_06_23_2007

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "core/include/ConfigNode.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT BinDetector : public Detector
{
  public:
    bool found;
    BinDetector(core::ConfigNode config,
                core::EventHubPtr eventHub = core::EventHubPtr());
    BinDetector(Camera*);
    ~BinDetector();

    void processImage(Image* input, Image* output= 0);
    
    void update();
    void show(char* window);
    double getX();
    double getY();
    IplImage* getAnalyzedImage();
    
  private:
    void init(core::ConfigNode config);
    
    double binX;
    double binY;
    int binCount;
    IplImage* binFrame;
    IplImage* rotated;
    IplImage* bufferFrame;
    Image* frame;
    Camera* cam;
};
	
} // namespace vision
} // namespace ram

dif // RAM_BIN_DETECTOR_H_06_23_2007
#endif // RAM_BIN_DETECTOR_H_06_23_2007
