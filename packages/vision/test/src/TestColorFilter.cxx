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
    
TEST_FIXTURE(ColorFilterFixture, CenterLightInPlace)
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

TEST_FIXTURE(ColorFilterFixture, CenterLightCopy)
{
    // Generated expected input
    vision::OpenCVImage input(640, 480);
    vision::OpenCVImage inputBase(640, 480);
    // Gray background
    vision::makeColor(&input, 50, 50, 50); 
    // Circle to find
    vision::drawCircle(&input, 320, 240, 50, CV_RGB(180, 45, 230)); 
    // Extra circle
    vision::drawCircle(&input, 80, 80, 50, CV_RGB(250, 100, 50)); 
    // What are input looked like
    inputBase.copyFrom(&input);
    
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
    vision::OpenCVImage output(640, 480);
    filter.filterImage(&input, &output);

    // Check results
    CHECK_CLOSE(&expected, &output, 0);
    CHECK_CLOSE(&inputBase, &input, 0);
}


TEST_FIXTURE(ColorFilterFixture, testGetSet)
{
    vision::ColorFilter filter(0, 0, 0, 0, 0, 0);

    filter.setChannel1High(100);
    filter.setChannel2High(120);
    filter.setChannel3High(140);
    filter.setChannel1Low(20);
    filter.setChannel2Low(40);
    filter.setChannel3Low(60);

    CHECK_EQUAL(20, filter.getChannel1Low());
    CHECK_EQUAL(40, filter.getChannel2Low());
    CHECK_EQUAL(60, filter.getChannel3Low());
    CHECK_EQUAL(100, filter.getChannel1High());
    CHECK_EQUAL(120, filter.getChannel2High());
    CHECK_EQUAL(140, filter.getChannel3High());

    // Test to make sure it keeps the low values within range
    filter.setChannel1Low(110);
    filter.setChannel2Low(130);
    filter.setChannel3Low(140);

    CHECK_EQUAL(100, filter.getChannel1Low());
    CHECK_EQUAL(120, filter.getChannel2Low());
    CHECK_EQUAL(140, filter.getChannel3Low());
    CHECK_EQUAL(100, filter.getChannel1High());
    CHECK_EQUAL(120, filter.getChannel2High());
    CHECK_EQUAL(140, filter.getChannel3High());

    filter.setChannel1High(20);
    filter.setChannel2High(40);
    filter.setChannel3High(60);

    CHECK_EQUAL(20, filter.getChannel1Low());
    CHECK_EQUAL(40, filter.getChannel2Low());
    CHECK_EQUAL(60, filter.getChannel3Low());
    CHECK_EQUAL(20, filter.getChannel1High());
    CHECK_EQUAL(40, filter.getChannel2High());
    CHECK_EQUAL(60, filter.getChannel3High());
}

TEST_FIXTURE(ColorFilterFixture, testSingleChannel)
{
    // Generated expected input
    vision::OpenCVImage input(640, 480, vision::Image::PF_RGB_8);
    // Gray background
    vision::makeColor(&input, 50, 50, 50); 
    // Circle to find
    vision::drawCircle(&input, 320, 240, 50, CV_RGB(180, 45, 230)); 
    // Extra circle
    vision::drawCircle(&input, 80, 80, 50, CV_RGB(250, 100, 50)); 

    // Draw the expected result
    vision::OpenCVImage expected(640, 480, vision::Image::PF_RGB_8);
    vision::makeColor(&expected, 0, 0, 0); // Black
    vision::drawCircle(&expected, 320, 240, 50, CV_RGB(255, 255, 255));
    expected.setPixelFormat(vision::Image::PF_GRAY_8);

    // Filter to threshold the image width
    vision::ColorFilter filter(100, 250, // B 
			       40, 60, // G
			       150, 200 // R
			       );

    // Filter image and check the result
    vision::OpenCVImage output(640, 480, vision::Image::PF_GRAY_8);
    filter.filterImage(&input, &output);

    // Check results
    CHECK_CLOSE(&expected, &output, 0);
}


} // SUITE(ColorFilter)
