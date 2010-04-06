/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/test/src/TestLineDetector.cxx
 */

// STD Includes
#include <iostream>

// Library Includes
#include "highgui.h"
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/include/Vector2.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/Detector.h"
#include "vision/include/LineDetector.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/Utility.h"

using namespace ram;

struct LineDetectorFixture
{
    LineDetectorFixture() :
        input(640, 480),
        detector(core::ConfigNode::fromString("{}"))
    {
    }

    vision::OpenCVImage input;
    vision::LineDetector detector;
};

SUITE(LineDetector) {

TEST_FIXTURE(LineDetectorFixture, horizontalLine)
{
    vision::makeColor(&input, 0, 0, 0);

    // Add a horizontal line to the center of the screen
    vision::drawLine(&input, 150, 240, 515, 240, 3, CV_RGB(0, 255, 0));

    // Expected values
    math::Vector2 expectedPt1Vector = math::Vector2(0, 0);
    vision::Detector::imageToAICoordinates(&input, 150, 240,
                                           expectedPt1Vector.x,
                                           expectedPt1Vector.y);
    CvPoint expectedPt1;
    expectedPt1.x = expectedPt1Vector.x;
    expectedPt1.y = expectedPt1Vector.y;
    
    math::Vector2 expectedPt2Vector = math::Vector2(0, 0);
    vision::Detector::imageToAICoordinates(&input, 515, 240,
                                           expectedPt1Vector.x,
                                           expectedPt1Vector.y);
    CvPoint expectedPt2;
    expectedPt2.x = expectedPt2Vector.x;
    expectedPt2.y = expectedPt2Vector.y;
    
    double expectedLength = (expectedPt2Vector - expectedPt1Vector).length();
    math::Radian expectedTheta = math::Radian(CV_PI/2);
    double expectedRho = 0;

    // Color filter the image for white line, black background
    vision::ColorFilter filter(0, 0, 255, 255, 0, 0);
    vision::OpenCVImage single(640, 480, vision::Image::PF_GRAY_8);
    filter.filterImage(&input, &single);
    
    // Process it
    vision::OpenCVImage output(640, 480, vision::Image::PF_GRAY_8);
    detector.processImage(&single, &output);

    cvStartWindowThread();
    cvNamedWindow("Filtered");
    cvShowImage("Filtered", output.asIplImage());
    cvWaitKey(0);
    cvDestroyWindow("Filtered");

    // Make sure two lines were found
    CHECK_EQUAL(2u, detector.getLines().size());
    int i = 1;
    BOOST_FOREACH(vision::LineDetector::Line line, detector.getLines())
    {
        std::cout << "Line " << i++ << ":\n"
                  << "Rho: " << line.rho() << "\n"
                  << "Theta: " << line.theta() << "\n"
                  << "Point1: " << "(" << line.point1().x
                  << ", " << line.point1().y << ")" << "\n"
                  << "Point2: " << "(" << line.point2().x
                  << ", " << line.point2().y << ")" << std::endl;
    }

    vision::LineDetector::Line line = detector.getLines()[0];

    CHECK_EQUAL(expectedPt1.x, line.point1().x);
    CHECK_EQUAL(expectedPt1.y, line.point1().y);
    CHECK_EQUAL(expectedPt2.x, line.point2().x);
    CHECK_EQUAL(expectedPt2.y, line.point2().y);
    CHECK_EQUAL(expectedLength, line.length());
    CHECK_EQUAL(expectedTheta, line.theta());
    CHECK_EQUAL(expectedRho, line.rho());
}

} // SUITE(LineDetector)
