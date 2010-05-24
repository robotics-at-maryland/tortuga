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
#include "vision/include/TrackedBlob.h"

#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

class RAM_EXPORT BuoyDetector : public Detector
{
  public:
    class Circle : public TrackedBlob
    {
    public:
        Circle(double x, double y, double r, double percentage, int id,
               Color::ColorType color = Color::UNKNOWN) :
            TrackedBlob(),
            m_x(x), m_y(y), m_r(r), m_percentage(percentage), m_color(color)
        {
            _setId(id);
        }

        double getX() const { return m_x; }
        double getY() const { return m_y; }
        double getRadius() const { return m_r; }
        double getPercentage() const { return m_percentage; }

        Color::ColorType getColor() const { return m_color; }
        void setColor(Color::ColorType color) { m_color = color; }

    private:
        double m_x;
        double m_y;
        double m_r;
        double m_percentage;
        Color::ColorType m_color;
    };

    class CircleComparer
    {
    public:
        static bool compare(Circle c1, Circle c2)
        {
            return c1.getPercentage() > c2.getPercentage();
        }
    };

    BuoyDetector(core::ConfigNode config,
                     core::EventHubPtr eventHub = core::EventHubPtr());
    BuoyDetector(Camera* camera);
    ~BuoyDetector();

    void update();
    void processImage(Image* input, Image* output = 0);

    void show(char* window);
    IplImage* getAnalyzedImage();

    typedef std::vector<Circle> CircleList;
    typedef CircleList::iterator CircleListIter;
    
  private:
    void init(core::ConfigNode config);

    bool inrange(int min, int max, int value);
    
    // Process current state, and publishes LIGHT_FOUND event
    void publishFoundEvent(int x, int y, Color::ColorType color);

    /** Processes the list of all found blobs and finds the larget valid one */
    void processCircles(BuoyDetector::CircleList& blobs);

    Camera *cam;

    /** State variables */
    bool found;
    std::set<int> m_lastIds;
    
    /** Stores the segmentation filter */
    SegmentationFilter *m_filter;

    CircleList m_circles;

    /** Threshold for almost hitting the red light */
    double m_almostHitRadius;

    Image *frame;
    Image *filtered;
    Image *gray;
    Image *edges;
    Image *hsv;

    // Hough circles configuration
    int m_param1;
    int m_param2;
    double m_dp;
    int m_min_dist;
    int m_hough_min_radius;
    int m_hough_max_radius;

    int m_dilateIterations;
    double m_min_radius;
    double m_min_percentage;

    // Color configuration values
    int m_red_min;
    int m_red_max;
    int m_green_min;
    int m_green_max;
    int m_yellow_min;
    int m_yellow_max;

    int m_buoyID;
    double m_sameBuoyThreshold;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_BUOY_DETECTOR_H_05_23_2010
