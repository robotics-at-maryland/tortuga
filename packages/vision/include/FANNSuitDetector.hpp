/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 David Love
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/include/ImageDetector.hpp
 */

#ifndef RAM_VISION_FANN_SUITE_DETECTOR_HPP
#define RAM_VISION_FANN_SUITE_DETECTOR_HPP

// Project Includes
#include "vision/include/Suit.h"
#include "vision/include/ImageDetector.hpp"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** Wraps ImageDetector to create a card-suite specific interface
 identical to that of SuiteDetector, but with lotsa stub functions **/
class RAM_EXPORT FANNSuitDetector : public Detector
{
private:
    ImageDetector m_imageDetector;
    IplImage* m_analyzed;
public:
    FANNSuitDetector (core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    void processImage(Image* input, Image* output= 0);
    /** Stub don't do nuttin' **/
    void update() {};
    /** Get the analyzed image - just returns the input **/
    IplImage* getAnalyzedImage();
    /** Get the suite type discovered **/
    Suit::SuitType getSuit();
    /** Stub don't do nuttin' **/
    int edgeRun(int startx, int starty, int endx, int endy, IplImage* img, int dir, int* numBackups = 0) { return 0; }
    /** Stub don't do nutin' **/
    void doEdgeRunning(IplImage*) {}
    /** Stub don't do nutin' **/
    bool makeSuitHistogram(IplImage*) { return true; }
    /** Stub don't do nutin' **/
	bool cropImage(IplImage*) { return true; }
    /** Stub don't do nutin' **/
    bool cyclicCompare(int[], int[], int[], int) { return true; }
};
	
} // namespace vision
} // namespace ram

#endif // RAM_SUIT_DETECTOR_H_04_29_2008
