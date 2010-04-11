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

    bool found;

    /** Center of the hedge in the local horizontal, -4/3 to 4/3 */
    double getX();

    /** Center of the red light in the local vertical, -1 to 1 */
    double getY();

    /** The percent of the top of the image blacked out */
    void setTopRemovePercentage(double percent);
    /** The percent of the bottom of the image blacked out */
    void setBottomRemovePercentage(double percent);

    void show(char* window);
    Image* getAnalyzedImage();

  private:
    void init(core::ConfigNode config);

    // Process current state, and publish the HEDGE_FOUND event
    void publishFoundEvent();

    /** Processes the list of all found blobs and finds the largest valid one */
    bool processLines(const LineDetector::LineList& lines,
                      LineDetector::Line& outLine);

    double m_hedgeCenterX;
    double m_hedgeCenterY;
    double m_hedgeWidth;
    int minGreenPixels;
    OpenCVImage* raw;
    OpenCVImage* preprocess;
    OpenCVImage* filtered;
    OpenCVImage* analyzed;
    CvPoint hedgeCenter;

    Image* frame;
    Camera* cam;

    /** Finds the hedge */
    LineDetector m_lineDetector;

    /** Percentage of the image to remove from the top */
    double m_topRemovePercentage;

    /** Percentage of the image to remove from the bottom */
    double m_bottomRemovePercentage;

    /** Percentage of the image to remove from the left */
    double m_leftRemovePercentage;

    /** Percentage of the image to remove from the right */
    double m_rightRemovePercentage;

    /** Filters for green using LUV */
    ColorFilter* m_filter;

    /** Debug level */
    int m_debug;
};

} // namespace vision
} // namespace ram

#endif // RAM_HEDGE_DETECTOR_H_03_09_2010
