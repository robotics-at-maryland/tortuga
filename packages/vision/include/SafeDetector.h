/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/SafeDetector.h
 */

#ifndef RAM_SAFE_DETECTOR_H_07_17_2008
#define RAM_SAFE_DETECTOR_H_07_17_2008

// STD Includes
#include <list>

// Project Includes
#include "core/include/ConfigNode.h"
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT SafeDetector : public Detector
{
  public:
    SafeDetector(core::ConfigNode config,
                core::EventHubPtr eventHub = core::EventHubPtr());
    ~SafeDetector();

    void processImage(Image* input, Image* output= 0);
    
    bool found();

    /** X cord of the bin closest to the center of the screen */
    double getX();
    
    /** Y cord of the bin closest to the center of the screen */
    double getY();

    bool blobsAreClose(BlobDetector::Blob b1, BlobDetector::Blob b2, double growThresh);
  private:
    void init(core::ConfigNode config);
    double m_safeX;
    double m_safeY;
    
    double m_rOverGMin;
    double m_rOverGMax;
    double m_bOverRMax;
    
    BlobDetector::Blob m_safeBlob;
    bool m_found;
    Image* m_working;
    BlobDetector blobDetector;
};

} // namespace vision
} // namespace ram

#endif // RAM_BIN_DETECTOR_H_06_23_2007
