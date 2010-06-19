/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/HedgeDetector.h
 */

#ifndef RAM_HEDGE_DETECTOR_H_03_09_2010
#define RAM_HEDGE_DETECTOR_H_03_09_2010

// STD Includes
#include <vector>

// Library Includes
#include "cv.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/LineDetector.h"
#include "core/include/ConfigNode.h"
#include "math/include/Math.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT HedgeDetector : public Detector
{
  public:
    HedgeDetector(core::ConfigNode config,
                  core::EventHubPtr eventHub = core::EventHubPtr());
    HedgeDetector(Camera* camera);
    ~HedgeDetector();

    void update();
    void processImage(Image* input, Image* output = 0);

    bool m_found;

    void show(char* window);

    IplImage* getAnalyzedImage();

  private:
    void init(core::ConfigNode config);

    /* Normal processing to find one blob/color */
    bool processColor(Image* input, Image* output,
                      ColorFilter& filter, BlobDetector::Blob& outBlob);

    // Process current state, and publish the HEDGE_FOUND event
    void publishFoundEvent(const BlobDetector::Blob& blob);

    void publishLostEvent();

    Camera* cam;

    /** Stores the various color filters */
    ColorFilter *m_colorFilter;

    /** Blob detector */
    BlobDetector m_blobDetector;

    Image *frame;
    Image *greenFrame;

    /* Configuration variables */
    double m_maxAspectRatio;
    double m_minAspectRatio;

    int m_minWidth;
    int m_minHeight;

    double m_minPixelPercentage;
    double m_maxDistance;

    int m_debug;
};

} // namespace vision
} // namespace ram

#endif // RAM_HEDGE_DETECTOR_H_03_09_2010
