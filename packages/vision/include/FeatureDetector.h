/*
 *  RedLightDetector.h
 *  
 *
 *  Created by Daniel Hakim on 12/4/07.
 *  Copyright 2007 Daniel Hakim. All rights reserved.
 *
 */

/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/FeatureDetector.h
 */

#ifndef RAM_FEATURE_DETECTOR_H_06_23_2007
#define RAM_FEATURE_DETECTOR_H_06_23_2007

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "core/include/ConfigNode.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT FeatureDetector : public Detector
{
  public:
    FeatureDetector(core::ConfigNode config,
                    core::EventHubPtr eventHub = core::EventHubPtr());
    FeatureDetector(Camera*, int=1000);
    ~FeatureDetector();
    
    void update();
    void processImage(Image* input, Image* output= 0);
    
    void show(char* window);
    IplImage* getAnalyzedImage();
    int maxFeatures;
    
  private:
    void init(core::ConfigNode config, int maxFeatures);
    
    IplImage* image;
    IplImage* grayscale;
    IplImage* raw;
    IplImage* eigImage;
    IplImage* tempImage;
    IplImage* edgeDetected;
    CvPoint2D32f* features;
    
    void copyChannel(IplImage*, IplImage*, int);
    void singleChannelToThreeChannel(IplImage* src, IplImage* dest);
    ram::vision::Image* frame;
    ram::vision::Camera* cam;
    
};

} // namespace vision
} // namespace ram

#endif // RAM_FEATURE_DETECTOR_H_06_23_2007
