/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/BlobDetector.h
 */


#ifndef RAM_VISION_ADAPTIVETHRESHER_H_08_02_2008
#define RAM_VISION_ADAPTIVETHRESHER_H_08_02_2008

#include "vision/include/Detector.h"
#include "core/include/ConfigNode.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/RedLightDetector.h"

namespace ram {
namespace vision {

class RAM_EXPORT AdaptiveThresher : public Detector
{
    public:
//        AdaptiveThresher(int pixelSearchRadius, int distThresholdSquared);
        AdaptiveThresher(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
        ~AdaptiveThresher();
        int pixelSearchRange;
        int* setUnionTable;
        int* pixelCounts;
        float* finalColorTableB, *finalColorTableG, * finalColorTableR;
        int curWidth;
        int curHeight;
        int distThreshSquared;
        void processImage(Image* in, Image* out);
        void segmentImage(Image* in, Image* out);
        void findCircle();
        OpenCVImage m_working;
//        RedLightDetector m_lightDetector;
        IplImage* cannied;
        IplImage* houghCircled;        
};




}//ram
}//vision

#endif // RAM_VISION_ADAPTIVETHRESHER_H_08_02_2008
