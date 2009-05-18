/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestBarbedWireDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/BarbedWireDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/test/include/Utility.h"

#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"


using namespace ram;

struct BarbedWireDetectorFixture
{
    BarbedWireDetectorFixture() :
        found(false),
        event(vision::BarbedWireEventPtr()),
        input(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
        eventHub->subscribeToType(vision::EventType::BARBED_WIRE_FOUND,
            boost::bind(&BarbedWireDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BARBED_WIRE_LOST,
            boost::bind(&BarbedWireDetectorFixture::lostHandler, this, _1));
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::BarbedWireEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::BarbedWireEventPtr();
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
    
    bool found;
    vision::BarbedWireEventPtr event;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::BarbedWireDetector detector;
};

SUITE(BarbedWireDetector) {
    
TEST_FIXTURE(BarbedWireDetectorFixture, SinglePipe)
{
    // Blue Image with green pipe in the center (horizontal)
    makeColor(&input, 120, 120, 255);
    drawSquare(&input, 320, 240, 200, 200/31, 0, CV_RGB(0, 255, 0));

    // Process it
    processImage(&input);

    double expectedTopX = 0 * 640.0/480.0;
    double expectedTopY = 0;
    double expectedTopWidth = 200.0/640.0;

    double expectedBottomX = 0;
    double expectedBottomY = 0;
    double expectedBottomWidth = -1;
    
    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedTopX, event->topX, 0.005);
    CHECK_CLOSE(expectedTopY, event->topY, 0.005);
    CHECK_CLOSE(expectedTopWidth, event->topWidth, 0.005);
    CHECK_CLOSE(expectedBottomX, event->bottomX, 0.000001);
    CHECK_CLOSE(expectedBottomY, event->bottomY, 0.000001);
    CHECK_CLOSE(expectedBottomWidth, event->bottomWidth, 0.000001);
}

TEST_FIXTURE(BarbedWireDetectorFixture, DoublePipe)
{
    // Blue Image with green pipe in the center (horizontal)
    makeColor(&input, 120, 120, 255);
    drawSquare(&input, 320, 240, 400, 400/31, 0, CV_RGB(0, 255, 0));
    drawSquare(&input, 200, 480/4 * 3, 200, 200/31, 0, CV_RGB(0, 255, 0));

    // Process it
    processImage(&input);

    double expectedTopX = 0;
    double expectedTopY = 0;
    double expectedTopWidth = 400.0/640.0;

    double expectedBottomX = (-120.0/320.0) * 640.0/480.0;
    double expectedBottomY = -0.5;
    double expectedBottomWidth = 200.0/640.0;
    
    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedTopX, event->topX, 0.005);
    CHECK_CLOSE(expectedTopY, event->topY, 0.005);
    CHECK_CLOSE(expectedTopWidth, event->topWidth, 0.005);
    CHECK_CLOSE(expectedBottomX, event->bottomX, 0.000001);
    CHECK_CLOSE(expectedBottomY, event->bottomY, 0.000001);
    CHECK_CLOSE(expectedBottomWidth, event->bottomWidth, 0.000001);
}

TEST_FIXTURE(BarbedWireDetectorFixture, OddAngle)
{
    // Top pipe that is off angle, bottom pipe that is good
    makeColor(&input, 120, 120, 255);
    drawSquare(&input, 400, 240, 400, 400/31, 35, CV_RGB(0, 255, 0));
    drawSquare(&input, 400, 480/4 * 3, 300, 300/31, 0, CV_RGB(0, 255, 0));

    // Process it
    detector.getPropertySet()->getProperty("minAngle")->set(70.0);
    processImage(&input);

    double expectedTopX = 1.0/3.0;
    double expectedTopY = -0.5;
    double expectedTopWidth = 300.0/640.0;

    double expectedBottomX = 0;
    double expectedBottomY = 0;
    double expectedBottomWidth = -1;
    
    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedTopX, event->topX, 0.005);
    CHECK_CLOSE(expectedTopY, event->topY, 0.005);
    CHECK_CLOSE(expectedTopWidth, event->topWidth, 0.005);
    CHECK_CLOSE(expectedBottomX, event->bottomX, 0.000001);
    CHECK_CLOSE(expectedBottomY, event->bottomY, 0.000001);
    CHECK_CLOSE(expectedBottomWidth, event->bottomWidth, 0.000001);
}


/*

TEST_FIXTURE(BarbedWireDetectorFixture, UpperLeft)
{
    // Blue Image with green target in upper left
    makeColor(&input, 120, 120, 255);
    drawBarbedWire(&input, 640/4, 480/4);
    
    processImage(&input);

    double expectedX = -0.5 * 640.0/480.0;
    double expectedY = 0.5;
    double expectedRange = 1.0 - 100.0/480;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found());

    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
    CHECK_CLOSE(expectedRange, event->range, 0.005);
}

TEST_FIXTURE(BarbedWireDetectorFixture, LowerRight)
{
    // Blue Image with green target in lower right
    makeColor(&input, 120, 120, 255);
    drawBarbedWire(&input, 640 - 640/4, 480/4 * 3);
    
    processImage(&input);

    double expectedX = 0.5 * 640.0/480.0;
    double expectedY = -0.5;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found());

    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
}

TEST_FIXTURE(BarbedWireDetectorFixture, SquareNess)
{
    // Perfect
    makeColor(&input, 120, 120, 255);
    drawBarbedWire(&input, 640/2, 480/2, 100, 100);
    processImage(&input);
    
    double expectedSquareness = 1;
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedSquareness, event->squareNess, 0.005);
    
    // Skinny
    makeColor(&input, 120, 120, 255);
    drawBarbedWire(&input, 640/2, 480/2, 200, 100);
    processImage(&input);
    
    expectedSquareness = 0.5;
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedSquareness, event->squareNess, 0.005);


    // Skinnier
    makeColor(&input, 120, 120, 255);
    drawBarbedWire(&input, 640/2, 480/2, 300, 100);
    processImage(&input);
    
    expectedSquareness = 1.0/3.0;
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedSquareness, event->squareNess, 0.005);
    }*/


