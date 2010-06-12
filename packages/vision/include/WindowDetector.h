/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/WindowDetector.h
 */

#ifndef RAM_BUOY_DETECTOR_H_06_01_2010
#define RAM_BUOY_DETECTOR_H_06_01_2010

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

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT WindowDetector : public Detector
{
  private:
    struct Area {
        size_t minX;
        size_t maxX;
        size_t minY;
        size_t maxY;
        size_t pixels;
        
        Area() :
            minX(0),
            maxX(0),
            minY(0),
            maxY(0),
            pixels(0)
        {
        }
    };

    typedef std::map<CvScalar, Area, bool(*)(CvScalar,CvScalar)> ScalarMap;

  public:
    WindowDetector(core::ConfigNode config,
                   core::EventHubPtr eventHub = core::EventHubPtr());
    WindowDetector(Camera* camera);
    ~WindowDetector();

    void update();
    void processImage(Image* input, Image* output = 0);

    void show(char* window);
    IplImage* getAnalyzedImage();
    
  private:
    void init(core::ConfigNode config);

    // Filters the blobs for possible matches
    void filterBlobs(ScalarMap& blobMap, BlobDetector::BlobList& blobs);

    // Find closest blob
    void closestBlob(BlobDetector::BlobList& blobs,
                     const BlobDetector::Blob& oldBlob,
                     BlobDetector::Blob **outBlob);

    // Help functions for extrapolatePositions
    void extrapolate3(BlobDetector::BlobList& blobs,
                      BlobDetector::Blob *topLeft,
                      BlobDetector::Blob *topRight,
                      BlobDetector::Blob *bottomLeft,
                      BlobDetector::Blob *bottomRight);

    void extrapolatePositions(BlobDetector::BlobList& blobs,
                              BlobDetector::Blob *topLeft,
                              BlobDetector::Blob *topRight,
                              BlobDetector::Blob *bottomLeft,
                              BlobDetector::Blob *bottomRight);
    
    // Process current state, and publishes TARGET_FOUND event
    void publishFoundEvent(const BlobDetector::Blob& blob,
                           Color::ColorType color);

    void publishLostEvent(Color::ColorType color);

    Camera *cam;
    
    /** Stores the segmentation filter */
    SegmentationFilter *m_filter;

    /** Blob detector */
    BlobDetector m_blobDetector;

    Image *frame;
    Image *filtered;
    Image *binary;

    // Configuration variables
    double m_maxAspectRatio;
    double m_minAspectRatio;

    int m_minWidth;
    int m_minHeight;

    double m_minPixelPercentage;

    double m_maxDistance;

    // Previous window positions
    bool m_topLeftFound;
    BlobDetector::Blob m_topLeftWindow;
    
    bool m_bottomLeftFound;
    BlobDetector::Blob m_bottomLeftWindow;

    bool m_topRightFound;
    BlobDetector::Blob m_topRightWindow;

    bool m_bottomRightFound;
    BlobDetector::Blob m_bottomRightWindow;

    int m_debug;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_WINDOW_DETECTOR_H_06_01_2010
