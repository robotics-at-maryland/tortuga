/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestColorFilter.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/ColorFilter.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/Utility.h"
#include "vision/test/include/UnitTestChecks.h"

using namespace ram;


struct ColorFilterFixture
{
    ColorFilterFixture()
    {
    }
};

SUITE(ColorFilter) {
    
TEST_FIXTURE(ColorFilterFixture, CenterLight)
{
    // Generated expected input
    vision::OpenCVImage input(640, 480);
    // Gray background
    vision::makeColor(&input, 50, 50, 50); 
    // Circle to find
    vision::drawCircle(&input, 320, 240, 50, CV_RGB(180, 45, 230)); 
    // Extra circle
    vision::drawCircle(&input, 80, 80, 50, CV_RGB(250, 100, 50)); 

    // Draw the expected result
    vision::OpenCVImage expected(640, 480);
    vision::makeColor(&expected, 0, 0, 0); // Black
    vision::drawCircle(&expected, 320, 240, 50, CV_RGB(255, 255, 255));

    // Filter to threshold the image with
    vision::ColorFilter filter(100, 250, // B 
			       40, 60, // G
			       150, 200 // R
			       );

    // Filter image and check the result
    filter.filterImage(&input);

    // Check results
    CHECK_CLOSE(&expected, &input, 0);
}

} // SUITE(ColorFilter)
