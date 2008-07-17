/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestVisionSystem.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>

// Project Includes
#include "vision/include/VisionSystem.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/test/include/Utility.h"

#include "core/include/EventHub.h"
#include "core/include/TimeVal.h"

#include "vision/test/include/MockCamera.h"

using namespace ram;

void drawRedCircle(vision::Image* image, int x, int y, int radius = 50);

struct VisionSystemFixture
{
    VisionSystemFixture() :
        redFound(false),
        redEvent(vision::RedLightEventPtr()),
        
        pipeFound(false),
        pipeEvent(vision::PipeEventPtr()),
        
        forwardImage(640, 480),
        forwardCamera(new MockCamera(&forwardImage)),

        downwardImage(640, 480),
        downwardCamera(new MockCamera(&downwardImage)),
        
        eventHub(new core::EventHub()),
        vision(vision::CameraPtr(forwardCamera),
               vision::CameraPtr(downwardCamera),
               core::ConfigNode::fromString("{'testing' : 1}"),
               boost::assign::list_of(eventHub))
    {
        eventHub->subscribeToType(vision::EventType::LIGHT_FOUND,
            boost::bind(&VisionSystemFixture::redFoundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::PIPE_FOUND,
            boost::bind(&VisionSystemFixture::pipeFoundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BIN_FOUND,
            boost::bind(&VisionSystemFixture::binFoundHandler, this, _1));
    }

    void redFoundHandler(core::EventPtr event_)
    {
        redFound = true;
        redEvent = boost::dynamic_pointer_cast<vision::RedLightEvent>(event_);
    }

    void pipeFoundHandler(core::EventPtr event_)
    {
        pipeFound = true;
        pipeEvent = boost::dynamic_pointer_cast<vision::PipeEvent>(event_);
    }

    void binFoundHandler(core::EventPtr event_)
    {
        binFound = true;
        binEvent = boost::dynamic_pointer_cast<vision::BinEvent>(event_);
    }
    
    
    bool redFound;
    vision::RedLightEventPtr redEvent;

    bool pipeFound;
    vision::PipeEventPtr pipeEvent;

    bool binFound;
    vision::BinEventPtr binEvent;
    
    vision::OpenCVImage forwardImage;
    MockCamera* forwardCamera;

    vision::OpenCVImage downwardImage;
    MockCamera* downwardCamera;
    
    core::EventHubPtr eventHub;
    vision::VisionSystem vision;
};



SUITE(VisionSystem) {

TEST(CreateDestroy)
{
    vision::VisionSystem vision(vision::CameraPtr(new MockCamera()),
                                vision::CameraPtr(new MockCamera()),
                                core::ConfigNode::fromString("{}"),
                                core::SubsystemList());
}

TEST_FIXTURE(VisionSystemFixture, RedLightDetector)
{
    // Blue Image with red circle in upper left
    makeColor(&forwardImage, 0, 0, 255);
    drawRedCircle(&forwardImage, 640/4, 480/4);

    // Start dectector and unbackground it
    vision.redLightDetectorOn();
    vision.unbackground(true);
    forwardCamera->background(0);
    
    // Process the current camera image
    vision.update(0);
    vision.redLightDetectorOff();

    // Check the events
    CHECK(redFound);
    CHECK(redEvent);
    CHECK_CLOSE(-0.5 * 4.0/3.0, redEvent->x, 0.005);
    CHECK_CLOSE(0.5, redEvent->y, 0.005);
    CHECK_CLOSE(3, redEvent->range, 0.1);
    CHECK_CLOSE(math::Degree(78.0/4), redEvent->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(105.0/4), redEvent->elevation, math::Degree(0.4));

    forwardCamera->unbackground(true);
}

TEST_FIXTURE(VisionSystemFixture, PipeDetector)
{
    vision::makeColor(&downwardImage, 0, 0, 255);
    // draw orange square (upper left, remember image rotated 90 deg)
    drawSquare(&downwardImage, 640 - (640/4), 480/4,
               230, 50, 25, CV_RGB(230,180,40));

    // Start dectector and unbackground it
    vision.pipeLineDetectorOn();
    vision.unbackground(true);
    forwardCamera->background(0);
    
    // Process the current camera image
    vision.update(0);
    vision.pipeLineDetectorOff();

    forwardCamera->unbackground(true);

    // Check Events
    CHECK(pipeFound);
    CHECK(pipeEvent);
    CHECK_CLOSE(-0.5, pipeEvent->x, 0.05);
    CHECK_CLOSE(0.431 * 640.0/480.0, pipeEvent->y, 0.1);
    CHECK_CLOSE(math::Degree(25), pipeEvent->angle, math::Degree(0.5));
}
    
TEST_FIXTURE(VisionSystemFixture, BinDetector)
{
    vision::makeColor(&downwardImage, 0, 0, 255);
    // draw orange square (upper left, remember image rotated 90 deg)
    drawBin(&downwardImage, 640 - (640/4), 480/4, 130, 25);

    // Start dectector and unbackground it
    vision.binDetectorOn();
    vision.unbackground(true);
    forwardCamera->background(0);
    
    // Process the current camera image
    vision.update(0);
    vision.binDetectorOff();

    forwardCamera->unbackground(true);

    // Check Events
    CHECK(binFound);
    CHECK(binEvent);
    CHECK_CLOSE(-0.5, binEvent->x, 0.05);
    CHECK_CLOSE(0.5 * 640.0/480.0, binEvent->y, 0.1);
}

} // SUITE(RedLightDetector)
