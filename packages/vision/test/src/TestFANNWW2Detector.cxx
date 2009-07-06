/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 David Love
 * All rights reserved.
 *
 * Author: David Love <loved@umd.edu>
 * File:  packages/vision/test/src/TestFANNSymbolDetector.cpp
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "core/include/ConfigNode.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/FANNWW2Detector.h"
#include "vision/test/include/Utility.h"

using namespace ram;

SUITE(FANNWW2Detector) {

struct FANNSymbolDetectorFixture
{
    FANNSymbolDetectorFixture() :
        detector(core::ConfigNode::fromString("{'training' : 1}"))
    {}

    vision::FANNWW2Detector detector;
};

    
TEST_FIXTURE(FANNSymbolDetectorFixture, needSquareCropped)
{
    CHECK_EQUAL(false, detector.needSquareCropped());
}

TEST_FIXTURE(FANNSymbolDetectorFixture, getNumberFeatures)
{
    CHECK_EQUAL(3, detector.getNumberFeatures());
}

TEST_FIXTURE(FANNSymbolDetectorFixture, getOutputCount)
{
    CHECK_EQUAL(4, detector.getOutputCount());
}

TEST_FIXTURE(FANNSymbolDetectorFixture, getImageFeatures)
{
    // Test Wide images, with square in three corners
    vision::OpenCVImage src(192, 96);
    vision::makeColor(&src, 0, 0, 0);
    vision::drawSquare(&src, 12, 12, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src, 12, 83, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src, 180, 12, 24, 24, 0.0, CV_RGB(255,255,255));
    
    float expectedFeatures[] = {2.0, 2.0, 0.75};
    float results[] = {0, 0, 0};

    CHECK_EQUAL(192u, src.getWidth());
    CHECK_EQUAL(96u, src.getHeight());
    detector.getImageFeatures(&src, results);
    CHECK_ARRAY_CLOSE(expectedFeatures, results, 3, 0.1);

    
    // Test tall images, with square in three corners
    vision::OpenCVImage src2(96, 192);
    vision::makeColor(&src2, 0, 0, 0);
    vision::drawSquare(&src2, 12, 12, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src2, 84, 12, 24, 24, 0.0, CV_RGB(255,255,255));
    vision::drawSquare(&src2, 12, 180, 24, 24, 0.0, CV_RGB(255,255,255));
    
    float expectedFeatures2[] = {2.0, 2.0, 0.75};
    float results2[] = {0, 0, 0};

    CHECK_EQUAL(96u, src2.getWidth());
    CHECK_EQUAL(192u, src2.getHeight());
    detector.getImageFeatures(&src2, results2);
    CHECK_ARRAY_CLOSE(expectedFeatures2, results2, 3, 0.1);

}

} // SUITE(FANNWW2Detector)
