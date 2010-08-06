/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/SymbolDetector.h
 */

#ifndef RAM_VISION_SYMBOLDETECTOR_H_06_30_2009
#define RAM_VISION_SYMBOLDETECTOR_H_06_30_2009

// Project Includes
#include "vision/include/Detector.h"
#include "vision/include/Symbol.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT SymbolDetector : public Detector
{
public:
    /** Get the current symbol */
    virtual Symbol::SymbolType getSymbol() = 0;

    /** Whether or not the detector needs the image cropped to a square */
    virtual bool needSquareCropped() = 0;
    
protected:
    SymbolDetector(core::EventHubPtr eventHub = core::EventHubPtr());
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_SYMBOLDETECTOR_H_06_30_2009
