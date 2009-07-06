/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FANNWW2Detector.h
 */

#ifndef RAM_VISION_FANNWW2DETECTOR_H_07_05_2009
#define RAM_VISION_FANNWW2DETECTOR_H_07_05_2009

// Project Includes
#include "vision/include/FANNSymbolDetector.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** Implements FANNSymbolDetector to find the 2009 Symbols
 *
 *  The features are a the amount of red in the four corners of the image, the
 *  aspect ratio of the image, and the amount of red on the two half of the
 *  image split the longest way.
 */
class RAM_EXPORT FANNWW2Detector : public FANNSymbolDetector
{
  public:
    FANNWW2Detector(core::ConfigNode config,
                    core::EventHubPtr eventHub = core::EventHubPtr());

    // Detector Methods
    void processImage(Image* input, Image* output= 0);
        
    // SymbolDetector Methods
    Symbol::SymbolType getSymbol();
    virtual bool needSquareCropped() { return false; }

    // FANNSymbolDetector
    virtual void getImageFeatures(Image* inputImage, float* features);
    
  private:
    Image* m_resizedImage;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_VISION_FANNWW2DETECTOR_H_07_05_2009
