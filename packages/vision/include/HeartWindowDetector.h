/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/include/HeartWindowDetector.h
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

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT HeartWindowDetector : public Detector
{
  public:
    HeartWindowDetector(core::ConfigNode config,
                   core::EventHubPtr eventHub = core::EventHubPtr());
    HeartWindowDetector(Camera* camera);
    ~HeartWindowDetector();

    void update();
    void processImage(Image* input, Image* output = 0);

    void show(char* window);
    IplImage* getAnalyzedImage();
    
  private:
    void init(core::ConfigNode config);

    /* Normal processing to find one blob/color */
    bool processColor(Image* input, Image* output,
                      ColorFilter& filter,
                      BlobDetector::Blob& outputBlob);
    
    /* Process current state, and publishes TARGET_FOUND event */
    void publishFoundEvent(const BlobDetector::Blob& blob,
                           Color::ColorType color);

    void publishLostEvent(Color::ColorType color);

    void drawDebugCircle(BlobDetector::Blob blob,
                         Image* output);

    Camera *cam;
    
    /** Stores the various color filters */
    ColorFilter *m_redFilter;
    ColorFilter *m_blueFilter;

    /** Blob detector */
    BlobDetector m_blobDetector;

    Image *frame;
    Image *redFrame;
    Image *blueFrame;
    Image *processingFrame;

    bool m_redFound;
    bool m_blueFound;

    /* Configuration variables */
    double m_maxAspectRatio;
    double m_minAspectRatio;

    int m_minWidth;
    int m_minHeight;

    double m_minRelInnerWidth;
    double m_minRelInnerHeight;

    int m_centerXDisagreement;
    int m_centerYDisagreement;

    double m_minPixelPercentage;
    double m_maxPixelPercentage;

    double m_innerMinPixelPercentage;

    int m_erodeIterations;
    int m_dilateIterations;

    double m_physicalWidthMeters;

    int m_debug;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_WINDOW_DETECTOR_H_06_01_2010
