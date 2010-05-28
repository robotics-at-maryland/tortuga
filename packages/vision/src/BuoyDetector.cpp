/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/BuoyDetector.cpp
 */

// STD Includes
#include <math.h>
#include <algorithm>
#include <iostream>

// Library Includes
#include "cv.h"
#include "highgui.h"

#include <boost/foreach.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

#include "vision/include/BuoyDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Color.h"
#include "vision/include/Events.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/SegmentationFilter.h"

namespace ram {
namespace vision {

BuoyDetector::BuoyDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_filter(0)
{
    init(config);
}

BuoyDetector::BuoyDetector(Camera* camera) :
    cam(camera),
    m_filter(0)
{
    init(core::ConfigNode::fromString("{}"));
}

void BuoyDetector::init(core::ConfigNode config)
{
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "houghParam1",
                         "The threshold to use for edges",
                         1, &m_param1, 1, 255);

    propSet->addProperty(config, false, "houghParam2",
                         "The threshold for a circle",
                         20, &m_param2, 1, 255);

    propSet->addProperty(config, false, "dp",
                         "Angle depth used for hough circle transformation",
                         1.0, &m_dp, 0.1, 10.0);

    propSet->addProperty(config, false, "min_dist",
                         "Minimum distance between two circle centers",
                         15, &m_min_dist, 0, 640*480);

    propSet->addProperty(config, false, "hough_min_radius",
                         "Minimum radius for hough circle transform",
                         0, &m_hough_min_radius);

    propSet->addProperty(config, false, "hough_max_radius",
                         "Maximum radius for hough circle transform",
                         0, &m_hough_max_radius);

    propSet->addProperty(config, false, "dilateIterations",
                         "Number of times to dilate the image in preprocessing",
                         10, &m_dilateIterations);

    propSet->addProperty(config, false, "minRadius",
                         "Minimum radius of a buoy (this is different "
                         "from the hough transform minimum!)",
                         0.0, &m_min_radius);

    propSet->addProperty(config, false, "minPercentage",
                         "Minimum percentage of circle completeness "
                         "to be considered a buoy",
                         .9, &m_min_percentage, 0.0, 1.0);

    propSet->addProperty(config, false, "almostHitRadius",
                         "Radius when the buoy is considered almost hit",
                         200.0, &m_almostHitRadius);

    propSet->addProperty(config, false, "redMin",
                         "Minimum value considered red",
                         0, &m_red_min, 0, 255);
    propSet->addProperty(config, false, "redMax",
                         "Maximum value considered red",
                         0, &m_red_max, 0, 255);

    propSet->addProperty(config, false, "greenMin",
                         "Minimum value considered green",
                         0, &m_green_min, 0, 255);
    propSet->addProperty(config, false, "greenMax",
                         "Maximum value considered green",
                         0, &m_green_max, 0, 255);

    propSet->addProperty(config, false, "yellowMin",
                         "Minimum value considered yellow",
                         0, &m_yellow_min, 0, 255);
    propSet->addProperty(config, false, "yellowMax",
                         "Maximum value considred yellow",
                         0, &m_yellow_max, 0, 255);

    m_filter = new SegmentationFilter(0.5, 500, 50);
    m_filter->addPropertiesToSet(propSet, &config);

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);
    
    // State machine variables 
    found = false;

    // Working images
    frame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    filtered = new OpenCVImage(640, 480, Image::PF_BGR_8);
    gray = new OpenCVImage(640, 480, Image::PF_GRAY_8);
    edges = new OpenCVImage(640, 480, Image::PF_GRAY_8);

    // Slightly deceptive, this gets converted to BGR automatically
    // because copyFrom copies the pixel format, then gets immediately
    // converted back to HSV. The default PixelFormat for this means nothing.
    hsv = new OpenCVImage(640, 480, Image::PF_HSV_8);
}

BuoyDetector::~BuoyDetector()
{
    delete m_filter;

    delete frame;
    delete filtered;
    delete gray;
    delete edges;
    delete hsv;
}

void BuoyDetector::update()
{
    cam->getImage(frame);
    processImage(frame);
}

