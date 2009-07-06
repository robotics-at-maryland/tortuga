/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/include/FANNSuitDetector.hpp
 */

#ifndef RAM_VISION_FANN_SUIT_DETECTOR_HPP
#define RAM_VISION_FANN_SUIT_DETECTOR_HPP

// Project Includes
#include "vision/include/FANNSymbolDetector.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** Implements FANNSymbolDetector to find the 2008 Symbols
 *
 *  Each feature it extracts is a pixel of the passed in image scaled to 64x64
 *  pixels.
 */
class RAM_EXPORT FANNSuitDetector : public FANNSymbolDetector
{
  public:
    FANNSuitDetector(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr());

    // Detector Methods
    void processImage(Image* input, Image* output= 0);
        
    // SymbolDetector Methods
    Symbol::SymbolType getSymbol();
    virtual bool needSquareCropped() { return true; }

    // FANNSymbolDetector
    virtual void getImageFeatures(Image* inputImage, float* features);
    
  private:
    Image* m_resizedImage;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_SUIT_DETECTOR_H_04_29_2008
