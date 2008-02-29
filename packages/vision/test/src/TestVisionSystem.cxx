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

#include "core/include/EventHub.h"
#include "core/include/TimeVal.h"

#include "vision/test/include/MockCamera.h"

using namespace ram;

void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B);

void drawRedCircle(vision::Image* image, int x, int y, int radius = 50);

struct VisionSystemFixture
{
    VisionSystemFixture() :
        found(false),
        event(vision::RedLightEventPtr()),
        
        forwardImage(640, 480),
        forwardCamera(new MockCamera(&forwardImage)),

        downwardImage(640, 480),
        downwardCamera(new MockCamera(&downwardImage)),
        
        eventHub(new core::EventHub()),
        vision(vision::CameraPtr(forwardCamera),
               vision::CameraPtr(downwardCamera),
               core::ConfigNode::fromString("{}"),
               boost::assign::list_of(eventHub))
    {
        eventHub->subscribeToType(vision::EventType::LIGHT_FOUND,
            boost::bind(&VisionSystemFixture::redFoundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::LIGHT_LOST,
            boost::bind(&VisionSystemFixture::redLostHandler, this, _1));
    }

    void redFoundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::RedLightEvent>(event_);
    }

    void redLostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::RedLightEventPtr();
    }
    
    bool found;
    vision::RedLightEventPtr event;
    
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
#ifndef RAM_WINDOWS
TEST_FIXTURE(VisionSystemFixture, RedLightDetector)
{
    // Blue Image with red circle in upper left (remeber image rotated 90 deg)
    makeColor(&forwardImage, 0, 0, 255);
    drawRedCircle(&forwardImage, 640 - (640/4), 480/4);

    // "Backgrounds" the camera (no real background thread, because this is
    // just a mock object)
    forwardCamera->background(0);

    // Have to wait for the processing thread to be waiting on the camera
    vision.redLightDetectorOn();
    ram::core::TimeVal::sleep(500 / 1000.0);

    // Release the image from the camera (and wait for the detector capture it)
    forwardCamera->update(0);
    ram::core::TimeVal::sleep(500.0 / 1000.0);

    // This stops the background thread
    vision.redLightDetectorOff();

    // Check the events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(-0.5, event->x, 0.005);
    CHECK_CLOSE(0.5, event->y, 0.005);
    CHECK_CLOSE(3, event->range, 0.1);
    CHECK_CLOSE(math::Degree(78.0/4), event->azimuth, math::Degree(0.4));
    CHECK_CLOSE(math::Degree(105.0/4), event->elevation, math::Degree(0.4));

    forwardCamera->unbackground(true);
}
#endif
} // SUITE(RedLightDetector)
