
/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
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
    drawSquare(&input, 350, 400, 100, 10, 0, CV_RGB(255,255,255));

    // Process it
    vision::OpenCVImage output(640, 480);
    detector.processImage(&input, &output);

    // Make sure we found no blobs
    CHECK_EQUAL(2u, detector.getBlobs().size());

    vision::BlobDetector::Blob blob = detector.getBlobs()[1];
    CHECK_EQUAL(250, blob.getMaxX());
    CHECK_EQUAL(150, blob.getMinX());
    CHECK_EQUAL(300, blob.getMaxY());
    CHECK_EQUAL(100, blob.getMinY());
    CHECK_CLOSE(100 * 200, blob.getSize(), 500);
    CHECK_EQUAL(200, blob.getCenterX());
    CHECK_EQUAL(200, blob.getCenterY());


    vision::BlobDetector::Blob blob2 = detector.getBlobs()[0];
    CHECK_EQUAL(400, blob2.getMaxX());
    CHECK_EQUAL(300, blob2.getMinX());
    CHECK_EQUAL(405, blob2.getMaxY());
    CHECK_EQUAL(395, blob2.getMinY());
    CHECK_CLOSE(100 * 10, blob2.getSize(), 500);
    CHECK_EQUAL(350, blob2.getCenterX());
    CHECK_EQUAL(400, blob2.getCenterY());
}

TEST_FIXTURE(BlobDetectorFixture, circleBlobs)
{
    // Make image black
    vision::makeColor(&input, 0, 0, 0);

    // Plain circle
    drawCircle(&input, 200, 250, 100, CV_RGB(255,255,255));
    
    // Process it
    vision::OpenCVImage output(640, 480);
    detector.processImage(&input, &output);

    // Make sure we found no blobs
    CHECK_EQUAL(1u, detector.getBlobs().size());
    
    vision::BlobDetector::Blob blob2 = detector.getBlobs()[0];
    CHECK_EQUAL(300, blob2.getMaxX());
    CHECK_EQUAL(100, blob2.getMinX());
    CHECK_EQUAL(350, blob2.getMaxY());
    CHECK_EQUAL(150, blob2.getMinY());
    CHECK_CLOSE(31456, blob2.getSize(), 500);
    CHECK_EQUAL(200, blob2.getCenterX());
    CHECK_EQUAL(250, blob2.getCenterY());
}

TEST_FIXTURE(BlobDetectorFixture, complexBlobs)
{
    // Make image black
    vision::makeColor(&input, 0, 0, 0);

    // Joined circles
    drawCircle(&input, 500, 250, 65, CV_RGB(255,255,255));
    drawCircle(&input, 550, 300, 65, CV_RGB(255,255,255));

    // Square with center cut out
    drawSquare(&input, 200, 200, 100, 200, 0, CV_RGB(255,255,255));
    drawSquare(&input, 200, 200, 50, 100, 0, CV_RGB(0,0,0));

    // Circle with a chunk cut out
    drawCircle(&input, 100, 390, 80, CV_RGB(255,255,255));
    drawSquare(&input, 100, 360, 40, 100, 0, CV_RGB(0,0,0));
    
    // Process it
    vision::OpenCVImage output(640, 480);
    detector.processImage(&input, &output);

    // Make sure we found no blobs
    CHECK_EQUAL(3u, detector.getBlobs().size());

    // Circle with chunk cut out
    vision::BlobDetector::Blob blob = detector.getBlobs()[0];
    CHECK_EQUAL(180, blob.getMaxX());
    CHECK_EQUAL(20, blob.getMinX());
    CHECK_EQUAL(470, blob.getMaxY());
    CHECK_EQUAL(313, blob.getMinY());
    CHECK_CLOSE(16002, blob.getSize(), 500);
    CHECK_EQUAL(100, blob.getCenterX());
    CHECK_EQUAL(397, blob.getCenterY());

    // Check the double circles
    blob = detector.getBlobs()[1];
    CHECK_EQUAL(615, blob.getMaxX());
    CHECK_EQUAL(435, blob.getMinX());
    CHECK_EQUAL(365, blob.getMaxY());
    CHECK_EQUAL(185, blob.getMinY());
    CHECK_CLOSE(22000, blob.getSize(), 300);
    CHECK_EQUAL(525, blob.getCenterX());
    CHECK_EQUAL(275, blob.getCenterY());

    // Holed out square
    blob = detector.getBlobs()[2];
    CHECK_EQUAL(250, blob.getMaxX());
    CHECK_EQUAL(150, blob.getMinX());
    CHECK_EQUAL(300, blob.getMaxY());
    CHECK_EQUAL(100, blob.getMinY());
    CHECK_CLOSE((100 * 200) - (50 * 100), blob.getSize(), 500);
    CHECK_EQUAL(200, blob.getCenterX());
    CHECK_EQUAL(200, blob.getCenterY());
}

TEST_FIXTURE(BlobDetectorFixture, manyBlobs)
{
    // Make image black
    vision::makeColor(&input, 0, 0, 0);

    // Draw a field of circles
    size_t expectedBlobs = 0;
    for (int x = 15; x < (int)(input.getWidth() - 11); x += 30)
    {
        for (int y = 15; y < (int)(input.getHeight() - 11); y += 30)
        {
            drawCircle(&input, x, y, 10, CV_RGB(255,255,255));
            expectedBlobs++;
        }
    }
    
    // Process it
    vision::OpenCVImage output(640, 480);
    detector.processImage(&input, &output);

    // Make sure we picked up all the blobs
    CHECK_EQUAL(expectedBlobs, detector.getBlobs().size());
}

TEST_FIXTURE(BlobDetectorFixture, minBlobSize)
{
    vision::BlobDetector detectorMinSize(
        core::ConfigNode::fromString("{\"minBlobSize\" : 5000 }"));
    
    // Make image black
    vision::makeColor(&input, 0, 0, 0);

    // Add some white rectangles
    drawSquare(&input, 200, 200, 100, 200, 0, CV_RGB(255,255,255));
    drawSquare(&input, 350, 400, 100, 10, 0, CV_RGB(255,255,255));

    // Process it
    vision::OpenCVImage output(640, 480);
    detectorMinSize.processImage(&input, &output);

    // Make sure we found just the big blob
    CHECK_EQUAL(1u, detectorMinSize.getBlobs().size());

    vision::BlobDetector::Blob blob = detectorMinSize.getBlobs()[0];
    CHECK_EQUAL(250, blob.getMaxX());
    CHECK_EQUAL(150, blob.getMinX());
    CHECK_EQUAL(300, blob.getMaxY());
    CHECK_EQUAL(100, blob.getMinY());
    CHECK_CLOSE(100 * 200, blob.getSize(), 500);
    CHECK_EQUAL(200, blob.getCenterX());
    CHECK_EQUAL(200, blob.getCenterY());
}

} // SUITE(BlobDetector)
