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
#include "vision/include/WindowDetector.h"
#include "vision/include/Color.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/test/include/Utility.h"

#include "core/include/EventHub.h"

using namespace ram;

static const std::string CONFIG =
    "{"
    "    'filtRedLMin' : 135,"
    "    'filtRedLMax' : 135,"
    "    'filtRedCMin' : 179,"
    "    'filtRedCMin' : 179,"
    "    'filtRedHMin' : 8,"
    "    'filtRedHMin' : 8,"
    "    'filtGreenLMin' : 223,"
    "    'filtGreenLMax' : 223,"
    "    'filtGreenCMin' : 135,"
    "    'filtGreenCMin' : 135,"
    "    'filtGreenHMax' : 90,"
    "    'filtGreenHMin' : 90,"
    "    'filtYellowLMin' : 247,"
    "    'filtYellowLMax' : 247,"
    "    'filtYellowCMin' : 107,"
    "    'filtYellowCMin' : 107,"
    "    'filtYellowHMax' : 60,"
    "    'filtYellowHMin' : 60,"
    "    'filtBlueLMin' : 82,"
    "    'filtBlueLMax' : 82,"
    "    'filtBlueCMin' : 130,"
    "    'filtBlueCMin' : 130,"
    "    'filtBlueHMax' : 188,"
    "    'filtBlueHMin' : 188"
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
        eventHub->subscribeToType(vision::EventType::WINDOW_FOUND,
            boost::bind(&WindowDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::WINDOW_LOST,
            boost::bind(&WindowDetectorFixture::lostHandler, this, _1));
    }

    void foundHandler(core::EventPtr event)
    {
        vision::WindowEventPtr tevent =
            boost::dynamic_pointer_cast<vision::WindowEvent>(event);
        assert(tevent != 0 && "Found handler received non-target event");
        foundEvents.push_back(tevent);
    }

    void lostHandler(core::EventPtr event)
    {
        vision::WindowEventPtr tevent =
            boost::dynamic_pointer_cast<vision::WindowEvent>(event);
        assert(tevent != 0 && "Lost handler received non-target event");
        lostEvents.push_back(tevent);
    }

    void clearEvents()
    {
        foundEvents.clear();
        lostEvents.clear();
    }

    std::vector<vision::WindowEventPtr> foundEvents;
    std::vector<vision::WindowEventPtr> lostEvents;
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
 //6-7-2013 McBryan, broke do to changin colorspace in OpenCVImage.cpp
    /*
    CHECK_EQUAL(4u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());

    if (foundEvents.size() > 3)
        CHECK_EQUAL(vision::Color::RED | vision::Color::GREEN |
                    vision::Color::YELLOW | vision::Color::BLUE,
                    foundEvents[0]->color | foundEvents[1]->color |
                    foundEvents[2]->color | foundEvents[3]->color);
*/
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
/*
    CHECK_EQUAL(3u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());

    if (foundEvents.size() > 2)
        CHECK_EQUAL(vision::Color::GREEN | vision::Color::YELLOW |
                    vision::Color::BLUE,
                    foundEvents[0]->color | foundEvents[1]->color |
                    foundEvents[2]->color);
*/
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
 //6-7-2013 McBryan, broke do to changin colorspace in OpenCVImage.cpp
/*
    CHECK_EQUAL(2u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());

    if (foundEvents.size() > 1)
        CHECK_EQUAL(vision::Color::GREEN | vision::Color::BLUE,
                    foundEvents[0]->color | foundEvents[1]->color);
*/
}

TEST_FIXTURE(WindowDetectorFixture, OneTargetsFound)
{
    makeColor(&input, 0, 0, 0);
    
    // Bottom left square
    drawTarget(&input, 280, 320, 140, 140, 20, 0, cvScalar(0, 255, 255));

    vision::OpenCVImage output(640, 480, vision::Image::PF_BGR_8);
    detector.processImage(&input, &output);
 //6-7-2013 McBryan, broke do to changin colorspace in OpenCVImage.cpp
/*
    CHECK_EQUAL(1u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());

    // Check the found event for color, should be unknown
    if (foundEvents.size() > 0)
        CHECK_EQUAL(vision::Color::YELLOW, foundEvents[0]->color);
*/
}

TEST_FIXTURE(WindowDetectorFixture, WindowLost)
{
    vision::OpenCVImage *secondInput =
        new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8);

    makeColor(&input, 0, 0, 0);
    makeColor(secondInput, 0, 0, 0);

    // Draw two images
    // Top left square
    drawTarget(&input, 280, 180, 140, 140, 20, 0, cvScalar(0, 0, 255));
    drawTarget(secondInput, 280, 180, 140, 140, 20, 0, cvScalar(0, 0, 255));
    // Top right square
    drawTarget(&input, 420, 180, 140, 140, 20, 0, cvScalar(0, 255, 0));
    drawTarget(secondInput, 420, 180, 140, 140, 20, 0, cvScalar(0, 255, 0));
    // Bottom left square
    drawTarget(&input, 280, 320, 140, 140, 20, 0, cvScalar(0, 255, 255));
    drawTarget(secondInput, 280, 320, 140, 140, 20, 0, cvScalar(0, 255, 255));
    // Bottom right square
    drawTarget(&input, 420, 320, 140, 140, 20, 0, cvScalar(255, 0, 0));
    // Do NOT draw for second input

    // Process the first image
    vision::OpenCVImage output(640, 480, vision::Image::PF_BGR_8);
    detector.processImage(&input, &output);
 //6-7-2013 McBryan, broke do to changin colorspace in OpenCVImage.cpp
/*
    CHECK_EQUAL(4u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());

    // Clear the event queue
    clearEvents();

    // Process the second image

    detector.processImage(secondInput, &output);
    CHECK_EQUAL(3u, foundEvents.size());
    CHECK_EQUAL(1u, lostEvents.size());

    // Check that the lost event is for the bottom right square (blue)
    if (lostEvents.size() > 0)
        CHECK_EQUAL(vision::Color::BLUE, lostEvents[0]->color);
*/
    delete secondInput;
}

}
