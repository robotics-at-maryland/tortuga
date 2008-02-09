/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestRedLightDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vision/include/RedLightDetector.h"
#include "vision/include/OpenCVImage.h"

using namespace ram;

void makeColor(vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B)
{
    int size = image->getWidth() * image->getHeight() * 3;
    unsigned char* data = image->getData();
    for (int i = 0; i < size; i += 3)
    {
        // BGR
        data[i] = B;
        data[i + 1]  = G;
        data[i + 2]  = R;
    }
}

void drawRedCircle(vision::Image* image, int x, int y, int radius = 50)
{
    CvPoint center;
    center.x = x;
    center.y = y;
    cvCircle(image->asIplImage(), center, radius, CV_RGB(255, 0, 0), -1);
}

SUITE(RedLightDetector) {

struct RedLightDetectorFixture
{
    RedLightDetectorFixture() :
        input(640, 480),
        detector(0)
    {}

    vision::OpenCVImage input;
    vision::RedLightDetector detector;
};
    
TEST_FIXTURE(RedLightDetectorFixture, CenterLight)
{
    // Blue Image with red circle in the center
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/2, 240);

    // Process it
    detector.processImage(&input, 0);

    CHECK_CLOSE(0, detector.getX(), 0.005);
    CHECK_CLOSE(0, detector.getY(), 0.005);
    CHECK(detector.found);
}

TEST_FIXTURE(RedLightDetectorFixture, UpperLeft)
{
    // Blue Image with red circle in upper left (remeber image rotated 90 deg)
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640 - (640/4), 480/4);
    
    detector.processImage(&input, 0);
    
    CHECK_CLOSE(-0.5, detector.getX(), 0.005);
    CHECK_CLOSE(0.5, detector.getY(), 0.005);
    CHECK(detector.found);
}

TEST_FIXTURE(RedLightDetectorFixture, LowerRight)
{
    // Blue Image with red circle in lower right (remeber image rotated 90 deg)
    makeColor(&input, 0, 0, 255);
    drawRedCircle(&input, 640/4, 480/4 * 3);
    
    detector.processImage(&input, 0);
    
    CHECK_CLOSE(0.5, detector.getX(), 0.005);
    CHECK_CLOSE(-0.5, detector.getY(), 0.005);
    CHECK(detector.found);
}

} // SUITE(RedLightDetector)
