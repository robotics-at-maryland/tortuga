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
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"

SUITE(OpenCVImage) {

TEST(copyFrom)
{
    ram::vision::OpenCVImage source(640, 480);
    ram::vision::OpenCVImage destination(10, 10);

    ram::vision::makeColor(&source, 255, 0, 0);

    // Copy image over
    destination.copyFrom(&source);
    
    // Should be exactly the same
    CHECK_CLOSE(&source, &destination,
                0);
}

} // SUITE(OpenCVImage)
