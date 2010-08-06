/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/BuoyDetector.h
 */

#ifndef RAM_BUOY_DETECTOR_H_05_23_2010
#define RAM_BUOY_DETECTOR_H_05_23_2010

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

class RAM_EXPORT BuoyDetector : public Detector
{
  public:
    BuoyDetector(core::ConfigNode config,
                     core::EventHubPtr eventHub = core::EventHubPtr());
    BuoyDetector(Camera* camera);
    ~BuoyDetector();

    void update();
    void processImage(Image* input, Image* output = 0);

    void show(char* window);
    IplImage* getAnalyzedImage();

  private:
    void init(core::ConfigNode config);

    bool inrange(int min, int max, int value);

    /* Normal processing to find one blob/color */
    bool processColor(Image* input, Image* output, ColorFilter& filter,
                      BlobDetector::Blob& outBlob);

    /** Counts the number of white pixels in the sub area of the image */
    int countWhitePixels(Image* source,
                         int upperLeftX, int upperLeftY,
                         int lowerRightX, int lowerRightY);

    
    // Process current state, and publishes LIGHT_FOUND event
    void publishFoundEvent(BlobDetector::Blob& blob, Color::ColorType color);
    void publishLostEvent(Color::ColorType color);

    Camera *cam;

    /** State variables */
    bool m_redFound;
    bool m_greenFound;
    bool m_yellowFound;

    /** Color Filters */
    ColorFilter *m_redFilter;
    ColorFilter *m_greenFilter;
    ColorFilter *m_yellowFilter;
    ColorFilter *m_blackFilter;

    /** Blob Detector */
    BlobDetector m_blobDetector;
    
    /** Threshold for almost hitting the red light */
    double m_almostHitPercentage;

    /** Whether or not to check for black below the buoy */
    bool m_checkBlack;

    /** The precentage of the subwindow that must be black */
    double m_minBlackPercentage;

    /** The biggest fraction of the window for the black check */
    double m_maxTotalBlackCheckSize;

    /** Percentage of the image to remove from the top */
    double m_topRemovePercentage;

    /** Percentage of the image to remove from the bottom */
    double m_bottomRemovePercentage;

    /** Percentage of the image to remove from the left */
    double m_leftRemovePercentage;

    /** Percentage of the image to remove from the right */
    double m_rightRemovePercentage;


    /** Working Images */
    Image *frame;
    Image *redFrame;
    Image *greenFrame;
    Image *yellowFrame;
    Image *blackFrame;

    /* Configuration variables */
    double m_maxAspectRatio;
    double m_minAspectRatio;

    int m_minWidth;
    int m_minHeight;

    double m_minPixelPercentage;
    int m_minPixels;
    double m_maxDistance;

    int m_debug;


};
	
} // namespace vision
} // namespace ram

#endif // RAM_BUOY_DETECTOR_H_05_23_2010
