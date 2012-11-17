/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/test/src/TestOrangePipeDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <cv.h>
// Project Includes
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"

#include "core/include/EventHub.h"

#include "vision/test/include/Utility.h"

using namespace ram;

struct OrangePipeDetectorFixture
{
    OrangePipeDetectorFixture() :
        found(false),
        centered(false),
        event(vision::PipeEventPtr()),
        input(640, 480, vision::Image::PF_BGR_8),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
        // Subscribe to events like so
        eventHub->subscribeToType(vision::EventType::PIPE_FOUND,
            boost::bind(&OrangePipeDetectorFixture::foundHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::PIPE_CENTERED,
            boost::bind(&OrangePipeDetectorFixture::centeredHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::PIPE_LOST,
            boost::bind(&OrangePipeDetectorFixture::lostHandler, this, _1));
        eventHub->subscribeToType(vision::EventType::PIPE_DROPPED,
            boost::bind(&OrangePipeDetectorFixture::droppedHandler, this, _1));
    }

    void processImage(vision::Image* image, bool show = false)
    {
        foundEvents.clear();
        lostEvents.clear();
        droppedEvents.clear();
        
        if (show)
	{
	    vision::OpenCVImage input(640, 480);
	    input.copyFrom(image);
	    vision::Image::showImage(&input, "Input");

	    vision::OpenCVImage output(640, 480);
	    detector.processImage(image, &output);
	    vision::Image::showImage(&output, "Output");
	}
	else
        {
            detector.processImage(image);
	}

        pipes = detector.getPipes();
    }

    void foundHandler(core::EventPtr event_)
    {
        found = true;
        event = boost::dynamic_pointer_cast<vision::PipeEvent>(event_);
        foundEvents.push_back(event);
    }

    void centeredHandler(core::EventPtr event_)
    {
        centered = true;
        event = boost::dynamic_pointer_cast<vision::PipeEvent>(event_);
    }

    void lostHandler(core::EventPtr event_)
    {
        found = false;
        event = boost::dynamic_pointer_cast<vision::PipeEvent>(event_);
        lostEvents.push_back(event);
    }

    void droppedHandler(core::EventPtr event_)
    {
        event = boost::dynamic_pointer_cast<vision::PipeEvent>(event_);
        droppedEvents.push_back(event);
    }

    
    bool found;
    bool centered;
    vision::PipeEventPtr event;
    std::vector<vision::PipeEventPtr> foundEvents;
    std::vector<vision::PipeEventPtr> lostEvents;
    std::vector<vision::PipeEventPtr> droppedEvents;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::OrangePipeDetector detector;
    vision::PipeDetector::PipeList pipes;
};

SUITE(OrangePipeDetector) {

// TEST_FIXTURE(OrangePipeDetectorFixture, TestLUV)
// {
//     // Blue Image with orange rectangle in it
//     vision::makeColor(&input, 0, 0, 255);
//     // draw orange square (upper left, remember image rotated 90 deg)
//     drawSquare(&input, 640/4, 480/4, 50, 230, 25, CV_RGB(230,180,40));

//     // Process it
//     detector.setUseLUVFilter(true);
//     processImage(&input);
    
//     double expectedX = -0.5;
//     double expectedY = 0.5;
//     math::Degree expectedAngle(25);
    
//     CHECK(detector.found());
//     CHECK_CLOSE(expectedX, detector.getX(), 0.05);
//     CHECK_CLOSE(expectedY, detector.getY(), 0.05);
//     CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(2));
// }

// TEST_FIXTURE(OrangePipeDetectorFixture, TestNoLUV)
// {
//     // Blue Image with orange rectangle in it
//     vision::makeColor(&input, 0, 0, 255);
//     // draw orange square (upper left, remember image rotated 90 deg)
//     drawSquare(&input, 640/4, 480/4, 50, 230, 25, CV_RGB(230,180,40));

//     // Process it
//     detector.setUseLUVFilter(false);
//     processImage(&input);
    
//     double expectedX = -0.5;
//     double expectedY = 0.5;
//     math::Degree expectedAngle(25);
    
//     CHECK(detector.found());
//     CHECK_CLOSE(expectedX, detector.getX(), 0.05);
//     CHECK_CLOSE(expectedY, detector.getY(), 0.05);
//     CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(2));
// }


// // TODO Test upright, and onside angle
// TEST_FIXTURE(OrangePipeDetectorFixture, UpperLeft)
// {
//     // Blue Image with orange rectangle in it
//     vision::makeColor(&input, 0, 0, 255);
//     // draw orange square (upper left, remember image rotated 90 deg)
//     drawSquare(&input, 640/4, 480/4, 50, 230, 25, CV_RGB(230,180,40));

//     // Process it
//     processImage(&input);
    
//     double expectedX = -0.5;
//     double expectedY = 0.5;
//     math::Degree expectedAngle(25);
    
//     CHECK(detector.found());
//     CHECK_CLOSE(expectedX, detector.getX(), 0.05);
//     CHECK_CLOSE(expectedY, detector.getY(), 0.05);
//     CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(2));
//     /// TODO: Add back hough angle detection to the pipe detector
//     //CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

//     // Check Events
//     CHECK(found);
//     CHECK(event);
//     CHECK_CLOSE(expectedX, event->x, 0.05);
//     CHECK_CLOSE(expectedY, event->y, 0.05);
//     CHECK_CLOSE(expectedAngle, event->angle, math::Degree(2));
//     /// TODO: Add back hough angle detection to the pipe detector
//     //CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
// }

// TEST_FIXTURE(OrangePipeDetectorFixture, Left)
// {
//     // Blue Image with orange rectangle in it
//     vision::makeColor(&input, 0, 0, 255);
//     // draw orange square (left side)
//     drawSquare(&input, 640/4, 480/2, 50, 230, 0, CV_RGB(230,180,40));

//     // Process it
//     processImage(&input);
    
//     double expectedX = -0.5;
//     double expectedY = 0;
//     math::Degree expectedAngle(0);
    
//     CHECK(detector.found());
//     CHECK_CLOSE(expectedX, detector.getX(), 0.05);
//     CHECK_CLOSE(expectedY, detector.getY(), 0.05);
//      CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(2));
//     /// TODO: Add back hough angle detection to the pipe detector
//     //CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

//     // Check Events
//     CHECK(found);
//     CHECK(event);
//     CHECK_CLOSE(expectedX, event->x, 0.05);
//     CHECK_CLOSE(expectedY, event->y, 0.05);
//     CHECK_CLOSE(expectedAngle, event->angle, math::Degree(2));
//     /// TODO: Add back hough angle detection to the pipe detector
//     //CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
// }

// TEST_FIXTURE(OrangePipeDetectorFixture, LowerRight)
// {
//     // Blue Image with orange rectangle in it
//     vision::makeColor(&input, 0, 0, 255);
//     // draw orange square (upper left)
//     drawSquare(&input, 640 - 640/4, 480/4 * 3,
//                50, 230, -25, CV_RGB(230,180,40));

//     // Process it
//     processImage(&input);
    
//     double expectedX = 0.5;
//     double expectedY = -0.5;
//     math::Degree expectedAngle(-25);
    
//     CHECK(detector.found());
//     CHECK_CLOSE(expectedX, detector.getX(), 0.05);
//     CHECK_CLOSE(expectedY, detector.getY(), 0.05);
//      CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(2));
//     /// TODO: Add back hough angle detection to the pipe detector
//     //CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

//     // Check Events
//     CHECK(found);
//     CHECK(event);
//     CHECK_CLOSE(expectedX, event->x, 0.05);
//     CHECK_CLOSE(expectedY, event->y, 0.05);
//     CHECK_CLOSE(expectedAngle, event->angle, math::Degree(2));
//     /// TODO: Add back hough angle detection to the pipe detector
//     //CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
// }

// TEST_FIXTURE(OrangePipeDetectorFixture, CenterUp)
// {
//     // Blue Image with orange rectangle in it
//     vision::makeColor(&input, 0, 0, 255);
//     // draw orange square in center sideways
//     drawSquare(&input, 640/2, 480/2, 50, 230, 0, CV_RGB(230,180,40));

//     // Process it
//     processImage(&input);

//     double expectedX = 0;
//     double expectedY = 0;
//     math::Degree expectedAngle(0);
    
//     CHECK(detector.found());
//     CHECK_CLOSE(expectedX, detector.getX(), 0.05);
//     CHECK_CLOSE(expectedY, detector.getY(), 0.05);
//     CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(2));
//     /// TODO: Add back hough angle detection to the pipe detector
//     //CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

//     // Check Events
//     CHECK(found);
//     CHECK(event);
//     CHECK_CLOSE(expectedX, event->x, 0.05);
//     CHECK_CLOSE(expectedY, event->y, 0.05);
//     CHECK_CLOSE(expectedAngle, event->angle, math::Degree(2));
//     /// TODO: Add back hough angle detection to the pipe detector
//     //CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
// }

// TEST_FIXTURE(OrangePipeDetectorFixture, CenterSideways)
// {
//     // Blue Image with orange rectangle in it
//     vision::makeColor(&input, 0, 0, 255);
//     // draw orange square in center sideways
//     drawSquare(&input, 640/2, 480/2, 50, 230, 90, CV_RGB(230,180,40));
    
//     // Process it
//     processImage(&input);
    
//     double expectedX = 0;
//     double expectedY = 0;
//     math::Degree expectedAngle(-90);
    
//     CHECK(detector.found());
//     CHECK_CLOSE(expectedX, detector.getX(), 0.05);
//     CHECK_CLOSE(expectedY, detector.getY(), 0.05);
//     //CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(0.5));

//     // Check Events
//     CHECK(found);
//     CHECK(event);
//     CHECK_CLOSE(expectedX, event->x, 0.05);
//     CHECK_CLOSE(expectedY, event->y, 0.05);
//     //CHECK_CLOSE(expectedAngle, event->angle, math::Degree(0.5));
// }

// TEST_FIXTURE(OrangePipeDetectorFixture, UpperLeftNoHough)
// {
//     // Disable hough to use blob based system instead
//     detector.setHough(true);

//     // Blue Image with orange rectangle in it
//     vision::makeColor(&input, 0, 0, 255);
//     // draw orange square (upper left, remember image rotated 90 deg)
//     drawSquare(&input, 640/4, 480/4, 50, 230, 25, CV_RGB(230,180,40));

//     // Process it
//     processImage(&input);
    
//     double expectedX = -0.5;
//     double expectedY = 0.5;
//     math::Degree expectedAngle(25);
    
//     CHECK(detector.found());
//     CHECK_CLOSE(expectedX, detector.getX(), 0.05);
//     CHECK_CLOSE(expectedY, detector.getY(), 0.05);
//     /// TODO: Determine why this doesn't have 1 degree accuracy any more
//     CHECK_CLOSE(expectedAngle, detector.getAngle(), math::Degree(2));

//     // Check Events
//     CHECK(found);
//     CHECK(event);
//     CHECK_CLOSE(expectedX, event->x, 0.05);
//     CHECK_CLOSE(expectedY, event->y, 0.05);
//     /// TODO: Determine why this doesn't have 1 degree accuracy any more
//     CHECK_CLOSE(expectedAngle, event->angle, math::Degree(2));
// }

// TEST_FIXTURE(OrangePipeDetectorFixture, Events_PIPE_LOST)
// {
//     // No light at all
//     makeColor(&input, 0, 0, 255);
    
//     processImage(&input);
//     CHECK(found == false);
//     CHECK(!event);

//     // Now we found the pipe (lower right location)
//     drawSquare(&input, 640 - 640/4, 480/4 * 3,
//                50, 230, -25, CV_RGB(230,180,40));
//     processImage(&input);
//     CHECK(found);
//     CHECK(event);
//     CHECK_CLOSE(0.5, event->x, 0.05);
//     CHECK_CLOSE(-0.5, event->y, 0.05);

//     // Now we lost the light
//     makeColor(&input, 0, 0, 255);
//     processImage(&input);
//     CHECK(found == false);
//     CHECK_EQUAL(1u, lostEvents.size());

//     // Make sure we don't get another lost event
//     processImage(&input);
//     CHECK_EQUAL(0u, lostEvents.size());
// }

// TEST_FIXTURE(OrangePipeDetectorFixture, Events_PIPE_CENTERED)
// {
//     // Pipe in the lower right
//     makeColor(&input, 0, 0, 255);
//     drawSquare(&input, 640 - 640/4, 480/4 * 3,
//                50, 230, 0, CV_RGB(230,180,40));
//     processImage(&input);
//     CHECK(found);
//     CHECK(event);
//     CHECK(!centered);
//     CHECK_CLOSE(0.5, event->x, 0.05);
//     CHECK_CLOSE(-0.5, event->y, 0.05);    

//     // Now pipe is dead center
//     makeColor(&input, 0, 0, 255);
//     drawSquare(&input, 640/2, 480/2, 50, 230, 0, CV_RGB(230,180,40));    
//     processImage(&input);
//     CHECK(found);

//     // Make sure we get the centered event
//     CHECK(centered);
//     CHECK(event);
//     CHECK_CLOSE(0, event->x, 0.05);
//     CHECK_CLOSE(0, event->y, 0.05);
// }

// TEST_FIXTURE(OrangePipeDetectorFixture, PipeTrackingAndLoss)
// {
//     // Make two pipes
//     vision::makeColor(&input, 0, 0, 255);
//     drawSquare(&input, 160, 240, 150, 50, 70, CV_RGB(230,180,40));
//     drawSquare(&input, 480, 240, 150, 50, 70, CV_RGB(230,180,40));

//     // Find the pipes and check the IDs
//     processImage(&input);
  
//     // Check the IDs of the event
//     CHECK_EQUAL(2u, pipes.size());
//     int minId = pipes[0].getId();
//     int maxId = pipes[1].getId();
    
//     if (maxId < minId)
//         std::swap(maxId, minId);
    
//     CHECK_EQUAL(0, minId);
//     CHECK_EQUAL(1, maxId);

//     // Determine the ID of the pipe on the left and right
//     vision::PipeDetector::Pipe leftPipe = pipes[0];
//     vision::PipeDetector::Pipe rightPipe = pipes[1];
//     int leftIdx = 0;
//     int rightIdx = 1;
//     if (pipes[1].getX() < pipes[0].getX())
//     {
//         std::swap(leftPipe, rightPipe);
//         std::swap(leftIdx, rightIdx);
//     }

//     /// TODO: Check for multiple found events
//     CHECK_EQUAL(2u, foundEvents.size());
//     CHECK_CLOSE(leftPipe.getX(), foundEvents[leftIdx]->x, 0.0005);
//     CHECK_CLOSE(leftPipe.getY(), foundEvents[leftIdx]->y, 0.0005);
//     CHECK_EQUAL(leftPipe.getId(), foundEvents[leftIdx]->id);
//     CHECK_CLOSE(rightPipe.getX(), foundEvents[rightIdx]->x, 0.0005);
//     CHECK_CLOSE(rightPipe.getY(), foundEvents[rightIdx]->y, 0.0005);
//     CHECK_EQUAL(rightPipe.getId(), foundEvents[rightIdx]->id);
    
//     // Draw the pipes slightly moved, and without the left one
//     vision::makeColor(&input, 0, 0, 255);
//     drawSquare(&input, 483,250, 150, 50, 70, CV_RGB(230,180,40));

//     // Make sure we still have the same ids
//     processImage(&input);

//     CHECK_EQUAL(1u, pipes.size());
//     CHECK_EQUAL(rightPipe.getId(), pipes[0].getId());

//     // Check out found events
//     CHECK_EQUAL(1u, foundEvents.size());
//     CHECK_CLOSE(pipes[0].getX(), foundEvents[0]->x, 0.0005);
//     CHECK_CLOSE(pipes[0].getY(), foundEvents[0]->y, 0.0005);
//     CHECK_EQUAL(rightPipe.getId(), foundEvents[0]->id);
    
//     // Check out lost events
//     CHECK_EQUAL(1u, droppedEvents.size());
//     CHECK_EQUAL(leftPipe.getId(), droppedEvents[0]->id);
// }


} // SUITE(OrangePipeDetector)
