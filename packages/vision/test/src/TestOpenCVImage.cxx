/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestOpenCVImage.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vision/test/include/UnitTestChecks.h"
#include "vision/include/OpenCVImage.h"

void makeColor(ram::vision::Image* image, unsigned char R, unsigned char G,
               unsigned char B);

SUITE(OpenCVImage) {

TEST(copyFrom)
{
    ram::vision::OpenCVImage source(640, 480);
    ram::vision::OpenCVImage destination(10, 10);

    makeColor(&source, 255, 0, 0);

    // Copy image over
    destination.copyFrom(&source);
    
    // Should be exactly the same
    CHECK_CLOSE(&source, &destination,
                0);
}

} // SUITE(OpenCVImage)
