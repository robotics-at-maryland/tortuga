/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FANNWeaponDetector.h
 */

#ifndef RAM_VISION_FANNWEAPONDETECTOR_H_06_27_2010
#define RAM_VISION_FANNWEAPONDETECTOR_H_06_27_2010

// Project Includes
#include "vision/include/FANNSymbolDetector.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT FANNWeaponDetector : public FANNSymbolDetector
{
  public:
    FANNWeaponDetector(core::ConfigNode config,
                       core::EventHubPtr eventHub = core::EventHubPtr());

    // Detector Methods
    void processImage(Image* input, Image* output = 0);

    // Symbol Detector Methods
    Symbol::SymbolType getSymbol();
    virtual bool needSquareCropped() { return false; }

    // FANN Symbol Detector
    virtual void getImageFeatures(Image* inputImage, float* features);

  private:
    Image* m_resizedImage;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_FANNWEAPONDETECTOR_H_06_27_2010
