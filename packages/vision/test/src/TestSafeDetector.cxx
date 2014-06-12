 
/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/test/src/TestSafeDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

// Project Includes
#include "vision/include/SafeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Image.h"
#include "vision/include/Events.h"
#include "vision/test/include/Utility.h"

#include "core/include/EventHub.h"

using namespace ram;

//static boost::filesystem::path getImagesDir()
//{
//    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
//    return root / "packages" / "vision" / "test" / "data" / "images" / "testsafe";
//}

void drawSafe(vision::Image* image, int x, int y, int width = 20)
{
    drawSquare(image, x+(int)(width*1.5f), y+(int)(width * 4), width*2, width,
                0, CV_RGB(175, 150, 50));
    drawSquare(image, x-(int)(width*1.5f), y+(int)(width * 4), width*2, width,
                0, CV_RGB(175, 150, 50));

    drawSquare(image, x+(int)(width*1.5f), y-(int)(width * 4), width*2, width,
                0, CV_RGB(175, 150, 50));
    drawSquare(image, x-(int)(width*1.5f), y-(int)(width * 4), width*2, width,
                0, CV_RGB(175, 150, 50));

    drawSquare(image, x+(int)(width*4), y+(int)(width * 1.5f), width, width*2,
                0, CV_RGB(175, 150, 50));
    drawSquare(image, x+(int)(width*4), y-(int)(width * 1.5f), width, width*2,
                0, CV_RGB(175, 150, 50));

    drawSquare(image, x-(int)(width*4), y+(int)(width * 1.5f), width, width*2,
                0, CV_RGB(175, 150, 50));
    drawSquare(image, x-(int)(width*4), y-(int)(width * 1.5f), width, width*2,
                0, CV_RGB(175, 150, 50));
}

struct SafeDetectorFixture
{
    SafeDetectorFixture() :
       found(false),
        event(vision::SafeEventPtr()),
        input(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"),eventHub)
    {
        eventHub->subscribeToType(vision::EventType::SAFE_FOUND,
            boost::bind(&SafeDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::SAFE_LOST,
            boost::bind(&SafeDetectorFixture::lostHandler, this, _1));
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::SafeEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::SafeEventPtr();
    }
    
    bool found;
    vision::SafeEventPtr event;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    ram::vision::SafeDetector detector;
};

SUITE(SafeDetector) {


TEST_FIXTURE(SafeDetectorFixture, seeSafe)
{
    vision::OpenCVImage output(640,480);
    vision::makeColor(&input, 0,0,255);
    drawSafe(&input, 320,240,10);
    detector.processImage(&input,&output);
//    vision::Image::showImage(&output);
    CHECK(event);
    if (event)
    {
        CHECK_CLOSE(0, event->x,.05);
        CHECK_CLOSE(0, event->y,.05);
    }
}

TEST_FIXTURE(SafeDetectorFixture, seeSafeRun)
{
    vision::OpenCVImage output(640,480);
    vision::makeColor(&input, 0,0,255);
    drawSafe(&input, 160,120,10);
    detector.processImage(&input,&output);
//    vision::Image::showImage(&output);
    CHECK(event);
    if (event)
    {
        CHECK_CLOSE(-1.3/2, event->x,.05);
        CHECK_CLOSE(1.0/2.0, event->y,.05);
    }

    vision::makeColor(&input, 0,0,255);
    drawSafe(&input, 320,240,10);
    detector.processImage(&input,&output);
//    vision::Image::showImage(&output);
    CHECK(event);
    if (event)
    {
        CHECK_CLOSE(0, event->x,.05);
        CHECK_CLOSE(0, event->y,.05);
    }

    vision::makeColor(&input, 0,0,255);
    drawSafe(&input, 480,360,10);
    detector.processImage(&input,&output);
//    vision::Image::showImage(&output);
    CHECK(event);
    if (event)
    {
        CHECK_CLOSE(1.3/2, event->x,.05);
        CHECK_CLOSE(-1.0/2.0, event->y,.05);
    }
}

TEST_FIXTURE(SafeDetectorFixture, runSafeRun)
{
    vision::OpenCVImage output(640,480);
    vision::makeColor(&input, 0,0,255);
    drawSafe(&input, 160,120,10);
    detector.processImage(&input,&output);
//    vision::Image::showImage(&output);
    CHECK(event);
    if (event)
    {
        CHECK_CLOSE(-1.3/2, event->x,.05);
        CHECK_CLOSE(1.0/2.0, event->y,.05);
    }
    
    // Now we lost the Safe
    makeColor(&input, 0, 0, 255);
    detector.processImage(&input, &output);
    CHECK(found == false);
    CHECK(!event);

    vision::makeColor(&input, 0,0,255);
    drawSafe(&input, 320,240,10);
    detector.processImage(&input,&output);
//    vision::Image::showImage(&output);
    CHECK(event);
    if (event)
    {
        CHECK_CLOSE(0, event->x,.05);
        CHECK_CLOSE(0, event->y,.05);
    }

    // Now we lost the Safe
    makeColor(&input, 0, 0, 255);
    detector.processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    vision::makeColor(&input, 0,0,255);
    drawSafe(&input, 480,360,10);
    detector.processImage(&input,&output);
//    vision::Image::showImage(&output);
    CHECK(event);
    if (event)
    {
        CHECK_CLOSE(1.3/2, event->x,.05);
        CHECK_CLOSE(-1.0/2.0, event->y,.05);
    }
}




} // SUITE(DuctDetector)
