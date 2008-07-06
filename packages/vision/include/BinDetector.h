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
#include "core/include/ConfigNode.h"
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/SuitDetector.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT BinDetector : public Detector
{
  public:
    BinDetector(core::ConfigNode config,
                core::EventHubPtr eventHub = core::EventHubPtr());
    BinDetector(Camera*);
    ~BinDetector();

    void processImage(Image* input, Image* output= 0);
    void update();
    void show(char* window);
    bool found();
    IplImage* getAnalyzedImage();
    
  private:
    void init(core::ConfigNode config);

    bool m_found;
    int numBinsFound;
    IplImage* binFrame;
    IplImage* rotated;
    IplImage* bufferFrame;
	IplImage* whiteMaskedFrame;
	IplImage* blackMaskedFrame;
	
    Image* frame;
    Camera* cam;
    SuitDetector suitDetector;
	BlobDetector blobDetector;
    /** Maximum distance for the bin to be considred "centered" */
    double m_centeredLimit;
    
    /** Whether or not we are centered */
    bool m_centered;
    
    Suit suitCenteredOver;
    
};
	
} // namespace vision
} // namespace ram

#endif // RAM_BIN_DETECTOR_H_06_23_2007
