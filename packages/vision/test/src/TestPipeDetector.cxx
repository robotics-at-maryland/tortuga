/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/test/src/TestPipeDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <cv.h>
// Project Includes
#include "vision/include/PipeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"

#include "core/include/EventHub.h"

#include "vision/test/include/Utility.h"

using namespace ram;

struct PipeDetectorFixture
{
    PipeDetectorFixture() :
        input(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
    }

    void processImage(vision::Image* image, bool show = false)
    {
        if (show)
	{
	    vision::Image::showImage(image, "Input");

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

    vision::PipeDetector::PipeList pipes;
    vision::OpenCVImage input;
    core::EventHubPtr eventHub;
    vision::PipeDetector detector;
};

SUITE(PipeDetector) {

// TODO Test upright, and onside angle
TEST_FIXTURE(PipeDetectorFixture, UpperLeft)
{
    // Blue Image with  rectangle in it
    vision::makeColor(&input, 0, 0, 0);
    // draw  square (upper left, remember image rotated 90 deg)
    drawSquare(&input, 640/4, 480/4, 50, 230, 25, CV_RGB(255,255,255));

    // Process it
    processImage(&input);
    
    double expectedX = -0.5 * 640.0/480.0;
    double expectedY = 0.5;
    math::Degree expectedAngle(25);
    
    CHECK(detector.found());
    CHECK_EQUAL(1u, pipes.size());

    vision::PipeDetector::Pipe pipe = pipes[0];
    CHECK_CLOSE(expectedX, pipe.getX(), 0.05);
    CHECK_CLOSE(expectedY, pipe.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, pipe.getAngle(), math::Degree(2));
    /// TODO: get hough working to improve accuracy of this to +/- 0.5 degrees
}

TEST_FIXTURE(PipeDetectorFixture, Left)
{
    // Blue Image with  rectangle in it
    vision::makeColor(&input, 0, 0, 0);
    // draw  square (left side)
    drawSquare(&input, 640/4, 480/2, 50, 230, 0, CV_RGB(255,255,255));

    // Process it
    processImage(&input);
    
    double expectedX = -0.5 * 640.0/480.0; 
    double expectedY = 0;
    math::Degree expectedAngle(0);
    
    CHECK(detector.found());
    CHECK_EQUAL(1u, pipes.size());

    vision::PipeDetector::Pipe pipe = pipes[0];
    CHECK_CLOSE(expectedX, pipe.getX(), 0.05);
    CHECK_CLOSE(expectedY, pipe.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, pipe.getAngle(), math::Degree(2));
    /// TODO: get hough working to improve accuracy of this to +/- 0.5 degrees
}

TEST_FIXTURE(PipeDetectorFixture, LowerRight)
{
    // Blue Image with  rectangle in it
    vision::makeColor(&input, 0, 0, 0);
    // draw  square (upper left)
    drawSquare(&input, 640 - 640/4, 480/4 * 3,
               50, 230, -25, CV_RGB(255,255,255));

    // Process it
    processImage(&input);
    
    double expectedX = 0.5 * 640.0/480.0; 
    double expectedY = -0.5;
    math::Degree expectedAngle(-25);
    
    CHECK(detector.found());
    CHECK_EQUAL(1u, pipes.size());

    vision::PipeDetector::Pipe pipe = pipes[0];
    CHECK_CLOSE(expectedX, pipe.getX(), 0.05);
    CHECK_CLOSE(expectedY, pipe.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, pipe.getAngle(), math::Degree(2));
    /// TODO: get hough working to improve accuracy of this to +/- 0.5 degrees
}

TEST_FIXTURE(PipeDetectorFixture, CenterUp)
{
    // Blue Image with  rectangle in it
    vision::makeColor(&input, 0, 0, 0);
    // draw  square in center sideways
    drawSquare(&input, 640/2, 480/2, 50, 230, 0, CV_RGB(255,255,255));

    // Process it
    processImage(&input);

    double expectedX = 0;
    double expectedY = 0 * 640.0/480.0; 
    math::Degree expectedAngle(0);
    
    CHECK(detector.found());
    CHECK_EQUAL(1u, pipes.size());

    vision::PipeDetector::Pipe pipe = pipes[0];
    CHECK_CLOSE(expectedX, pipe.getX(), 0.05);
    CHECK_CLOSE(expectedY, pipe.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, pipe.getAngle(), math::Degree(2));
    /// TODO: get hough working to improve accuracy of this to +/- 0.5 degrees
}

TEST_FIXTURE(PipeDetectorFixture, CenterSideways)
{
    // Blue Image with  rectangle in it
    vision::makeColor(&input, 0, 0, 0);
    // draw  square in center sideways
    drawSquare(&input, 640/2, 480/2, 50, 230, 90, CV_RGB(255,255,255));
    
    // Process it
    processImage(&input);
    
    double expectedX = 0;
    double expectedY = 0 * 640.0/480.0; 
    math::Degree expectedAngle(-90);
    
    CHECK(detector.found());
    CHECK_EQUAL(1u, pipes.size());

    vision::PipeDetector::Pipe pipe = pipes[0];
    CHECK_CLOSE(expectedX, pipe.getX(), 0.05);
    CHECK_CLOSE(expectedY, pipe.getY(), 0.05);
    //CHECK_CLOSE(expectedAngle, pipe.getAngle(), math::Degree(0.5));
}

TEST_FIXTURE(PipeDetectorFixture, UpperLeftNoHough)
{
    // Disable hough to use blob based system instead
    detector.setHough(true);

    // Blue Image with  rectangle in it
    vision::makeColor(&input, 0, 0, 0);
    // draw  square (upper left, remember image rotated 90 deg)
    drawSquare(&input, 640/4, 480/4, 50, 230, 25, CV_RGB(255,255,255));

    // Process it
    processImage(&input);
    
    double expectedX = -0.5 * 640.0/480.0;
    double expectedY = 0.5;
    math::Degree expectedAngle(25);
    
    CHECK(detector.found());
    CHECK_EQUAL(1u, pipes.size());

    vision::PipeDetector::Pipe pipe = pipes[0];
    CHECK_CLOSE(expectedX, pipe.getX(), 0.05);
    CHECK_CLOSE(expectedY, pipe.getY(), 0.05);
    CHECK_CLOSE(expectedAngle, pipe.getAngle(), math::Degree(2));
    /// TODO: see why this isn't +/- 1 degree
}

} // SUITE(PipeDetector)