void BuoyDetector::processImage(Image* input, Image* output)
{
    frame->copyFrom(input);
    filtered->copyFrom(frame);
    m_filter->filterImage(filtered);

    cvCvtColor(filtered->asIplImage(), gray->asIplImage(), CV_BGR2GRAY);
    cvCanny(gray->asIplImage(), edges->asIplImage(),
            m_param1, std::max(m_param1 / 2, 1));

    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *seq = cvHoughCircles(gray->asIplImage(), storage, CV_HOUGH_GRADIENT,
                                m_dp, m_min_dist, m_param1, m_param2,
                                m_hough_min_radius, m_hough_max_radius);

    cvDilate(edges->asIplImage(),edges->asIplImage(),NULL,m_dilateIterations);

    BuoyDetector::CircleList circleList;
    unsigned char* edgeData = edges->getData();
    for (int i=0; i < seq->total; i++) {
        float* p = (float *) cvGetSeqElem(seq, i);
        int acc = 0, total = 0;


        for (int j=0; j < 360; j++) {
            double theta = j * CV_PI / 180;
            int ptx = cvRound(p[2] * cos(theta) + p[0]);
            int pty = cvRound(p[2] * sin(theta) + p[1]);

            // Only look at valid points
            if (ptx >= 0 && ptx < (int) edges->getWidth() &&
                pty >= 0 && pty < (int) edges->getHeight()) {
                if (edgeData[ptx + pty * edges->getWidth()]) {
                    acc += 1;
                }
                total += 1;
            }
        }

        if (total > 0 && p[2] > m_min_radius) {
            circleList.push_back(
                Circle(p[0], p[1], p[2], acc / (float) total, m_buoyID++));
        }
    }

    std::sort(circleList.begin(), circleList.end(),
              BuoyDetector::CircleComparer::compare);

    // Remove the buoys under the minimum percentage of completeness
    while (circleList.size() > 0 &&
           circleList.back().getPercentage() < m_min_percentage)
    {
        circleList.pop_back();
    }

    // Match candidate buoys and match colors to them
    processCircles(circleList);

    // Get the set of the Ids of the newest pipes
    std::set<int> newIds;
    BOOST_FOREACH(BuoyDetector::Circle circle, m_circles)
        newIds.insert(circle.getId());

    // Find which ids were lost
    std::vector<int> lostIds(m_lastIds.size()); 
    std::vector<int>::iterator lostIdsEnd =
        std::set_difference(m_lastIds.begin(), m_lastIds.end(),
                            newIds.begin(), newIds.end(), lostIds.begin());
    lostIds.resize(lostIdsEnd - lostIds.begin());

    // Publish dropped event for all lost ids
    BOOST_FOREACH(int id, lostIds)
    {
        BuoyEventPtr event(new BuoyEvent(0, 0, Color::UNKNOWN));
        event->id = id;
        publish(EventType::BUOY_DROPPED, event);
    }

    m_lastIds = newIds;

    if (m_circles.size() > 0)
    {
        // Publish found events for all buoys, set found flag to true
        BOOST_FOREACH(BuoyDetector::Circle circle, m_circles)
        {
            double eventX, eventY;
            Detector::imageToAICoordinates(input, cvRound(circle.getX()),
                                           cvRound(circle.getY()), 
                                           eventX, eventY);

            BuoyEventPtr event(new BuoyEvent(eventX, eventY,
                                             circle.getColor()));
            event->azimuth = math::Degree(
                (78.0 / 2) * event->x * -1.0 *
                (double) input->getHeight()/input->getWidth());
            event->elevation = math::Degree((105.0 / 2) * event->y * 1);
            event->id = circle.getId();

            // Compute range (assume a sphere)
            double buoyRadius = 0.25; // feet
            event->range = (buoyRadius * input->getHeight()) /
                (circle.getRadius() * tan(78.0/2 * (M_PI/180)));

            publish(EventType::BUOY_FOUND, event);
        }
        found = true;
    }
    else
    {
        // Publish lost event if found flag is true and there are no buoys
        if (found) {
            core::EventPtr event(new core::Event());
            publish(EventType::BUOY_LOST, event);
        }
    }
        
    // Debug image
    if (output) {
        // Copy from the filtered image
        output->copyFrom(filtered);
        IplImage* debug = output->asIplImage();

        // Loop through the circles and draw a bounding box around them all
        // Color the box based on the color of the buoy, unknown is gray
        BOOST_FOREACH(BuoyDetector::Circle circle, m_circles)
        {
            CvPoint center;
            center.x = cvRound(circle.getX());
            center.y = cvRound(circle.getY());

            CvPoint UL, LR;
            UL.x = center.x - cvRound(circle.getRadius());
            UL.y = center.y - cvRound(circle.getRadius());
            LR.x = center.x + cvRound(circle.getRadius());
            LR.y = center.y + cvRound(circle.getRadius());

            CvScalar scalar;
            switch (circle.getColor()) {
            case Color::RED:
                scalar = cvScalar(0, 0, 255);
                break;
            case Color::GREEN:
                scalar = cvScalar(0, 255, 0);
                break;
            case Color::YELLOW:
                scalar = cvScalar(0, 255, 255);
                break;
            default:
                scalar = cvScalar(128, 128, 128);
                break;
            }

            // Small inner circle
            cvCircle(debug, center, 3, scalar, -1);
            // Large outer circle
            cvCircle(debug, center, cvRound(circle.getRadius()), scalar, 3);
            // Bounding box
            cvRectangle(debug, UL, LR, scalar, 3);
        }
    }
}

