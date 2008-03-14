/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestRedLightDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/RedLightDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"

#include "core/include/EventHub.h"

using namespace ram;

void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B)
{
    int size = image->getWidth() * image->getHeight() * 3;
    unsigned char* data = image->getData();
    for (int i = 0; i < size; i += 3)
    {
        // BGR
        data[i] = B;
        data[i + 1]  = G;
        data[i + 2]  = R;
    }
}

void drawRedCircle(vision::Image* image, int x, int y, int radius = 50)
{
    CvPoint center;
    center.x = x;
    center.y = y;
    cvCircle(image->asIplImage(), center, radius, CV_RGB(255, 0, 0), -1);
}

struct RedLightDetectorFixture
{
    RedLightDetectorFixture() :
        found(false),
        almostHit(false),
        event(vision::RedLightEventPtr()),
        input(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
        eventHub->subscribeToType(vision::EventType::LIGHT_FOUND,
            boost::bind(&RedLightDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::LIGHT_LOST,
            boost::bind(&RedLightDetectorFixture::lostHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::LIGHT_ALMOST_HIT,
            boost::bind(&RedLightDetectorFixture::almostHitHandler, this, _1));
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::RedLightEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::RedLightEventPtr();
    }

    void almostHitHandler(core::EventPtr event)
    {
        almostHit = true;
    }
    
    bool found;
    bool almostHit;
    vision::RedLightEventPtr event;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::RedLightDetector detector;
};

SUITE(RedLightDetector) {
    
TEST_FIXTURE(RedLightDetectorFixture, CenterLight)
{
    // Blue Image with red circle in the center
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/2, 240);

    // Process it
    detector.processImage(&input);

    double expectedX = 0 * 640.0/480.0;
    double expectedY = 0;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);

    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
    CHECK_CLOSE(3, event->range, 0.1);
    CHECK_CLOSE(math::Degree(0), event->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(0), event->elevation, math::Degree(0.4));

    // Make sure we haven't "almost hit" the light
    CHECK(false == almostHit);
}

TEST_FIXTURE(RedLightDetectorFixture, UpperLeft)
{
    // Blue Image with red circle in upper left (remeber image rotated 90 deg)
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640 - (640/4), 480/4);
    
    detector.processImage(&input);

    double expectedX = -0.5 * 640.0/480.0;
    double expectedY = 0.5;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);

    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
    CHECK_CLOSE(3, event->range, 0.1);
    CHECK_CLOSE(math::Degree(78.0/4), event->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(105.0/4), event->elevation, math::Degree(0.4));

    // Make sure we haven't "almost hit" the light
    CHECK(false == almostHit);
}

TEST_FIXTURE(RedLightDetectorFixture, LowerRight)
{
    // Blue Image with red circle in lower right (remeber image rotated 90 deg)
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/4, 480/4 * 3);
    
    detector.processImage(&input);

    double expectedX = 0.5 * 640.0/480.0;
    double expectedY = -0.5;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);

    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
    CHECK_CLOSE(3, event->range, 0.1);
    CHECK_CLOSE(math::Degree(-78.0/4), event->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(-105.0/4), event->elevation, math::Degree(0.4));

    // Make sure we haven't "almost hit" the light
    CHECK(false == almostHit);
}

TEST_FIXTURE(RedLightDetectorFixture, Events_LIGHT_LOST)
{
    // No light at all
    makeColor(&input, 0, 0, 255);
    
    detector.processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Now we found the light
    drawRedCircle(&input, 640 - (640/4), 480/4);
    detector.processImage(&input);
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(-0.5 * 640.0/480.0, event->x, 0.005);
    CHECK_CLOSE(0.5, event->y, 0.005);

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

TEST_FIXTURE(RedLightDetectorFixture, Events_LIGHT_ALMOST_HIT)
{
    // Blue Image with red circle in the center
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/2, 240, 140);

    // Process it
    detector.processImage(&input);

    double expectedX = 0 * 640.0/480.0;
    double expectedY = 0;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);

    // Check the events

    // LIGHT_ALMOST_HIT
    CHECK(almostHit);
    
    // LIGHT_FOUND
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
    CHECK_CLOSE(1.058, event->range, 0.1);
    CHECK_CLOSE(math::Degree(0), event->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(0), event->elevation, math::Degree(0.4));
}

TEST_FIXTURE(RedLightDetectorFixture, RemoveTop)
{
    // Blue Image with red circle in upper center (remeber image rotated 90 deg)
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640 - 60, 480/2);

    // Check with a non top removed detector
    detector.processImage(&input);
    double expectedX = 0  * 640.0/480.0;
    double expectedY = 0.815;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);

    // Create a detector which remove the top of the window
    vision::RedLightDetector detectorTopRemoved(
        core::ConfigNode::fromString("{ 'topRemovePercentage' : 0.25 }"));

    vision::OpenCVImage out(480, 640);
    detectorTopRemoved.processImage(&input, &out);
    CHECK(false == detectorTopRemoved.found);
}

} // SUITE(RedLightDetector)
