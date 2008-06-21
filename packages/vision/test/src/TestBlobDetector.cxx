
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

// Project Includes
#include "vision/include/BlobDetector.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/Utility.h"

using namespace ram;

struct BlobDetectorFixture
{
    BlobDetectorFixture() :
        input(640, 480),
        detector(core::ConfigNode::fromString("{}"))
    {
    }

    vision::OpenCVImage input;
    vision::BlobDetector detector;
};

SUITE(BlobDetector) {

TEST_FIXTURE(BlobDetectorFixture, noBlobs)
{
    // Empty black image
    vision::makeColor(&input, 0, 0, 0);

    // Process it
    vision::OpenCVImage output(640, 480);
    detector.processImage(&input, &output);
    //vision::Image::showImage(&output);

    // Make sure we found no blobs
    CHECK_EQUAL(0u, detector.getBlobs().size());
}

TEST_FIXTURE(BlobDetectorFixture, simpleBlobs)
{
    // Make image black
    vision::makeColor(&input, 0, 0, 0);

    // Add some white rectangles
    drawSquare(&input, 200, 200, 100, 200, 0, CV_RGB(255,255,255));
    //    drawSquare(&input, 350, 400, 100, 10, 0, CV_RGB(255,255,255));

    // Process it
    vision::OpenCVImage output(640, 480);
    detector.processImage(&input, &output);

    // Make sure we found no blobs
    CHECK_EQUAL(1u, detector.getBlobs().size());
    vision::BlobDetector::Blob blob = detector.getBlobs()[0];
    CHECK_EQUAL(250, blob.getMaxX());
	CHECK_EQUAL(150, blob.getMinX());
    CHECK_EQUAL(300, blob.getMaxY());
	CHECK_EQUAL(100, blob.getMinY());
	CHECK_CLOSE(100 * 200, blob.getSize(), 500);
	CHECK_EQUAL(200, blob.getCenterX());
	CHECK_EQUAL(200, blob.getCenterY());
}

} // SUITE(BlobDetector)
