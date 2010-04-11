/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/LineDetector.h
 */

#ifndef RAM_VISION_LINEDETECTOR_H_03_30_2010
#define RAM_VISION_LINEDETECTOR_H_03_30_2010

// STD Includes
#include <cmath>
#include <vector>

// Library Includes
#include "cv.h"
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"

#include "core/include/ConfigNode.h"

#include "math/include/Math.h"
#include "math/include/Vector2.h"

// Must be included last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/** Finds all connected white regions in a black and white RGB image
 *
 *  As it finds all connected components it records their max and min X,Y
 *  bounds, and total white pixel counts. This is returned as a list of Blob
 *  objects ordered from largest to smallest.
 */  
class RAM_EXPORT LineDetector  : public Detector
{
public:

    /** A single set of connected white pixels in the image
     *
     *  Stores min/max X and Y bounds of the blob, its center, and pixel count.
     */
    class Line
    {
    public:
        Line(CvPoint pt1, CvPoint pt2, math::Radian theta, double rho) :
            m_pt1(pt1),
            m_pt2(pt2),
            m_theta(theta),
            m_rho(rho) { init(); }
        Line() : m_theta(0), m_rho(0) { init(); }

        double length() const { return m_line.length(); }
        double squaredLength() const { return m_line.squaredLength(); }

        int getCenterX() const { return (m_pt1.x + m_pt2.x) / 2; }
        int getCenterY() const { return (m_pt1.y + m_pt2.y) / 2; }
        
        int getMaxX() const { return (m_pt1.x > m_pt2.x) ? m_pt1.x : m_pt2.x; }
        int getMinX() const { return (m_pt1.x < m_pt2.x) ? m_pt1.x : m_pt2.x; }
        
        int getMaxY() const { return (m_pt1.y > m_pt2.y) ? m_pt1.y : m_pt2.y; }
        int getMinY() const { return (m_pt1.y > m_pt2.y) ? m_pt1.y : m_pt2.y;; }

        int getHeight() const { return abs(m_pt1.y - m_pt2.y) + 1; }

        int getWidth() const { return abs(m_pt1.x - m_pt2.x) + 1; }

        CvPoint point1() const { return m_pt1; }
        CvPoint point2() const { return m_pt2; }
        math::Radian theta() const { return m_theta; }
        double rho() const { return m_rho; }

    private:
        void init()
        {
            m_line = math::Vector2(m_pt2.x, m_pt2.y) -
                     math::Vector2(m_pt1.x, m_pt1.y);
        }

        CvPoint m_pt1;
        CvPoint m_pt2;
        math::Vector2 m_line;
        math::Radian m_theta;
        double m_rho;
    };
    
    class LineComparer
    {
        public:
            static bool compare(Line b1, Line b2)
            {
                math::Radian theta = b2.theta() - b1.theta();
                if (theta == math::Radian(0)) {
                    return b2.rho() - b1.rho();
                } else {
                    return theta < math::Radian(0);
                }
            }
    };
    
    LineDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    LineDetector(int minimumLineSize = 0);
    ~LineDetector();
    
    void processImage(Image* input, Image* output= 0);
    
    bool found();

    /** Sets the minimum number of pixels a blob must be to be reported */
    void setMinimumLineSize(int pixels);

    /** Returns the minimum number of pixels a blob must to be reported */
    int getMinimumLineSize();

    /** Returns all blobs bigger then minimum blob size, sorted large->small */
    std::vector<Line> getLines();

    typedef std::vector<Line> LineList;
  private:
    /** Initializes the class */
    void init(core::ConfigNode config);

    /** Runs edge filter on the image and stores it in data */
    void edgeFilter(IplImage* img);

    /** Takes a binary image and finds the lines. Returns number found */
    int houghTransform();

    LineList m_lines;

    /** Minimum line length and gap between lines */
    double m_minLineSize;
    double m_maxGapLength;

    /** Properties */
    double m_highThreshold;
    double m_lowThreshold;
    int m_houghThreshold;
    int m_maxLines;
    double m_rhoGap;
    math::Radian m_thetaGap;
    int m_squareGap;

    /** "Image" used during internal processing */
    Image* data;
    
    /** Number of pixels represented in the data array */
    size_t m_dataSize;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_LINEDETECTOR_H_06_19_2008
