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
#include "highgui.h"

// Project Includes
#include "vision/include/HedgeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/test/include/Utility.h"

#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"


using namespace ram;

/**
 * Draws a hedge with a center of (x, y) and a width on screen from 0-1.
 *
 * Values passed in should not draw anything outside the field of view, as
 * this function does not check for boundary conditions.
 */
void drawHedge(vision::Image* input, int x, int y, double w, int thickness)
{
    // Get width in relation to 640x480 screen
    double width = w * 640;
    double height = width / 2.0;

    // Left vertical line
    vision::drawLine(input,
                     x-((int)width/2), y-((int)height/2),
                     x-((int)width/2), y+((int)height/2),
                     thickness, CV_RGB(0, 255, 0));

    // Right vertical line
    vision::drawLine(input,
                     x+((int)width/2), y-((int)height/2),
                     x+((int)width/2), y+((int)height/2),
                     thickness, CV_RGB(0, 255, 0));

    // Bottom horizontal line
    vision::drawLine(input,
                     x-((int)width/2), y+((int)height/2),
                     x+((int)width/2), y+((int)height/2),
                     thickness, CV_RGB(0, 255, 0));
}

static const std::string CONFIG =
    "{"
    " 'name' : 'HedgeDetector',"
    " 'filtGMin' : 150"
    "}";

struct HedgeDetectorFixture
{
    HedgeDetectorFixture() :
        found(false),
        event(vision::HedgeEventPtr()),
        input(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString(CONFIG), eventHub)
    {
        eventHub->subscribeToType(vision::EventType::HEDGE_FOUND,
            boost::bind(&HedgeDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::HEDGE_LOST,
            boost::bind(&HedgeDetectorFixture::lostHandler, this, _1));
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::HedgeEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::HedgeEventPtr();
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
    vision::HedgeEventPtr event;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::HedgeDetector detector;
};

SUITE(HedgeDetector) {
    
TEST_FIXTURE(HedgeDetectorFixture, Center)
{
    // Blue Image with green pipe in the center (horizontal)
    makeColor(&input, 120, 120, 255);
    drawHedge(&input, 320, 240, .5, 10);
    cvSaveImage("hedge.png", input.asIplImage());

    // Process it
    processImage(&input);

    double expectedX = 0 * 640.0/480.0;
    double expectedY = 0;
    double expectedWidth = 0.5;
    
    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
    CHECK_CLOSE(expectedWidth, event->width, 0.005);
}

TEST_FIXTURE(HedgeDetectorFixture, Left)
{
    // Blue Image with green pipe in the center (horizontal)
    makeColor(&input, 120, 120, 255);
    drawHedge(&input, 240, 240, .5, 3);

    // Process it
    processImage(&input);

    double expectedX = -0.1875 * 640.0/480.0;
    double expectedY = 0;
    double expectedWidth = 0.5;
    
    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.005);
    CHECK_CLOSE(expectedY, event->y, 0.005);
    CHECK_CLOSE(expectedWidth, event->width, 0.005);
}

} // SUITE(HedgeDetector)
