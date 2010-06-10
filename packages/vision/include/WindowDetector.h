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
#include <set>
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
    
    // Process current state, and publishes TARGET_FOUND event
    void publishFoundEvent(int x, int y, Color::ColorType color);

    Camera *cam;

    /** State variables */
    bool found;
    
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

    int m_debug;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_WINDOW_DETECTOR_H_06_01_2010