TEST_FIXTURE(BarbedWireDetectorFixture, Events_BARBED_WIRE_LOST)
{
    // No light at all
    makeColor(&input, 120, 120, 255);
    
    processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Now we found the light (Upper Left)
    drawSquare(&input, 640/4, 480/4, 300, 300/31, 0, CV_RGB(0, 255, 0));
    drawSquare(&input, 640/4, 480/2, 200, 200/31, 0, CV_RGB(0, 255, 0));

    processImage(&input);
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(-0.5 * 640.0/480.0, event->topX, 0.005);
    CHECK_CLOSE(0.5, event->topY, 0.005);
    CHECK_CLOSE(300.0/640.0, event->topWidth, 0.005);
    
    // Now we lost the barbed wire
    makeColor(&input, 120, 120, 255);
    processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Make sure we don't get another lost event
    found = true;
    processImage(&input);
    CHECK(found == true);
}

TEST_FIXTURE(BarbedWireDetectorFixture, RemoveTop)
{
    // Blue Image with red circle in upper center
    makeColor(&input, 120, 120, 255);
    drawSquare(&input, 640/2, 45, 300, 300/31, 0, CV_RGB(0, 255, 0));

    
    // Check with a non top removed detector
    processImage(&input);

    double expectedTopX = 0 * 640.0/480.0;
    double expectedTopY = 0.808;
    double expectedTopWidth = 300.0/640.0;

    double expectedBottomX = 0;
    double expectedBottomY = 0;
    double expectedBottomWidth = -1;
    
    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedTopX, event->topX, 0.005);
    CHECK_CLOSE(expectedTopY, event->topY, 0.005);
    CHECK_CLOSE(expectedTopWidth, event->topWidth, 0.005);
    CHECK_CLOSE(expectedBottomX, event->bottomX, 0.000001);
    CHECK_CLOSE(expectedBottomY, event->bottomY, 0.000001);
    CHECK_CLOSE(expectedBottomWidth, event->bottomWidth, 0.000001);

    // Create a detector which remove the top of the window
    vision::BarbedWireDetector detectorTopRemoved(
        core::ConfigNode::fromString("{ 'topRemovePercentage' : 0.25 }"));

    detectorTopRemoved.processImage(&input);
    CHECK(false == detectorTopRemoved.found());
}

TEST_FIXTURE(BarbedWireDetectorFixture, RemoveBottom)
{
    // Blue Image with red circle in upper center
    makeColor(&input, 120, 120, 255);
    drawSquare(&input, 640/2, 435, 500, 500/31, 0, CV_RGB(0, 255, 0));
    
    // Check with a non top removed detector
    processImage(&input);

    double expectedTopX = 0 * 640.0/480.0;
    double expectedTopY = -0.808;
    double expectedTopWidth = 500.0/640.0;

    double expectedBottomX = 0;
    double expectedBottomY = 0;
    double expectedBottomWidth = -1;
    
    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedTopX, event->topX, 0.005);
    CHECK_CLOSE(expectedTopY, event->topY, 0.005);
    CHECK_CLOSE(expectedTopWidth, event->topWidth, 0.005);
    CHECK_CLOSE(expectedBottomX, event->bottomX, 0.000001);
    CHECK_CLOSE(expectedBottomY, event->bottomY, 0.000001);
    CHECK_CLOSE(expectedBottomWidth, event->bottomWidth, 0.000001);

    // Create a detector which remove the top of the window
    vision::BarbedWireDetector detectorBottomRemoved(
        core::ConfigNode::fromString("{ 'bottomRemovePercentage' : 0.25 }"));

    detectorBottomRemoved.processImage(&input);
    CHECK(false == detectorBottomRemoved.found());
}
/*
TEST_FIXTURE(BarbedWireDetectorFixture, oddShapes)
{
    // Make sure we don't say a rectangle is a bouy
    makeColor(&input, 120, 120, 255);
    // Same area as normal circle just bad bounding box
    drawSquare(&input, 640/2, 480/2, 155, 55, 0, CV_RGB(255,0,0));

    processImage(&input);
    CHECK(false == detector.found());    

    // Now flip it up right
    makeColor(&input, 120, 120, 255);
    drawSquare(&input, 640/2, 480/2, 55, 155, 0, CV_RGB(255,0,0));

    processImage(&input);
    CHECK(false == detector.found());    

    // Now test the square *with* the light and make sure we get the light
    // (LowerRight position)
    makeColor(&input, 120, 120, 255);
    drawSquare(&input, 640/2, 480/2, 80, 240, 0, CV_RGB(255,0,0));
    drawBarbedWire(&input, 640 - 640/4, 480/4 * 3);

    processImage(&input);

    double expectedX = 0.5 * 640.0/480.0;
    double expectedY = -0.5;
    CHECK_CLOSE(expectedX, detector.getX(), 0.005);
    CHECK_CLOSE(expectedY, detector.getY(), 0.005);
    CHECK(detector.found());

    
    // Make sure we don't except a mostly empty blob
//    vision::BarbedWireDetector detectorTopRemoved(
//        core::ConfigNode::fromString("{ 'minFillPercentage' : 0.25 }"));
}*/

} // SUITE(BarbedWireDetector)
