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
#include "vision/test/include/Utility.h"
#include "vision/include/VisionSystem.h"

#include "core/include/EventHub.h"

using namespace ram;

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

    void processImage(vision::Image* image, bool show = false)
    {
        if (show)
	{
	    vision::OpenCVImage input(640, 480);
	    input.copyFrom(image);
	    vision::Image::showImage(&input, "Input");

	    vision::OpenCVImage output(640, 480);
	    detector.processImage(image, &output);
	    vision::Image::showImage(&output, "Output");
	}
	else
        {
            detector.processImage(image);
	}
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
    processImage(&input);

    double expectedX = 0;
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
    // Blue Image with red circle in upper left
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/4, 480/4);
    
    processImage(&input);
    
    double expectedX = -0.5;
    double expectedY = 0.5;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);

    math::Degree xFOV = vision::VisionSystem::getFrontHorizontalFieldOfView();
    math::Degree yFOV = vision::VisionSystem::getFrontVerticalFieldOfView();

    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
    CHECK_CLOSE(3, event->range, 0.1);
    CHECK_CLOSE(math::Degree(xFOV/4), event->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(yFOV/4), event->elevation, math::Degree(0.4));

    // Make sure we haven't "almost hit" the light
    CHECK(false == almostHit);
}

/*TEST_FIXTURE(RedLightDetectorFixture, TestLUV)
{
    // Blue Image with red circle in lower right
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640 - 640/4, 480/4 * 3);

    detector.setUseLUVFilter(true);
    processImage(&input);

    double expectedX = 0.5 * 640.0/480.0;
    double expectedY = -0.5;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);
    }*/

TEST_FIXTURE(RedLightDetectorFixture, TestNOLUV)
{
    // Now without the LUV
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640 - 640/4, 480/4 * 3);
    
    detector.setUseLUVFilter(false);
    processImage(&input);

    double expectedX = 0.5;
    double expectedY = -0.5;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);
}

TEST_FIXTURE(RedLightDetectorFixture, Events_LIGHT_LOST)
{
    // No light at all
    makeColor(&input, 0, 0, 255);
    
    processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Now we found the light (Upper Left)
    drawRedCircle(&input, 640/4, 480/4);
    processImage(&input);
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(-0.5, event->x, 0.005);
    CHECK_CLOSE(0.5, event->y, 0.005);

    // Now we lost the light
    makeColor(&input, 0, 0, 255);
    processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Make sure we don't get another lost event
    found = true;
    processImage(&input);
    CHECK(found == true);
}

TEST_FIXTURE(RedLightDetectorFixture, Events_LIGHT_ALMOST_HIT)
{
    // Blue Image with red circle in the center
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/2, 240, 140);

    // Process it
    processImage(&input);

    double expectedX = 0;
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

TEST_FIXTURE(RedLightDetectorFixture, Events_LIGHT_ALMOST_HIT_TOPBOT)
{
    // Remove the top and bottom of the image
    detector.setBottomRemovePercentage(0.25);
    detector.setTopRemovePercentage(0.25);

    // Blue Image with red circle in the center
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/2, 240, 100);

    // Process it
    processImage(&input);

    double expectedX = 0;
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
    CHECK_CLOSE(1.48185, event->range, 0.1);
    CHECK_CLOSE(math::Degree(0), event->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(0), event->elevation, math::Degree(0.4));
}

TEST_FIXTURE(RedLightDetectorFixture, RemoveTop)
{
    // Blue Image with red circle in upper center
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/2, 45);

    // Check with a non top removed detector
    processImage(&input);
    double expectedX = 0;
    double expectedY = 0.808;
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

TEST_FIXTURE(RedLightDetectorFixture, RemoveBottom)
{
    // Blue Image with red circle in upper center
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/2, 435);

    // Check with a non top removed detector
    processImage(&input);
    double expectedX = 0;
    double expectedY = -0.808;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);

    // Create a detector which remove the top of the window
    vision::RedLightDetector detectorBottomRemoved(
        core::ConfigNode::fromString("{ 'bottomRemovePercentage' : 0.25 }"));

    vision::OpenCVImage out(480, 640);
    detectorBottomRemoved.processImage(&input, &out);
    CHECK(false == detectorBottomRemoved.found);
}

//TODO: Test remove left and right

TEST_FIXTURE(RedLightDetectorFixture, oddShapes)
{
    // Make sure we don't say a rectangle is a bouy
    makeColor(&input, 0, 0, 255);
    // Same area as normal circle just bad bounding box
    drawSquare(&input, 640/2, 480/2, 155, 55, 0, CV_RGB(255,0,0));

    processImage(&input);
    CHECK(false == detector.found);    

    // Now flip it up right
    makeColor(&input, 0, 0, 255);
    drawSquare(&input, 640/2, 480/2, 55, 155, 0, CV_RGB(255,0,0));

    processImage(&input);
    CHECK(false == detector.found);    

    // Now test the square *with* the light and make sure we get the light
    // (LowerRight position)
    makeColor(&input, 0, 0, 255);
    drawSquare(&input, 640/2, 480/2, 80, 240, 0, CV_RGB(255,0,0));
    drawRedCircle(&input, 640 - 640/4, 480/4 * 3);

    processImage(&input);

    double expectedX = 0.5;
    double expectedY = -0.5;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found);

    
    // Make sure we don't except a mostly empty blob
//    vision::RedLightDetector detectorTopRemoved(
//        core::ConfigNode::fromString("{ 'minFillPercentage' : 0.25 }"));
}

} // SUITE(RedLightDetector)
