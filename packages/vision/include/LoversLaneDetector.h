/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/LoversLaneDetector.h
 */

#ifndef RAM_LOVERSLANE_DETECTOR_H_03_09_2010
#define RAM_LOVERSLANE_DETECTOR_H_03_09_2010

// STD Includes
#include <vector>

// Library Includes
#include "cv.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/LineDetector.h"
#include "core/include/ConfigNode.h"
#include "math/include/Math.h"
#include "vision/include/ImageFilter.h"
#include "vision/include/TableColorFilter.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT LoversLaneDetector : public Detector
{
  public:
    LoversLaneDetector(core::ConfigNode config,
                  core::EventHubPtr eventHub = core::EventHubPtr());
    LoversLaneDetector(Camera* camera);
    ~LoversLaneDetector();

    void update();
    void processImage(Image* input, Image* output = 0);

    bool m_found;

    void show(char* window);

    IplImage* getAnalyzedImage();

    // Setter and Getter for lookup table color filter
    bool getLookupTable();
    void setLookupTable(bool LookupTable);

  private:
    void init(core::ConfigNode config);

    /* Normal processing to find one blob/color */
    bool processColor(Image* input, Image* output,
                      ImageFilter& filter,
                      BlobDetector::Blob& leftBlob,
                      BlobDetector::Blob& rightBlob,
                      BlobDetector::Blob& outBlob);

    bool processSides(Image* input,
                      BlobDetector::Blob& fullBlob,
                      BlobDetector::Blob& leftBlob,
                      BlobDetector::Blob& rightBlob);

    // Process current state, and publish the LOVERSLANE_FOUND event
    void publishFoundEvent(BlobDetector::Blob& blob,
                           BlobDetector::Blob& leftBlob,
                           BlobDetector::Blob& rightBlob);

    void publishLostEvent();

    Camera* cam;

    /** Stores the various color filters */
    ColorFilter *m_colorFilter;

    /** Color Filter Lookup Table */
    TableColorFilter *m_tableColorFilter;

    /** Blob detector */
    BlobDetector m_blobDetector;
    BlobDetector m_lBlobDetector;
    BlobDetector m_rBlobDetector;

    Image *frame;
    Image *greenFrame;

    /* Configuration variables */
    double m_maxAspectRatio;
    double m_minAspectRatio;

    int m_minWidth;
    int m_minHeight;

    double m_minPixelPercentage;
    double m_maxPixelPercentage;
    double m_maxDistance;
    double m_poleThreshold;

    int m_erodeIterations;
    int m_dilateIterations;

    double m_physicalHeightMeters;
    int m_debug;

    bool m_colorFilterLookupTable;

    std::string m_lookupTablePath;
};

} // namespace vision
} // namespace ram

#endif // RAM_LOVERSLANE_DETECTOR_H_03_09_2010
