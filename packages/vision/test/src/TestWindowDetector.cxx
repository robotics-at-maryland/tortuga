/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/test/src/TestWindowDetector.cxx
 */

// STD Includes
#include <iostream>
#include <vector>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include "cv.h"

// Project Includes
#define private public
#include "vision/include/WindowDetector.h"
#undef private
#include "vision/include/Color.h"

#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/test/include/Utility.h"

#include "core/include/EventHub.h"

using namespace ram;

static const std::string CONFIG =
    "{\n"
    "    'filtRedHMin' : 0,\n"
    "    'filtRedHMax' : 10,\n"
    "    'filtRedVMin' : 10,\n"
    "    'filtGreenHMin' : 60,\n"
    "    'filtGreenHMax' : 60,\n"
    "    'filtGreenVMin' : 10,\n"
    "    'filtYellowHMin' : 30,\n"
    "    'filtYellowHMax' : 30,\n"
    "    'filtYellowVMin' : 10,\n"
    "    'filtBlueHMin' : 120,\n"
    "    'filtBlueHMax' : 120,\n"
    "    'filtBlueVMin' : 10\n"
    "}";

static void drawTarget(vision::Image* image, int x, int y, int width,
                       int height, int borderSize, double angle, CvScalar color)
{
    // Find the inside color of the target
    unsigned char* data = image->getData() + ((x + y * image->getWidth()) * 3);
    CvScalar background = cvScalar(data[0], data[1], data[2]);

    // Draw a filled outer square with a smaller background square
    drawSquare(image, x, y, width, height, angle, color);
    drawSquare(image, x, y, width - borderSize*2,
               height - borderSize*2, angle, background);    
}

struct WindowDetectorFixture
{
    WindowDetectorFixture() :
        input(640, 480, vision::Image::PF_BGR_8),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString(CONFIG), eventHub)
    {
        eventHub->subscribeToType(vision::EventType::TARGET_FOUND,
            boost::bind(&WindowDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::TARGET_LOST,
            boost::bind(&WindowDetectorFixture::foundHandler, this, _1));
    }

    void foundHandler(core::EventPtr event)
    {
        vision::TargetEventPtr tevent =
            boost::dynamic_pointer_cast<vision::TargetEvent>(event);
        assert(tevent != 0 && "Found handler received non-target event");
        foundEvents.push_back(tevent);
    }

    void lostHandler(core::EventPtr event)
    {
        vision::TargetEventPtr tevent =
            boost::dynamic_pointer_cast<vision::TargetEvent>(event);
        assert(tevent != 0 && "Lost handler received non-target event");
        lostEvents.push_back(tevent);
    }

    std::vector<vision::TargetEventPtr> foundEvents;
    std::vector<vision::TargetEventPtr> lostEvents;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::WindowDetector detector;
};

SUITE(WindowDetector) {

TEST_FIXTURE(WindowDetectorFixture, FourTargetsFound)
{
    makeColor(&input, 0, 0, 0);

    // Top left square
    drawTarget(&input, 280, 180, 140, 140, 20, 0, cvScalar(0, 0, 255));
    // Top right square
    drawTarget(&input, 420, 180, 140, 140, 20, 0, cvScalar(0, 255, 0));
    // Bottom left square
    drawTarget(&input, 280, 320, 140, 140, 20, 0, cvScalar(0, 255, 255));
    // Bottom right square
    drawTarget(&input, 420, 320, 140, 140, 20, 0, cvScalar(255, 0, 0));

    vision::OpenCVImage output(640, 480, vision::Image::PF_BGR_8);
    detector.processImage(&input, &output);
    
    CHECK_EQUAL(4u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());
}

TEST_FIXTURE(WindowDetectorFixture, ThreeTargetsFound)
{
    makeColor(&input, 0, 0, 0);

    // Top right square
    drawTarget(&input, 420, 180, 140, 140, 20, 0, cvScalar(0, 255, 0));
    // Bottom left square
    drawTarget(&input, 280, 320, 140, 140, 20, 0, cvScalar(0, 255, 255));
    // Bottom right square
    drawTarget(&input, 420, 320, 140, 140, 20, 0, cvScalar(255, 0, 0));    

    vision::OpenCVImage output(640, 480, vision::Image::PF_BGR_8);
    detector.processImage(&input, &output);

    CHECK_EQUAL(3u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());
}

TEST_FIXTURE(WindowDetectorFixture, TwoTargetsFound)
{
    makeColor(&input, 0, 0, 0);

    // Top right square
    drawTarget(&input, 420, 180, 140, 140, 20, 0, cvScalar(0, 255, 0));
    // Bottom right square
    drawTarget(&input, 420, 320, 140, 140, 20, 0, cvScalar(255, 0, 0));    

    vision::OpenCVImage output(640, 480, vision::Image::PF_BGR_8);
    detector.processImage(&input, &output);

    CHECK_EQUAL(2u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());
}

TEST_FIXTURE(WindowDetectorFixture, OneTargetsFound)
{
    makeColor(&input, 0, 0, 0);
    
    // Bottom left square
    drawTarget(&input, 280, 320, 140, 140, 20, 0, cvScalar(0, 255, 255));

    vision::OpenCVImage output(640, 480, vision::Image::PF_BGR_8);
    detector.processImage(&input, &output);

    CHECK_EQUAL(1u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());

    // Check the found event for color, should be unknown
    if (foundEvents.size() > 0)
        CHECK_EQUAL(vision::Color::YELLOW, foundEvents[0]->color);
}

}
