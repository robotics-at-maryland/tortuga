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

#include "vision/test/include/Utility.h"

using namespace ram;

struct OrangePipeDetectorFixture
{
    OrangePipeDetectorFixture() :
        found(false),
        centered(false),
        event(vision::PipeEventPtr()),
        input(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
        // Subscribe to events like so
        eventHub->subscribeToType(vision::EventType::PIPE_FOUND,
            boost::bind(&OrangePipeDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::PIPE_CENTERED,
            boost::bind(&OrangePipeDetectorFixture::centeredHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::PIPE_LOST,
            boost::bind(&OrangePipeDetectorFixture::lostHandler, this, _1));
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::PipeEvent>(event_);
    }

    void centeredHandler(core::EventPtr event_)
    {
        centered = true;
        event = boost::dynamic_pointer_cast<vision::PipeEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::PipeEventPtr();
    }
    
    bool found;
    bool centered;
    vision::PipeEventPtr event;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::OrangePipeDetector detector;
};

SUITE(OrangePipeDetector) {

// TODO Test upright, and onside angle
TEST_FIXTURE(OrangePipeDetectorFixture, UpperLeft)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw orange square (upper left, remember image rotated 90 deg)
    drawSquare(&input, 640/4, 480/4, 50, 230, 25, CV_RGB(230,180,40));

    // Process it
    detector.processImage(&input);
    
    double expectedX = -0.5 * 640.0/480.0;
    double expectedY = 0.5;
    math::Degree expectedAngle(25);
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
    CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
}

TEST_FIXTURE(OrangePipeDetectorFixture, Left)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw orange square (left side)
    drawSquare(&input, 640/4, 480/2, 50, 230, 0, CV_RGB(230,180,40));

    // Process it
    detector.processImage(&input);
    
    double expectedX = -0.5 * 640.0/480.0; 
    double expectedY = 0;
    math::Degree expectedAngle(0);
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
    CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
}

TEST_FIXTURE(OrangePipeDetectorFixture, LowerRight)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw orange square (upper left)
    drawSquare(&input, 640 - 640/4, 480/4 * 3,
               50, 230, -25, CV_RGB(230,180,40));

    // Process it
    detector.processImage(&input);
    
    double expectedX = 0.5 * 640.0/480.0; 
    double expectedY = -0.5;
    math::Degree expectedAngle(-25);
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
    CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
}

TEST_FIXTURE(OrangePipeDetectorFixture, CenterUp)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw orange square in center sideways
    drawSquare(&input, 640/2, 480/2, 50, 230, 0, CV_RGB(230,180,40));

    // Process it
    detector.processImage(&input);

    double expectedX = 0;
    double expectedY = 0 * 640.0/480.0; 
    math::Degree expectedAngle(0);
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
    CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
}

TEST_FIXTURE(OrangePipeDetectorFixture, CenterSideways)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw orange square in center sideways
    drawSquare(&input, 640/2, 480/2, 50, 230, 90, CV_RGB(230,180,40));
    
    // Process it
    detector.processImage(&input);
    
    double expectedX = 0;
    double expectedY = 0 * 640.0/480.0; 
    math::Degree expectedAngle(-90);
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
    //CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
    //CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
}

TEST_FIXTURE(OrangePipeDetectorFixture, UpperLeftNoHough)
{
    // Disable hough to use blob based system instead
    detector.setHough(true);

    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw orange square (upper left, remember image rotated 90 deg)
    drawSquare(&input, 640/4, 480/4, 50, 230, 25, CV_RGB(230,180,40));

    // Process it
    detector.processImage(&input);
    
    double expectedX = -0.5 * 640.0/480.0;
    double expectedY = 0.5;
    math::Degree expectedAngle(25);
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(1));

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
    CHECK_CLOSE(expectedAngle, event->angle, math::Degree(1));
}

TEST_FIXTURE(OrangePipeDetectorFixture, Events_PIPE_LOST)
{
    // No light at all
    makeColor(&input, 0, 0, 255);
    
    detector.processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Now we found the pipe (lower right location)
    drawSquare(&input, 640 - 640/4, 480/4 * 3,
               50, 230, -25, CV_RGB(230,180,40));
    detector.processImage(&input);
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(0.5 * 640.0/480.0, event->x, 0.05);
    CHECK_CLOSE(-0.5, event->y, 0.05);

    // Now we lost the light
    makeColor(&input, 0, 0, 255);
    detector.processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Make sure we don't get another lost event
    found = true;
    detector.processImage(&input);
    CHECK(found == true);
}

TEST_FIXTURE(OrangePipeDetectorFixture, Events_PIPE_CENTERED)
{
    // Pipe in the lower right
    makeColor(&input, 0, 0, 255);
    drawSquare(&input, 640 - 640/4, 480/4 * 3,
               50, 230, 0, CV_RGB(230,180,40));
    detector.processImage(&input);
    CHECK(found);
    CHECK(event);
    CHECK(!centered);
    CHECK_CLOSE(0.5 * 640.0/480.0, event->x, 0.05);
    CHECK_CLOSE(-0.5, event->y, 0.05);    

    // Now pipe is dead center
    makeColor(&input, 0, 0, 255);
    drawSquare(&input, 640/2, 480/2, 50, 230, 0, CV_RGB(230,180,40));    
    detector.processImage(&input);
    CHECK(found);

    // Make sure we get the centered event
    CHECK(centered);
    CHECK(event);
    CHECK_CLOSE(0, event->x, 0.05);
    CHECK_CLOSE(0, event->y, 0.05);
}

} // SUITE(OrangePipeDetector)
