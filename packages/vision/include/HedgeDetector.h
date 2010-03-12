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
#include "vision/include/BlobDetector.h"
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

    bool found;

    /** Center of the hedge in the local horizontal, -4/3 to 4/3 */
    double getX();

    /** Center of the red light in the local vertical, -1 to 1 */
    double getY();

    /** The percent of the top of the image blacked out */
    void setTopRemovePercentage(double percent);
    /** The percent of the bottom of the image blacked out */
    void setBottomRemovePercentage(double percent);

    /** Set whether or not to use the LUV filter */
    void setUseLUVFilter(bool value);

    void show(char* window);
    IplImage* getAnalyzedImage();

  private:
    void init(core::ConfigNode config);

    /** Filter for green values */
    void filterForGreen(IplImage* image, IplImage* flashFrame);

    void filterForGreenLUV(IplImage* image);

    // Process current state, and publish the HEDGE_FOUND event
    void publishFoundEvent();

    /** Processes the list of all found blobs and finds the largest valid one */
    bool processBlobs(const BlobDetector::BlobList& blobs,
                      BlobDetector::Blob& outBlob);

    double m_hedgeCenterX;
    double m_hedgeCenterY;
    double m_hedgeWidth;
    int minGreenPixels;
    IplImage* image;
    IplImage* raw;
    IplImage* flashFrame;
    IplImage* saveFrame;
    CvPoint hedgeCenter;

    Image* frame;
    Camera* cam;

    /** Finds the hedge */
    BlobDetector m_blobDetector;

    /** Initial level of minimum green pixels (and lower bound) */
    int m_initialMinGreenPixels;

    /** Percentage of the image to remove from the top */
    double m_topRemovePercentage;

    /** Percentage of the image to remove from the bottom */
    double m_bottomRemovePercentage;

    /** Percentage of the image to remove from the left */
    double m_leftRemovePercentage;

    /** Percentage of the image to remove from the right */
    double m_rightRemovePercentage;

    /** Filters for green */
    ColorFilter* m_filter;

    /** Whether or not to use the newer LUV color filter */
    bool m_useLUVFilter;
};

} // namespace vision
} // namespace ram

#endif // RAM_HEDGE_DETECTOR_H_03_09_2010
