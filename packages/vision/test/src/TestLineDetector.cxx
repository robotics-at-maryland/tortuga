/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/test/src/TestLineDetector.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "math/include/Vector2.h"
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
    math::Vector2 expectedPt2Vector = math::Vector2(0, 0);
    vision::Detector::imageToAICoordinates(&input, 515, 240,
                                           expectedPt1Vector.x,
                                           expectedPt1Vector.y);
    double expectedLength = (expectedPt2Vector - expectedPt1Vector).length();
    double expectedTheta = 0;
    //double expectedRho = ?; // might not even record this,it isn't very useful
    
    // Process it
    vision::OpenCVImage output(640, 480);
    detector.processImage(&input, &output);

    // Make sure one line was found
    CHECK_EQUAL(1u, detector.getLines().size());
    vision::LineDetector::Line line = detector.getLines()[0];

    CHECK_EQUAL(expectedPt1Vector, line.point1());
    CHECK_EQUAL(expectedPt2Vector, line.point2());
    CHECK_EQUAL(expectedLength, line.length());
    CHECK_EQUAL(expectedTheta, line.theta());
    //CHECK_EQUAL(expectedRho, line.rho);
}

} // SUITE(LineDetector)
