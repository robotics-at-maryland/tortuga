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

// Project Includes
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
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

} // SUITE(Camera)
