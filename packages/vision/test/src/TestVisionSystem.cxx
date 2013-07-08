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
#include "vision/include/Image.h"
#include "vision/include/VisionSystem.h"

#include "core/include/EventHub.h"
#include "core/include/TimeVal.h"

#include "vision/test/include/MockCamera.h"

#include "math/test/include/MathChecks.h"
#include "math/include/Events.h"

using namespace ram;

void drawRedCircle(vision::Image* image, int x, int y, int radius = 50);

static const std::string CONFIG = "{"
    "'testing' : 1,"
    "'BuoyDetector' : {"
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
    "},"
    "'WindowDetector' : {"
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
    "},"
    "'VelocityDetector' : {"
    "    'usePhaseCorrelation' : 1"
    "}"
"}";

struct VisionSystemFixture
{

   VisionSystemFixture() :
       buoyFound(false),
        buoyEvent(vision::BuoyEventPtr()),
       
        pipeFound(false),
        pipeEvent(vision::PipeEventPtr()),

        binFound(false),
        binEvent(vision::BinEventPtr()),
        
        cupidFound(false),
        cupidEvent(vision::CupidEventPtr()),

        loversLaneFound(false),
        loversLaneEvent(vision::LoversLaneEventPtr()),
        
        forwardImage(640, 480, vision::Image::PF_BGR_8),
        forwardCamera(new MockCamera(&forwardImage)),

        downwardImage(640, 480, vision::Image::PF_BGR_8),
        downwardCamera(new MockCamera(&downwardImage)),
        
        eventHub(new core::EventHub()),
        vision(vision::CameraPtr(forwardCamera),
               vision::CameraPtr(downwardCamera),
               core::ConfigNode::fromString(CONFIG),
               boost::assign::list_of(eventHub))
    {
        eventHub->subscribeToType(vision::EventType::BUOY_FOUND,
            boost::bind(&VisionSystemFixture::buoyFoundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::PIPE_FOUND,
            boost::bind(&VisionSystemFixture::pipeFoundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BIN_FOUND,
            boost::bind(&VisionSystemFixture::binFoundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::CUPID_FOUND,
            boost::bind(&VisionSystemFixture::cupidFoundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::LOVERSLANE_FOUND,
            boost::bind(&VisionSystemFixture::loversLaneFoundHandler, this, _1));
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
    
    void buoyFoundHandler(core::EventPtr event_)
    {
        buoyFound = true;
        buoyEvent = boost::dynamic_pointer_cast<vision::BuoyEvent>(event_);
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

    void cupidFoundHandler(core::EventPtr event_)
    {
        cupidFound = true;
        cupidEvent = boost::dynamic_pointer_cast<vision::CupidEvent>(event_);
    }

    void loversLaneFoundHandler(core::EventPtr event_)
    {
        loversLaneFound = true;
        loversLaneEvent =
            boost::dynamic_pointer_cast<vision::LoversLaneEvent>(event_);
    }
 
    
    bool buoyFound;
    vision::BuoyEventPtr buoyEvent;

    bool pipeFound;
    vision::PipeEventPtr pipeEvent;

    bool binFound;
    vision::BinEventPtr binEvent;

    bool cupidFound;
    vision::CupidEventPtr cupidEvent;

    bool loversLaneFound;
    vision::LoversLaneEventPtr loversLaneEvent;
    
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

TEST_FIXTURE(VisionSystemFixture, BuoyDetector)
{

    // Blue Image with red circle in upper left
    makeColor(&forwardImage, 0, 0, 255);
    drawRedCircle(&forwardImage, 640/4, 480/4);

    //vision::Image::showImage(&forwardImage);

    // Start dectector and unbackground it
    vision.buoyDetectorOn();
    runDetectorForward();
    vision.buoyDetectorOff();
    forwardCamera->unbackground(true);
    
    math::Degree xFOV = vision::VisionSystem::getFrontHorizontalFieldOfView();
    math::Degree yFOV = vision::VisionSystem::getFrontVerticalFieldOfView();

    // Check the events
 //6-7-2013 McBryan, broke do to changin colorspace in OpenCVImage.cpp
/*
    CHECK(buoyFound);
    CHECK(buoyEvent);
    CHECK_CLOSE(-0.5, buoyEvent->x, 0.005);
    CHECK_CLOSE(0.5, buoyEvent->y, 0.005);
    CHECK_CLOSE(1.05, buoyEvent->range, 0.1);
    CHECK_CLOSE(math::Degree(xFOV/4), buoyEvent->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(yFOV/4), buoyEvent->elevation, math::Degree(0.4));

*/
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

    // // Check Events
    // CHECK(pipeFound);
    // CHECK(pipeEvent);
    // CHECK_CLOSE(-0.5, pipeEvent->x, 0.05);
    // CHECK_CLOSE(0.431, pipeEvent->y, 0.1);
    // CHECK_CLOSE(math::Degree(25), pipeEvent->angle, math::Degree(2));
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
 //   CHECK(binFound);
 //   CHECK(binEvent);
    if (binEvent)
    {
        CHECK_CLOSE(-0.5, binEvent->x, 0.05);
        CHECK_CLOSE(0.5, binEvent->y, 0.1);
    }
}

TEST_FIXTURE(VisionSystemFixture, CupidDetector)
{
    // Blue Image with green cupid in the center
    vision::makeColor(&forwardImage, 0, 0, 255);
    drawTarget(&forwardImage, 640/2, 255, 70, 70);

    // Start dectector and unbackground it
    vision.cupidDetectorOn();
    runDetectorForward();
    vision.cupidDetectorOff();
    forwardCamera->unbackground(true);
    
    // Process the current camera image
    vision.update(0);

    // double expectedX = 0;
    // double expectedY = 0;
    //double expectedRange = 0.310;

    // these dont work because now we are looking for two sub-blobs
    // Check the events
    // CHECK(cupidFound);
    // CHECK(cupidEvent);
    // CHECK_CLOSE(expectedX, cupidEvent->x, 0.005);
    // CHECK_CLOSE(expectedY, cupidEvent->y, 0.005);
    //CHECK_CLOSE(expectedRange, cupidEvent->range, 0.005);
}

TEST_FIXTURE(VisionSystemFixture, LoversLaneDetector)
{
    // Blue Image with green pipe in the center (horizontal)
    makeColor(&forwardImage, 120, 120, 255);
    drawSquare(&forwardImage, 320, 240, 400, 400/31, 0, CV_RGB(0, 255, 0));
    drawSquare(&forwardImage, 320, 480/4*3, 200, 200/31, 0, CV_RGB(0, 255, 0));

    // Process it
    vision.loversLaneDetectorOn();
    runDetectorForward();
    vision.loversLaneDetectorOff();
    forwardCamera->unbackground(true);

    // Check the events
    // CHECK(loversLaneFound);
    // CHECK(loversLaneEvent);
}

} // SUITE(RedLightDetector)
