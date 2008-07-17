
/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/test/src/TestOrangePipeDetector.cxx
 */

// STD Includes
#include <cmath>
#include <set>
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <cv.h>

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/BinDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"

#include "core/include/EventHub.h"

#include "vision/test/include/Utility.h"

#include "math/include/Matrix3.h"

using namespace ram;

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "references";
}

struct BinDetectorFixture
{
    BinDetectorFixture() :
        found(false),
        centered(false),
        dropped(false),
        event(vision::BinEventPtr()),
        droppedEvent(vision::BinEventPtr()),
        input(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
        // Subscribe to events like so
        eventHub->subscribeToType(vision::EventType::BIN_FOUND,
            boost::bind(&BinDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BIN_CENTERED,
            boost::bind(&BinDetectorFixture::centeredHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BIN_LOST,
            boost::bind(&BinDetectorFixture::lostHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::BIN_DROPPED,
            boost::bind(&BinDetectorFixture::droppedHandler, this, _1));
        
        detector.setSuitDetectionOn(false);
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::BinEvent>(event_);
    }

    void centeredHandler(core::EventPtr event_)
    {
        centered = true;
        event = boost::dynamic_pointer_cast<vision::BinEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = vision::BinEventPtr();
    }

    void droppedHandler(core::EventPtr event_)
    {
        dropped = true;
        droppedEvent = boost::dynamic_pointer_cast<vision::BinEvent>(event_);
    }
    
    bool found;
    bool centered;
    bool dropped;
    vision::BinEventPtr event;
    vision::BinEventPtr droppedEvent;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::BinDetector detector;
};

SUITE(BinDetector) {

// TODO Test upright, and onside angle

TEST_FIXTURE(BinDetectorFixture, UpperLeft)
{
    detector.setSuitDetectionOn(false);

    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin (upper left, remember image rotated 90 deg)
    drawBin(&input, 640 - (640/4), 480/4, 130, 25);

    // Process it
    //vision::OpenCVImage output(640, 480);
    detector.processImage(&input);//, &output);
    //vision::Image::showImage(&output);
    double expectedX = -0.5;
    double expectedY = 0.5 * 640.0/480.0;
    math::Degree expectedAngle(25);
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, SuperSuitTest)
{ 
    detector.setSuitDetectionOn(true);
    int right = 0;
    for (int deg = 0; deg < 360; deg+=10)
    {
        
        vision::makeColor(&input, 0, 0, 255);
        vision::drawBin(&input, 100,100, 200, deg, vision::Club);
        vision::OpenCVImage output(640,480);
        detector.processImage(&input, &output);
        if (detector.getSuit() == vision::Suit::CLUB)
        {
            right++;
        }
        else if (detector.getSuit() == vision::Suit::UNKNOWN)
        {
            
        }
        else
        {
            CHECK(false);
        }
//        vision::Image::showImage(&output);
    }
    //CHECK(right >= 12);
    
    right = 0;
    for (int deg = 0; deg < 360; deg+=10)
    {
        vision::makeColor(&input, 0, 0, 255);
        vision::drawBin(&input, 320,240, 200, deg, vision::Spade);
        vision::OpenCVImage output(640,480);
        detector.processImage(&input, &output);
        if (detector.getSuit() == vision::Suit::SPADE)
        {
            right++;
        }
        else if (detector.getSuit() == vision::Suit::UNKNOWN)
        {
            
        }
        else
        {
            CHECK(false);
        }
//        vision::Image::showImage(&output);
    }
    //CHECK(right >= 12);

    right = 0;
    for (int deg = 0; deg < 360; deg+=10)
    {
        vision::makeColor(&input, 0, 0, 255);
        vision::drawBin(&input, 320,240, 200, deg, vision::Heart);
        vision::OpenCVImage output(640,480);
        detector.processImage(&input, &output);
        if (detector.getSuit() == vision::Suit::HEART)
        {
            right++;
        }
        else if (detector.getSuit() == vision::Suit::UNKNOWN)
        {
            
        }
        else
        {
            CHECK(false);
        }
//        vision::Image::showImage(&output);
    }
    //CHECK(right >= 12);

    right = 0;
    for (int deg = 0; deg < 360; deg+=10)
    {
        vision::makeColor(&input, 0, 0, 255);
        vision::drawBin(&input, 320,240, 200, deg, vision::Diamond);
        vision::OpenCVImage output(640,480);
        detector.processImage(&input, &output);
        if (detector.getSuit() == vision::Suit::DIAMOND)
        {
            right++;
        }
        else if (detector.getSuit() == vision::Suit::UNKNOWN)
        {
            
        }
        else
        {
            CHECK(false);
        }
//        vision::Image::showImage(&output);
    }
    //CHECK(right >= 12);

    detector.setSuitDetectionOn(false);
}

TEST_FIXTURE(BinDetectorFixture, FourBins)
{
    detector.setSuitDetectionOn(true);
    vision::Image* ref = vision::Image::loadFromFile(
        ((getImagesDir()/"distorted-grainy.png").string()));//Negative flag means load as is, positive means force 3 channel, 0 means force grayscale
    
    CHECK(ref != NULL);
    
    detector.processImage(ref);
    detector.setSuitDetectionOn(false);
}

TEST_FIXTURE(BinDetectorFixture, BinTracking)
{
    detector.setSuitDetectionOn(false);
    vision::makeColor(&input, 0, 0, 255);
    vision::drawBin(&input, 160,240, 150, 70, vision::Heart);
    
    vision::drawBin(&input, 480,240, 150, 70, vision::Diamond);
   
    detector.processImage(&input);
   
    vision::BinDetector::BinList bins = detector.getBins();
    std::vector<vision::BinDetector::Bin> binVect(bins.size());
    std::copy(bins.begin(), bins.end(), binVect.begin());

    CHECK_EQUAL(2u, bins.size());
    
    int minId = binVect[0].getId();
    int maxId = binVect[1].getId();
    
    if (maxId < minId)
    {
        minId = minId^maxId;
        maxId = maxId^minId;
        minId = minId^maxId;
    }
    
    CHECK_EQUAL(0, minId);
    CHECK_EQUAL(1, maxId);
    
    vision::makeColor(&input, 0, 0, 255);
    vision::drawBin(&input, 160,240, 150, 70, vision::Heart);
    
    vision::drawBin(&input, 480,240, 150, 70, vision::Diamond);

    // Process Images
    detector.processImage(&input);
    bins = detector.getBins();
    binVect.reserve(bins.size());
    std::copy(bins.begin(), bins.end(), binVect.begin());

    minId = binVect[0].getId();
    maxId = binVect[1].getId();
    
    if (maxId < minId)
    {
        minId = minId^maxId;
        maxId = maxId^minId;
        minId = minId^maxId;
    }
    
    CHECK_EQUAL(0, minId);
    CHECK_EQUAL(1, maxId);
}

TEST_FIXTURE(BinDetectorFixture, Left)
{
    detector.setSuitDetectionOn(false);
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin (left, remember image rotated 90 deg)
    drawBin(&input, 640/2, 480/4, 130, 0);

    // Process it
    detector.processImage(&input);
    
    double expectedX = -0.5; 
    double expectedY = 0;
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, LowerRight)
{
    detector.setSuitDetectionOn(false);
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin (upper left, remember image rotated 90 deg)
    drawBin(&input, 640/4, 480/4 * 3, 130, -25);

    // Process it
    detector.processImage(&input);
    
    double expectedX = 0.5;
    double expectedY = -0.5 * 640.0/480.0; 
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, CenterUp)
{
    detector.setSuitDetectionOn(false);
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin in center sideways
    drawBin(&input, 640/2, 480/2, 130, 0);

    // Process it
    detector.processImage(&input);

    double expectedX = 0;
    double expectedY = 0 * 640.0/480.0; 
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, CenterSideways)
{
    detector.setSuitDetectionOn(false);
    // Blue Image with orange rectangle in it
    vision::makeColor(&input, 0, 0, 255);
    // draw the bin in center sideways
    drawBin(&input, 640/2, 480/2, 130, 90);
    
    // Process it
    detector.processImage(&input);
    
    double expectedX = 0;
    double expectedY = 0 * 640.0/480.0; 
    
    CHECK(detector.found());
    CHECK_CLOSE(expectedX, detector.getX(), 0.05);
    CHECK_CLOSE(expectedY, detector.getY(), 0.05);

    // Check Events
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(expectedX, event->x, 0.05);
    CHECK_CLOSE(expectedY, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, Events_BIN_LOST)
{
    detector.setSuitDetectionOn(false);
    // No light at all
    makeColor(&input, 0, 0, 255);
    
    detector.processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Now we found the bin (lower right location)
    drawBin(&input, 640/4, 480/4 * 3, 130, -25);
    detector.processImage(&input);
    CHECK(found);
    CHECK(event);
    CHECK_CLOSE(0.5, event->x, 0.05);
    CHECK_CLOSE(-0.5 * 640.0/480.0, event->y, 0.05);

    // Now we lost the bin
    makeColor(&input, 0, 0, 255);
    detector.processImage(&input);
    CHECK(found == false);
    CHECK(!event);

    // Make sure we don't get another lost event
    found = true;
    detector.processImage(&input);
    CHECK(found == true);
}

TEST_FIXTURE(BinDetectorFixture, Events_BIN_DROPPED)
{
    detector.setSuitDetectionOn(false);
    // Place a set of four bins on screen
    makeColor(&input, 0, 0, 255);
    drawBin(&input, 640/5, 480/2, 100, 0);
    drawBin(&input, 640/5 * 2, 480/2, 100, 0);
    drawBin(&input, 640/5 * 3, 480/2, 100, 0);
    drawBin(&input, 640/5 * 4, 480/2, 100, 0);
    
    detector.processImage(&input);

    // Make sure we haven't dropped anything
    CHECK_EQUAL(false, dropped);
    
    // Ensure unique IDs
    vision::BinDetector::BinList bins = detector.getBins();
    CHECK_EQUAL(4u, bins.size());
    std::set<int> startingIds;
    BOOST_FOREACH(vision::BinDetector::Bin bin, bins)
    {
        startingIds.insert(bin.getId());
    }
    CHECK_EQUAL(4u, startingIds.size());
    
    // Record ID of bin, then nuke it
    makeColor(&input, 0, 0, 255);
    drawBin(&input, 640/5, 480/2, 100, 0);
    drawBin(&input, 640/5 * 2, 480/2, 100, 0);
    drawBin(&input, 640/5 * 3, 480/2, 100, 0);    

    detector.processImage(&input);
    bins = detector.getBins();
    CHECK_EQUAL(3u, bins.size());
    std::set<int> endingIds;
    BOOST_FOREACH(vision::BinDetector::Bin bin, bins)
    {
        endingIds.insert(bin.getId());
    }
    CHECK_EQUAL(3u, endingIds.size());

    // Find the difference
    std::set<int> lostIds;
    std::set_difference(startingIds.begin(), startingIds.end(),
                        endingIds.begin(), endingIds.end(),
                        std::inserter(lostIds, lostIds.begin()));
    CHECK_EQUAL(1u, lostIds.size());
    
    // Now make sure we got the right dropped event
    CHECK(dropped);
    CHECK(droppedEvent);
    CHECK_EQUAL((*lostIds.begin()), droppedEvent->id);
}

TEST_FIXTURE(BinDetectorFixture, Events_BIN_CENTERED)
{
    detector.setSuitDetectionOn(false);
    // Bin in the lower right
    makeColor(&input, 0, 0, 255);
    drawBin(&input, 640/4, 480/4 * 3, 130, 0);
    detector.processImage(&input);
    CHECK(found);
    CHECK(event);
    CHECK(!centered);
    CHECK_CLOSE(0.5, event->x, 0.05);
    CHECK_CLOSE(-0.5 * 640.0/480.0, event->y, 0.05);    

    // Now bin is dead center
    makeColor(&input, 0, 0, 255);
    drawBin(&input, 640/2, 480/2, 130, 0);
    detector.processImage(&input);
    CHECK(found);

    // Make sure we get the centered event
    CHECK(centered);
    CHECK(event);
    CHECK_CLOSE(0, event->x, 0.05);
    CHECK_CLOSE(0, event->y, 0.05);

    // Blank image
    centered = false;
    found = false;
    event = vision::BinEventPtr();
    makeColor(&input, 0, 0, 255);
    detector.processImage(&input);

    CHECK(!event);
    CHECK(!found);
    CHECK(!centered);

    // Now make a dead centered bin, and make sure get another centered
    makeColor(&input, 0, 0, 255);
    drawBin(&input, 640/2, 480/2, 130, 0);
    detector.processImage(&input);

    CHECK(found);
    CHECK(centered);
    CHECK(event);
    CHECK_CLOSE(0, event->x, 0.05);
    CHECK_CLOSE(0, event->y, 0.05);
}

TEST_FIXTURE(BinDetectorFixture, Suit)
{
    // Bin in center
    makeColor(&input, 0, 0, 255);
    drawBin(&input, 200, 150, 130, 25, vision::Heart);
//    vision::Image::showImage(&input);
}

} // SUITE(BinDetector)
