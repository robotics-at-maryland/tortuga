/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestChecks.cxx
 */

#ifdef RAM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif

// STD Includes
#include <cstdlib>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/filesystem.hpp>

// Project Includes
#include "vision/include/OpenCVImage.h"
#include "vision/test/include/UnitTestChecks.h"

// Bring image classes into scope
using namespace ram::vision;

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "images" /
        "testchecks";
}

TEST(ImageAreClose)
{
    Image* a = new OpenCVImage((getImagesDir() / "A.jpg").string());
    Image* b = new OpenCVImage((getImagesDir() / "ACompressed.jpg").string());

    // This compares this image with a compressed version
    CHECK_CLOSE(*a, *b, 6.0);

    // Comparision to self should yield no difference
    CHECK_CLOSE(*a, *a, 0);
    
    delete a;
    delete b;
}
