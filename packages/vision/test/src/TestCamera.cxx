/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestCamera.cxx
 */

#ifdef RAM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"

#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/MockCamera.h"

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "images" /
        "testcamera";
}

using namespace ram::vision;

SUITE(Camera) {

TEST(getImage)
{
    Image* expected = new OpenCVImage((getImagesDir() / "A.jpg").string());
    Image* result = new OpenCVImage(expected->getWidth(),
                                    expected->getHeight());
    
    MockCamera camera(expected);
    camera.update(0);
    camera.getImage(result);

    // Should be exactly the same
    CHECK_CLOSE(*expected, *result, 0);
}

TEST(copyToPublic)
{
    Image* imageA = new OpenCVImage((getImagesDir() / "A.jpg").string());
    Image* expected = new OpenCVImage((getImagesDir() / "B.jpg").string());
    Image* result = new OpenCVImage(expected->getWidth(),
                                    expected->getHeight());
    
    MockCamera camera(imageA, expected);
    camera.update(0);
    camera.getImage(result);

    // Should be exactly the same
    CHECK_CLOSE(*expected, *result, 0);
}

struct CameraFixture
{
    CameraFixture() :
        rawImage(new OpenCVImage(10,10)),
        capturedImage(new OpenCVImage(10,10))
    {}
    
    ~CameraFixture()
    {
        delete rawImage;
        delete capturedImage;
    }

    void rawCaptureHandler(ram::core::EventPtr event)
    {
        rawImage->copyFrom(
            boost::dynamic_pointer_cast<ImageEvent>(event)->image);
    }

    void captureHandler(ram::core::EventPtr event)
    {
        capturedImage->copyFrom(
            boost::dynamic_pointer_cast<ImageEvent>(event)->image);
    }

    Image* rawImage;
    Image* capturedImage;
};

TEST_FIXTURE(CameraFixture, event_RAW_IMAGE_CAPTURED_and_IMAGE_CAPTURED)
{
    Image* expectedRaw = new OpenCVImage((getImagesDir() / "A.jpg").string());
    Image* expectedCap = new OpenCVImage((getImagesDir() / "B.jpg").string());
    
    MockCamera camera(expectedRaw, expectedCap);
    camera.subscribe(ram::vision::Camera::RAW_IMAGE_CAPTURED,
                     boost::bind(&CameraFixture::rawCaptureHandler, this, _1));
    camera.subscribe(ram::vision::Camera::IMAGE_CAPTURED,
                     boost::bind(&CameraFixture::captureHandler, this, _1));

    // Fire off events, then change handlers
    camera.update(0);
    CHECK_CLOSE(*expectedRaw, *rawImage, 0);
    CHECK_CLOSE(*expectedCap, *capturedImage, 0);
}

} // SUITE(Camera)
