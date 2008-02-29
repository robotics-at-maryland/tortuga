/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/test/src/TestOrangePipeDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <cv.h>
// Project Includes
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"

#include "core/include/EventHub.h"

using namespace ram;

void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B);

void drawRedCircle(vision::Image* image, int x, int y, int radius = 50);

struct OrangePipeDetectorFixture
{
    OrangePipeDetectorFixture() :
        found(false),
        event(vision::PipeEventPtr()),
        input(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
        // Subscribe to events like so
        eventHub->subscribeToType(vision::EventType::PIPE_FOUND,
            boost::bind(&OrangePipeDetectorFixture::foundHandler, this, _1));
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::PipeEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::PipeEventPtr();
    }
    
    bool found;
    vision::PipeEventPtr event;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::OrangePipeDetector detector;
};

SUITE(OrangePipeDetector) {
    
TEST_FIXTURE(OrangePipeDetectorFixture, CenterLight)
{
    // Blue Image with orange rectangle in it
    makeColor(&input, 0, 0, 255);
    // draw orange square
    CvPoint pts[4];
    pts[0] = CvPoint();
    pts[0].x = 25;
    pts[0].y = 25;
    pts[1] = CvPoint();
    pts[1].x=25;
    pts[1].y=75;
    pts[2] = CvPoint();
    pts[2].x=255;
    pts[2].y=75;
    pts[3] = CvPoint();
    pts[3].x=255;
    pts[3].y=25;
    
    cvFillConvexPoly(input,pts,4,CV_RGB(230,180,40));
    

    vision::OpenCVImage tmp(480, 640);
    // Process it
    detector.processImage(&input, &tmp);
    vision::Image::saveToFile(&input, "in.png");
    vision::Image::saveToFile(&tmp, "out.png");
    
    double expectedX = 0;
    double expectedY = 0;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    //CHECK(detector.found);

    // Check the events
    //CHECK(found);
    //CHECK(event);
    //CHECK_CLOSE(expectedX, event->x, 0.005);
    //CHECK_CLOSE(expectedY, event->y, 0.005);
    //CHECK_CLOSE(3, event->range, 0.1);
    //CHECK_CLOSE(math::Degree(0), event->azimuth, math::Degree(0.4));
    //CHECK_CLOSE(math::Degree(0), event->elevation, math::Degree(0.4));
}

} // SUITE(OrangePipeDetector)
