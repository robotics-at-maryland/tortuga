/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestDetector.cxx
 */

// STD Includes
#include <utility>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Test Includes
#include "vision/include/Detector.h"
#include "vision/include/OpenCVImage.h"

using namespace ram;
SUITE(Detector) {

TEST(imageToAICoordinates)
{
    vision::OpenCVImage image(640, 480);
    double outX = 0;
    double outY = 0;

    // Center
    vision::Detector::imageToAICoordinates(&image, 640/2, 480/2, outX, outY);
    double expectedX = 0 * 640.0/480.0;
    double expectedY = 0;
    CHECK_CLOSE(expectedX, outX, 0.001);
    CHECK_CLOSE(expectedY, outY, 0.001);

    // Upper Left
    vision::Detector::imageToAICoordinates(&image, 640/4, 480/4, outX, outY);
    expectedX = -0.5 * 640.0/480.0;
    expectedY = 0.5;
    CHECK_CLOSE(expectedX, outX, 0.001);
    CHECK_CLOSE(expectedY, outY, 0.001);

    // Lower Right
    vision::Detector::imageToAICoordinates(&image, 640 - 640/4, 480/4 * 3,
					   outX, outY);
    expectedX = 0.5 * 640.0/480.0;
    expectedY = -0.5;
    CHECK_CLOSE(expectedX, outX, 0.001);
    CHECK_CLOSE(expectedY, outY, 0.001);
}

} // SUITE(Detector)
