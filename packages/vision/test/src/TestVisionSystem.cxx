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

        binFound(false),
        binEvent(vision::BinEventPtr()),
        
        targetFound(false),
        targetEvent(vision::TargetEventPtr()),

        barbedWireFound(false),
        barbedWireEvent(vision::BarbedWireEventPtr()),
        
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
        eventHub->subscribeToType(vision::EventType::TARGET_FOUND,
            boost::bind(&VisionSystemFixture::targetFoundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BARBED_WIRE_FOUND,
            boost::bind(&VisionSystemFixture::barbedWireFoundHandler,this,_1));
    }

    void runDetectorForward()
    {
        vision.unbackground(true);
        forwardCamera->background(0);
    
        // Process the current camera image
        vision.update(0);
    }

    void runDetectorDownward()
    {
        vision.unbackground(true);
        downwardCamera->background(0);
        
        // Process the current camera image
        vision.update(0);
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

    void targetFoundHandler(core::EventPtr event_)
    {
        targetFound = true;
        targetEvent = boost::dynamic_pointer_cast<vision::TargetEvent>(event_);
    }

    void barbedWireFoundHandler(core::EventPtr event_)
    {
        barbedWireFound = true;
        barbedWireEvent =
            boost::dynamic_pointer_cast<vision::BarbedWireEvent>(event_);
    }
 
    
    bool redFound;
    vision::RedLightEventPtr redEvent;

    bool pipeFound;
    vision::PipeEventPtr pipeEvent;

    bool binFound;
    vision::BinEventPtr binEvent;

    bool targetFound;
    vision::TargetEventPtr targetEvent;

    bool barbedWireFound;
    vision::BarbedWireEventPtr barbedWireEvent;
    
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
    runDetectorForward();
    vision.redLightDetectorOff();
    forwardCamera->unbackground(true);
    
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
    // draw orange square (upper left)
    drawSquare(&downwardImage, 640/4, 480/4,
               50, 230, 25, CV_RGB(230,180,40));

    // Start dectector and unbackground it
    vision.pipeLineDetectorOn();
    runDetectorDownward();
    vision.pipeLineDetectorOff();
    downwardCamera->unbackground(true);

    // Check Events
    CHECK(pipeFound);
    CHECK(pipeEvent);
    CHECK_CLOSE(-0.5 * 640.0/480.0, pipeEvent->x, 0.05);
    CHECK_CLOSE(0.431, pipeEvent->y, 0.1);
    CHECK_CLOSE(math::Degree(25), pipeEvent->angle, math::Degree(2));
    /// TODO: Add back hough angle detection to the pipe detector
    //CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));
}

TEST_FIXTURE(VisionSystemFixture, BinDetector)
{
    vision::makeColor(&downwardImage, 0, 0, 255);
    // draw orange square (upper left)
    drawBin(&downwardImage, 640/4, 480/4, 130, 25);

    // Start dectector and unbackground it
    vision.binDetectorOn();
    runDetectorDownward();
    vision.binDetectorOff();
    downwardCamera->unbackground(true);

    // Check Events
    CHECK(binFound);
    CHECK(binEvent);
    CHECK_CLOSE(-0.5 * 640.0/480.0, binEvent->x, 0.05);
    CHECK_CLOSE(0.5, binEvent->y, 0.1);
}

TEST_FIXTURE(VisionSystemFixture, TargetDetector)
{
    // Blue Image with green target in the center
    vision::makeColor(&forwardImage, 120, 120, 255);
    drawTarget(&forwardImage, 640/2, 240, 200, 100);

    // Start dectector and unbackground it
    vision.targetDetectorOn();
    runDetectorForward();
    vision.targetDetectorOff();
    forwardCamera->unbackground(true);
    
    // Process the current camera image
    vision.update(0);

    double expectedX = 0 * 640.0/480.0;
    double expectedY = 0;
    double expectedRange = 1.0 - 200.0/480;
    double expectedSquareness = 0.5;

    // Check the events
    CHECK(targetFound);
    CHECK(targetEvent);
    CHECK_CLOSE(expectedX, targetEvent->x, 0.005);
    CHECK_CLOSE(expectedY, targetEvent->y, 0.005);
    CHECK_CLOSE(expectedRange, targetEvent->range, 0.005);
    CHECK_CLOSE(expectedSquareness, targetEvent->squareNess, 0.005);
}

TEST_FIXTURE(VisionSystemFixture, BarbedWireDetector)
{
    // Blue Image with green pipe in the center (horizontal)
    makeColor(&forwardImage, 120, 120, 255);
    drawSquare(&forwardImage, 320, 240, 400, 400/31, 0, CV_RGB(0, 255, 0));
    drawSquare(&forwardImage, 320, 480/4*3, 200, 200/31, 0, CV_RGB(0, 255, 0));

    // Process it
    vision.barbedWireDetectorOn();
    runDetectorForward();
    vision.barbedWireDetectorOff();
    forwardCamera->unbackground(true);

    double expectedTopX = 0;
    double expectedTopY = 0;
    double expectedTopWidth = 401.0/640.0;

    double expectedBottomX = 0;
    double expectedBottomY = -0.5;
    double expectedBottomWidth = 201.0/640.0;
    
    // Check the events
    CHECK(barbedWireFound);
    CHECK(barbedWireEvent);
    CHECK_CLOSE(expectedTopX, barbedWireEvent->topX, 0.005);
    CHECK_CLOSE(expectedTopY, barbedWireEvent->topY, 0.005);
    CHECK_CLOSE(expectedTopWidth, barbedWireEvent->topWidth, 0.005);
    CHECK_CLOSE(expectedBottomX, barbedWireEvent->bottomX, 0.000001);
    CHECK_CLOSE(expectedBottomY, barbedWireEvent->bottomY, 0.000001);
    CHECK_CLOSE(expectedBottomWidth, barbedWireEvent->bottomWidth, 0.000001);
}

} // SUITE(RedLightDetector)
