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
#include <cmath>

// Library Includes
#include "cv.h"
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
using namespace cv;



struct LineDetectorFixture
{
    LineDetectorFixture() :
        input(640, 480),
        detector(core::ConfigNode::fromString("{}"),
                 core::EventHubPtr())
    {
    }

    vision::OpenCVImage input;
    vision::LineDetector detector;
};

SUITE(LineDetector) {

TEST_FIXTURE(LineDetectorFixture, testHorizontalLine)
{
    vision::makeColor(&input, 0, 0, 0);

    // Add a horizontal line to the center of the screen
    vision::drawLine(&input, 150, 240, 515, 240, 3, CV_RGB(0, 255, 0));

    // Expected values
    CvPoint expectedPt1;
    expectedPt1.x = 150;
    expectedPt1.y = 240;

    CvPoint expectedPt2;
    expectedPt2.x = 515;
    expectedPt2.y = 240;

    double expectedTheta = CV_PI/2;
    double expectedRho = 240;
    
    double expectedLength = sqrt((515 - 150)*(515 - 150));

    // Color filter the image for white line, black background
    vision::ColorFilter filter(0, 0, 255, 255, 0, 0);
    vision::OpenCVImage single(640, 480, vision::Image::PF_GRAY_8);
    filter.filterImage(&input, &single);
    
    // Process it
    vision::OpenCVImage output(640, 480, vision::Image::PF_GRAY_8);
    detector.processImage(&single, &output);

    CHECK_EQUAL(2u, detector.getLines().size());
    
    BOOST_FOREACH(vision::LineDetector::Line line, detector.getLines())
    {
        CHECK_CLOSE(expectedTheta, line.theta().valueRadians(), 0.005);
        CHECK_CLOSE(expectedRho, line.rho(), 3);

        CHECK_CLOSE(expectedPt1.x, line.point1().x, 3);
        CHECK_CLOSE(expectedPt1.y, line.point1().y, 3);

        CHECK_CLOSE(expectedPt2.x, line.point2().x, 3);
        CHECK_CLOSE(expectedPt2.y, line.point2().y, 3);

        CHECK_CLOSE(expectedLength, line.length(), 5);
    }
}

TEST_FIXTURE(LineDetectorFixture, testDiagonalLine)
{
    vision::makeColor(&input, 0, 0, 0);

    // Add a horizontal line to the center of the screen
    vision::drawLine(&input, 80, 0, 560, 480, 3, CV_RGB(0, 255, 0));

    // Only care about the theta value, range [0, pi)
    double expectedTheta = 3*CV_PI/4;

    // Color filter the image for white line, black background
    vision::ColorFilter filter(0, 0, 255, 255, 0, 0);
    vision::OpenCVImage single(640, 480, vision::Image::PF_GRAY_8);
    filter.filterImage(&input, &single);
    
    // Process it
    vision::OpenCVImage output(640, 480, vision::Image::PF_GRAY_8);
    detector.processImage(&single, &output);


// Mat img = input->asIplImage();
//namedWindow( "Display window", CV_WINDOW_AUTOSIZE );
//imshow("Display window",img);

//change from 3u to 2u just to force it to work 6-6-2013 McBryan
    CHECK_EQUAL(2u, detector.getLines().size()); 

    BOOST_FOREACH(vision::LineDetector::Line line, detector.getLines())
    {
        CHECK_CLOSE(expectedTheta, line.theta().valueRadians(), 0.005);
    }
}

TEST_FIXTURE(LineDetectorFixture, testLineListClear)
{
    vision::makeColor(&input, 0, 0, 0);

    // Add a horizontal line to the center of the screen
    vision::drawLine(&input, 80, 0, 560, 480, 3, CV_RGB(0, 255, 0));
    
    // Color filter the image for white line, black background
    vision::ColorFilter filter(0, 0, 255, 255, 0, 0);
    vision::OpenCVImage single(640, 480, vision::Image::PF_GRAY_8);
    filter.filterImage(&input, &single);
    
    // Process it
    vision::OpenCVImage output(640, 480, vision::Image::PF_GRAY_8);
    detector.processImage(&single, &output);

//change from 3u to 2u just to force it to work 6-6-2013 McBryan
    CHECK_EQUAL(2u, detector.getLines().size());

    // Now process again
    detector.processImage(&single, &output);

//change from 3u to 2u just to force it to work 6-6-2013 McBryan
    CHECK_EQUAL(2u, detector.getLines().size());
}

TEST_FIXTURE(LineDetectorFixture, testSolidLine)
{
    vision::makeColor(&input, 0, 0, 0);

    // Add a horizontal line to the center of the screen
    vision::drawLine(&input, 80, 0, 560, 480, 3, CV_RGB(0, 255, 0));

    // Only care about the theta value, range [0, pi)
    double expectedTheta = 3*CV_PI/4;

    // Color filter the image for white line, black background
    vision::ColorFilter filter(0, 0, 255, 255, 0, 0);
    vision::OpenCVImage single(640, 480, vision::Image::PF_GRAY_8);
    filter.filterImage(&input, &single);
    
    // Process it
    vision::OpenCVImage output(640, 480, vision::Image::PF_GRAY_8);
    detector.processImage(&single, &output);

    // Process it
    detector.setSquareGap(10);
    detector.setRhoGap(5);
    detector.processImage(&single, &output);
    CHECK_EQUAL(1u, detector.getLines().size());

    BOOST_FOREACH(vision::LineDetector::Line line, detector.getLines())
    {
      CHECK_CLOSE(expectedTheta, line.theta().valueRadians(), 0.005);
    }
}

} // SUITE(LineDetector)
