/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Nathan Davidge <ndavidge@gmail.com>
 * All rights reserved.
 *
 * Author: Nathan Davidge <ndavidge@gmail.com>
 * File:  packages/vision/test/src/TestVelocityDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <cv.h>

// Project Includes
#include "vision/include/VelocityDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"

#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

#include "vision/test/include/Utility.h"

#include "math/test/include/MathChecks.h"

//static boost::filesystem::path getImagesDir()
//{
//    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
//    return root / "packages" / "vision" / "test" / "data" / "images";
//}

using namespace ram;

struct VelocityDetectorFixture
{
    VelocityDetectorFixture() :
        input1(640, 480),
        input2(640, 480),
        eventHub(new core::EventHub()),
        detector(core::ConfigNode::fromString("{}"), eventHub)
    {
    }

    void determineVelocity(bool show = false)
    {
        if (show)
	{
	    vision::Image::showImage(&input1, "Input 1");
	    vision::Image::showImage(&input2, "Input 2");

	    detector.processImage(&input1);

	    vision::OpenCVImage output(640, 480);
	    detector.processImage(&input2, &output);
	    vision::Image::showImage(&output, "Output");
	}
	else
        {
            detector.processImage(&input1);
            detector.processImage(&input2);
	}
    }

    vision::OpenCVImage input1;
    vision::OpenCVImage input2;
    core::EventHubPtr eventHub;
    vision::VelocityDetector detector;
};

SUITE(VelocityDetector) {

TEST_FIXTURE(VelocityDetectorFixture, UpperLeft)
{
    // Black backgrounds with  rectangle in it
    vision::makeColor(&input1, 0, 0, 0);
    vision::makeColor(&input2, 0, 0, 0);

    // First image square in center
    drawSquare(&input1, 320, 240, 100, 100, 0, CV_RGB(255,255,255));
    // Second image upper left -25 on x, -50 on y
    drawSquare(&input2, 320 - 25, 240 - 50, 100, 100, 0, CV_RGB(255,255,255));

    math::Vector2 expectedVelocity(25, -50);

    // Run the detector
    detector.usePhaseCorrelation();
    determineVelocity();
    CHECK_CLOSE(expectedVelocity, detector.getVelocity(), 1.0);
    
    detector.useLKFlow();
    determineVelocity();
    CHECK_CLOSE(expectedVelocity, detector.getVelocity(), 1.0);    
}

TEST_FIXTURE(VelocityDetectorFixture, UpperRight)
{
    // Black backgrounds with  rectangle in it
    vision::makeColor(&input1, 0, 0, 0);
    vision::makeColor(&input2, 0, 0, 0);

    // First image square in center
    drawSquare(&input1, 320, 240, 100, 100, 0, CV_RGB(255,255,255));
    // Second image upper right +25 on x, -50 on y
    drawSquare(&input2, 320 + 25, 240 - 50, 100, 100, 0, CV_RGB(255,255,255));

    math::Vector2 expectedVelocity(-25, -50);

    // Run the detector
    detector.usePhaseCorrelation();
    determineVelocity();
    CHECK_CLOSE(expectedVelocity, detector.getVelocity(), 1.0);
    
    detector.useLKFlow();
    determineVelocity();
    CHECK_CLOSE(expectedVelocity, detector.getVelocity(), 1.0);
}

TEST_FIXTURE(VelocityDetectorFixture, LowerLeft)
{
    // Black backgrounds with  rectangle in it
    vision::makeColor(&input1, 0, 0, 0);
    vision::makeColor(&input2, 0, 0, 0);

    // First image square in center
    drawSquare(&input1, 320, 240, 100, 100, 0, CV_RGB(255,255,255));
    // Second image lower left -25 on x, +50 on y
    drawSquare(&input2, 320 - 25, 240 + 50, 100, 100, 0, CV_RGB(255,255,255));

    math::Vector2 expectedVelocity(25, 50);

    // Run the detector
    detector.usePhaseCorrelation();
    determineVelocity();
    CHECK_CLOSE(expectedVelocity, detector.getVelocity(), 1.0);
    
    detector.useLKFlow();
    determineVelocity();
    CHECK_CLOSE(expectedVelocity, detector.getVelocity(), 1.0);
}

TEST_FIXTURE(VelocityDetectorFixture, LowerRight)
{
    // Black backgrounds with  rectangle in it
    vision::makeColor(&input1, 0, 0, 0);
    vision::makeColor(&input2, 0, 0, 0);

    // First image square in center
    drawSquare(&input1, 320, 240, 100, 100, 0, CV_RGB(255,255,255));
    // Second image lower right +25 on x, +50 on y
    drawSquare(&input2, 320 + 25, 240 + 50, 100, 100, 0, CV_RGB(255,255,255));

    math::Vector2 expectedVelocity(-25, 50);

    // Run the detector
    detector.usePhaseCorrelation();
    determineVelocity();
    CHECK_CLOSE(expectedVelocity, detector.getVelocity(), 1.0);
    
    detector.useLKFlow();
    determineVelocity();
    CHECK_CLOSE(expectedVelocity, detector.getVelocity(), 1.0);
}


} // SUITE(VelocityDetector)
