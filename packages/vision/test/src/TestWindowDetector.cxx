/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/test/src/TestWindowDetector.cxx
 */

// STD Includes
#include <vector>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#define private public
#include "vision/include/WindowDetector.h"
#undef private
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/test/include/Utility.h"

#include "core/include/EventHub.h"

using namespace ram;

struct WindowDetectorFixture
{
    WindowDetectorFixture() :
        input(640, 480, vision::Image::PF_BGR_8),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
        eventHub->subscribeToType(vision::EventType::TARGET_FOUND,
            boost::bind(&WindowDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::TARGET_LOST,
            boost::bind(&WindowDetectorFixture::foundHandler, this, _1));
    }

    void foundHandler(core::EventPtr event)
    {
        foundEvents.push_back(event);
    }

    void lostHandler(core::EventPtr event)
    {
        lostEvents.push_back(event);
    }

    std::vector<core::EventPtr> foundEvents;
    std::vector<core::EventPtr> lostEvents;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::WindowDetector detector;
};

SUITE(WindowDetector)
{

TEST_FIXTURE(WindowDetectorFixture, FourTargetsFound)
{
    makeColor(&input, 0, 0, 0);

    // Top left square
    drawSquare(&input, 280, 180, 140, 140, 0, cvScalar(0, 0, 255));
    drawSquare(&input, 280, 180, 100, 100, 0, cvScalar(0, 0, 0));

    // Top right square
    drawSquare(&input, 420, 180, 140, 140, 0, cvScalar(0, 255, 0));
    drawSquare(&input, 420, 180, 100, 100, 0, cvScalar(0, 0, 0));

    // Bottom left square
    drawSquare(&input, 280, 320, 140, 140, 0, cvScalar(0, 255, 255));
    drawSquare(&input, 280, 320, 100, 100, 0, cvScalar(0, 0, 0));

    // Bottom right square
    drawSquare(&input, 420, 320, 140, 140, 0, cvScalar(255, 255, 0));
    drawSquare(&input, 420, 320, 100, 100, 0, cvScalar(0, 0, 0));

    vision::OpenCVImage output(640, 480, vision::Image::PF_BGR_8);
    detector.processImage(&input, &output);

    vision::Image::showImage(&input, "Input");
    vision::Image::showImage(&output, "Output");
    
    CHECK_EQUAL(4u, foundEvents.size());
    CHECK_EQUAL(0u, lostEvents.size());
}

}
