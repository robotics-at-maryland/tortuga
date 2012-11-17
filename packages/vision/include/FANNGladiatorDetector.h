/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FANNGladiatorDetector.h
 */

#ifndef RAM_VISION_FANNGLADIATORDETECTOR_H_06_27_2010
#define RAM_VISION_FANNGLADIATORDETECTOR_H_06_27_2010

// Project Includes
#include "vision/include/FANNSymbolDetector.h"
#include "vision/include/BlobDetector.h"
// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT FANNGladiatorDetector : public FANNSymbolDetector
{
  public:
    FANNGladiatorDetector(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr());

    virtual ~FANNGladiatorDetector();

    // Detector Methods
    void processImage(Image* input, Image* output = 0);

    // Symbol Detector Methods
    Symbol::SymbolType getSymbol();
    virtual bool needSquareCropped() { return false; }

    // FANN Symbol Detector
    virtual void getImageFeatures(Image* inputImage, float* features);

  private:
    Image* m_frame;
    Image* m_croppedFrame;
    unsigned char *m_scratchBuffer;
    BlobDetector m_blobDetector;

    Image* cropBinImage(Image* binImage, unsigned char* buffer);
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_FANNGLADIATORDETECTOR_H_06_27_2010
