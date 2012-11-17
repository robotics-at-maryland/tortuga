/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/include/CupidDetector.h
 */

#ifndef RAM_HEART_WINDOW_DETECTOR_H
#define RAM_HEART_WINDOW_DETECTOR_H

// STD Includes
#include <map>
#include <vector>

// Library Includes
#include "cv.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Color.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "core/include/ConfigNode.h"
#include "vision/include/TableColorFilter.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT CupidDetector : public Detector
{
  public:
    CupidDetector(core::ConfigNode config,
                   core::EventHubPtr eventHub = core::EventHubPtr());
    CupidDetector(Camera* camera);
    ~CupidDetector();

    void update();
    void processImage(Image* input, Image* output = 0);

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
                      BlobDetector::Blob& outputBlob);

    bool findHearts(Image* input,
                    ColorFilter& filter,
                    BlobDetector::Blob& windowBlob,
                    BlobDetector::Blob& smallHeart,
                    BlobDetector::Blob& largeHeart);
    
    /* Process current state, and publishes TARGET_FOUND event */
    void publishFoundEvent(const BlobDetector::Blob& blob,
                           Color::ColorType color);

    void publishLostEvent(Color::ColorType color);

    void drawDebugCircle(BlobDetector::Blob blob,
                         Image* output,
                         unsigned char R,
                         unsigned char G,
                         unsigned char B);

    Camera *cam;
    
    /** Stores the various color filters */
    ColorFilter *m_redFilter;
    ColorFilter *m_blueFilter;

    /** Color Filter Lookup Table */
    TableColorFilter *m_redTableColorFilter;
    TableColorFilter *m_blueTableColorFilter;

    /** Blob detector */
    BlobDetector m_blobDetector;

    Image *m_frame;
    Image *m_redFrame;
    Image *m_blueFrame;
    Image *m_processingFrame;
    Image *m_heartsFrame;

    bool m_redFound;
    bool m_blueFound;

    /* Configuration variables */
    double m_maxAspectRatio;
    double m_minAspectRatio;

    double m_minHeartAspectRatio;
    double m_maxHeartAspectRatio;

    // double m_minLargeHeartRelativeWidth;
    // double m_maxLargeHeartRelativeWidth;

    // double m_minSmallHeartRelativeWidth;
    // double m_maxSmallHeartRelativeWidth;

    double m_minHeartPixelPct;

    int m_minWidth;
    int m_minHeight;

    double m_minPixelPercentage;
    double m_maxPixelPercentage;

    int m_erodeIterations;
    int m_dilateIterations;

    double m_physicalWidthMeters;
    double m_physicalHeightMeters;

    int m_debug;

    bool m_colorFilterLookupTable;

    std::string m_redLookupTablePath;
    std::string m_blueLookupTablePath;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_WINDOW_DETECTOR_H_06_01_2010
