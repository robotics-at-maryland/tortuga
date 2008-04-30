/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/SuitDetector.h
 */

#ifndef RAM_SUIT_DETECTOR_H_04_29_2008
#define RAM_SUIT_DETECTOR_H_04_29_2008

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "core/include/ConfigNode.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
		
enum Suit {CLUB, SPADE, HEART, DIAMOND, UNKNOWN};
    
class RAM_EXPORT SuitDetector : public Detector
{
  public:
    Suit suit;
    SuitDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    SuitDetector(Camera*);
    ~SuitDetector();
    void processImage(Image* input, Image* output= 0);
    void update();
    IplImage* getAnalyzedImage();
    double getX();
    double getY();
    Suit getSuit();

  private:
    Camera* cam;
    IplImage* ratioImage;
    IplImage* analyzedImage;
    void init(core::ConfigNode config);
    double suitX;
    double suitY;	
    
};
	
} // namespace vision
} // namespace ram

#endif // RAM_SUIT_DETECTOR_H_04_29_2008
