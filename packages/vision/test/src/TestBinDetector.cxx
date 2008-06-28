
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
#include "vision/include/BinDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"

#include "core/include/EventHub.h"

#include "vision/test/include/Utility.h"

using namespace ram;

struct BinDetectorFixture
{
    BinDetectorFixture() :
        found(false),
        centered(false),
        event(vision::BinEventPtr()),
        input(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
        // Subscribe to events like so
        eventHub->subscribeToType(vision::EventType::BIN_FOUND,
            boost::bind(&BinDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BIN_CENTERED,
            boost::bind(&BinDetectorFixture::centeredHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BIN_LOST,
            boost::bind(&BinDetectorFixture::lostHandler, this, _1));
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::BinEvent>(event_);
    }

    void centeredHandler(core::EventPtr event_)
    {
        centered = true;
        event = boost::dynamic_pointer_cast<vision::BinEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::BinEventPtr();
    }
    
    bool found;
    bool centered;
    vision::BinEventPtr event;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::BinDetector detector;
};

SUITE(BinDetector) {

// TODO Test upright, and onside angle

TEST_FIXTURE(BinDetectorFixture, UpperLeft)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin (upper left, remember image rotated 90 deg)
    drawBin(&input, 640 - (640/4), 480/4, 130, 25);

    // Process it
    vision::OpenCVImage output(640, 480);
    detector.processImage(&input, &output);
    
    double expectedX = -0.5;
    double expectedY = 0.5 * 640.0/480.0;
    math::Degree expectedAngle(25);
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, Left)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin (left, remember image rotated 90 deg)
    drawBin(&input, 640/2, 480/4, 130, 0);

    // Process it
    detector.processImage(&input);
    
    double expectedX = -0.5; 
    double expectedY = 0;
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, LowerRight)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin (upper left, remember image rotated 90 deg)
    drawBin(&input, 640/4, 480/4 * 3, 130, -25);

    // Process it
    detector.processImage(&input);
    
    double expectedX = 0.5;
    double expectedY = -0.5 * 640.0/480.0; 
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, CenterUp)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin in center sideways
    drawBin(&input, 640/2, 480/2, 130, 0);

    // Process it
    detector.processImage(&input);

    double expectedX = 0;
    double expectedY = 0 * 640.0/480.0; 
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, CenterSideways)
{
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin in center sideways
    drawBin(&input, 640/2, 480/2, 130, 90);
    
    // Process it
    detector.processImage(&input);
    
    double expectedX = 0;
    double expectedY = 0 * 640.0/480.0; 
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, Events_BIN_LOST)
{
    // No light at all
    makeColor(&input, 0, 0, 255);
    
    detector.processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Now we found the bin (lower right location)
    drawBin(&input, 640/4, 480/4 * 3, 130, -25);
    detector.processImage(&input);
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(0.5, event->x, 0.05);
    CHECK_CLOSE(-0.5 * 640.0/480.0, event->y, 0.05);

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

TEST_FIXTURE(BinDetectorFixture, Events_BIN_CENTERED)
{
    // Bin in the lower right
    makeColor(&input, 0, 0, 255);
    drawBin(&input, 640/4, 480/4 * 3, 130, 0);
    detector.processImage(&input);
    CHECK(found);
    CHECK(event);
    CHECK(!centered);
    CHECK_CLOSE(0.5, event->x, 0.05);
    CHECK_CLOSE(-0.5 * 640.0/480.0, event->y, 0.05);    

    // Now bin is dead center
    makeColor(&input, 0, 0, 255);
    drawBin(&input, 640/2, 480/2, 130, 0);
    detector.processImage(&input);
    CHECK(found);

    // Make sure we get the centered event
    CHECK(centered);
    CHECK(event);
    CHECK_CLOSE(0, event->x, 0.05);
    CHECK_CLOSE(0, event->y, 0.05);
}

/*TEST_FIXTURE(BinDetectorFixture, Suit)
{
    // Bin in the lower right
    makeColor(&input, 0, 0, 255);
    drawBin(&input, 640/4, 480/4 * 3, 130, 0, vision::Heart);
    vision::Image::showImage(&input);
    }*/

} // SUITE(BinDetector)
