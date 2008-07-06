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
#include "vision/include/BlobDetector.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
		
enum Suit {CLUB, CLUBR90, CLUBR180, CLUBR270, 
           SPADE, SPADER90, SPADER180, SPADER270, 
           HEART, HEARTR90, HEARTR180, HEARTR270, 
           DIAMOND, DIAMONDR90, DIAMONDR180, DIAMONDR270, 
           UNKNOWN, NONEFOUND};
    
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
    Suit getSuit();
	int edgeRun(int startx, int starty, IplImage* img);
    bool makeSuitHistogram(IplImage*);
	
  private:
    Camera* cam;
    IplImage* ratioImage;
    IplImage* analyzedImage;
	IplImage* tempHoughImage;
    void init(core::ConfigNode config);
    BlobDetector blobDetector;
    IplImage* scaledRedSuit;
    int histoArr[128]; //twice scaledRedSuit's height.
    static const int HISTOARRSIZE = 128;    
};
	
} // namespace vision
} // namespace ram

#endif // RAM_SUIT_DETECTOR_H_04_29_2008