bool BuoyDetector::inrange(int min, int max, int value)
{
    if (min <= max) {
        return min <= value && value <= max;
    } else {
        return min <= value || value <= max;
    }
}

void BuoyDetector::show(char* window)
{
    Image *debug = new OpenCVImage(640, 480);

    // Reprocess the last frame with debug on
    // (We may be recalculating everything, but speed isn't an issue)
    processImage(frame, debug);

    cvNamedWindow(window);
    cvShowImage(window, debug->asIplImage());

    cvWaitKey(0);

    cvDestroyWindow(window);

    delete debug;
}

void BuoyDetector::publishFoundEvent(int x, int y, Color::ColorType color)
{
    RedLightEvent event(x, y, color);
    event.azimuth = 0;
    event.elevation = 0;
    event.range = 0;
}

void BuoyDetector::processCircles(BuoyDetector::CircleList& blobs)
{
    bool redFound = false, greenFound = false, yellowFound = false;

    TrackedBlob::updateIds(&m_circles, &blobs, m_sameBuoyThreshold);
    m_circles = blobs;

    hsv->copyFrom(frame);
    hsv->setPixelFormat(Image::PF_HSV_8);
    unsigned char* hueData = hsv->getData();

    // Do NOT use BOOST_FOREACH
    // BOOST_FOREACH makes a copy of the value, so when we change it,
    // the real object doesn't know that it should be changed.
    BuoyDetector::CircleListIter circle = m_circles.begin();
    BuoyDetector::CircleListIter end = m_circles.end();
    for ( ; circle != end; circle++)
    {
        double hue_hist[256] = { 0 };
        int radius = cvRound(circle->getRadius());
        int pixels = 0;
        
        assert(radius > 0 && "Radius must be positive");
        for (int row=-radius; row < radius+1; row++) {
            // Figure out the pixel span for this row
            double theta = asin(row / radius);
            int width = cvRound(radius * cos(theta));
            
            for (int col=-width; col < width; col++) {
                double increment = 0;
                int x = row + cvRound(circle->getX());
                int y = col + cvRound(circle->getY());
                
                if (x >= 0 && x < (int) hsv->getWidth() &&
                    y >= 0 && y < (int) hsv->getHeight()) {
                    int hue = hueData[(x + y * hsv->getWidth())*3];
                    hue_hist[hue] += 1 - increment;
                    pixels += 1;
                }
            }
        }
        
        // Find the peak in the histogram
        int peak = 0;
        for (int i=0; i < 256; i++) {
            if (hue_hist[i] > hue_hist[peak])
                peak = i;
        }
        
        // Check if it's red
        if (!redFound && inrange(m_red_min, m_red_max, peak)) {
            // Red color
            circle->setColor(Color::RED);
            redFound = true;
        } else if (!greenFound && inrange(m_green_min, m_green_max, peak)) {
            // Green color
            circle->setColor(Color::GREEN);
            greenFound = true;
        } else if (!yellowFound && inrange(m_yellow_min, m_yellow_max, peak)) {
            // Yellow color
            circle->setColor(Color::YELLOW);
            yellowFound = true;
        } else {
            // Unknown color
            circle->setColor(Color::UNKNOWN);
        }
    }
}

} // namespace vision
} // namespace ram
