/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/test/src/TestBuoyDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/BuoyDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/include/Color.h"
#include "vision/test/include/Utility.h"

#include "core/include/EventHub.h"

static const std::string CONFIG =       
    "{"
    "'debug' : 2,"
    "'maxAspectRatio' : 3,"
    "'minAspectRatio' : .5,"
    "'minWidth' : 50,"
    "'minHeight' : 50,"
    "'minPixelPercentage' : .1,"
    "'maxDistance' : 15,"
    "'almostHitRadius' : 200,"
    "'filtRedLMin' : 136,"
    "'filtRedLMax' : 136,"
    "'filtRedUMin' : 223,"
    "'filtRedUMax' : 223,"
    "'filtRedVMin' : 177,"
    "'filtRedVMax' : 177,"
    "'filtGreenLMin' : 224,"
    "'filtGreenLMax' : 224,"
    "'filtGreenUMin' : 37,"
    "'filtGreenUMax' : 37,"
    "'filtGreenVMin' : 246,"
    "'filtGreenVMax' : 246,"
    "'filtYellowLMin' : 248,"
    "'filtYellowLMax' : 248,"
    "'filtYellowUMin' : 102,"
    "'filtYellowUMax' : 102,"
    "'filtYellowVMin' : 246,"
    "'filtYellowVMax' : 246,"
    "}";

    using namespace ram;

struct BuoyDetectorFixture
{
    BuoyDetectorFixture() :
        found(false),
        dropped(false),
        almostHit(false),
        event(vision::BuoyEventPtr()),
        input(640, 480, vision::Image::PF_BGR_8),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString(CONFIG), eventHub)
    {
        eventHub->subscribeToType(vision::EventType::BUOY_FOUND,
            boost::bind(&BuoyDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BUOY_DROPPED,
            boost::bind(&BuoyDetectorFixture::droppedHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BUOY_LOST,
            boost::bind(&BuoyDetectorFixture::lostHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BUOY_ALMOST_HIT,
            boost::bind(&BuoyDetectorFixture::almostHitHandler, this, _1));
    }

    void processImage(vision::Image* image, bool show = false)
    {
        if (show)
        {
            vision::OpenCVImage input(640, 480, vision::Image::PF_BGR_8);
            input.copyFrom(image);
            vision::Image::showImage(&input, "Input");

            vision::OpenCVImage output(640, 480, vision::Image::PF_BGR_8);
            detector.processImage(image, &output);
            vision::Image::showImage(&output, "Output");
        }
        else
        {
            detector.processImage(image);
        }
    }

    void almostHitHandler(core::EventPtr event_)
    {
        almostHit = true;
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::BuoyEvent>(event_);
    }

    void droppedHandler(core::EventPtr event_)
    {
        dropped = true;
        event = boost::dynamic_pointer_cast<vision::BuoyEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
    }

    bool found;
    bool dropped;
    bool almostHit;
    vision::BuoyEventPtr event;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::BuoyDetector detector;
};


SUITE(BuoyDetector) {

// Red Hue: 0
// Yellow Hue: 30
// Green Hue: 60

TEST_FIXTURE(BuoyDetectorFixture, CenterRedBuoy)
{
    vision::makeColor(&input, 0, 0, 0);
    
    vision::OpenCVImage output(640, 480, vision::Image::PF_BGR_8);
    vision::drawCircle(&input, 320, 240, 50, cvScalar(0, 0, 255));
    processImage(&input);
    
    CHECK(found);
    CHECK_CLOSE(0.0, event->x, 0.02);
    CHECK_CLOSE(0.0, event->y, 0.02);
    // Radius of 25
    CHECK_CLOSE(2.963753, event->range, 0.3);
    CHECK_EQUAL(vision::Color::RED, event->color);
}

} // SUITE(BuoyDetector)
