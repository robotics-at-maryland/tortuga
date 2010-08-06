/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/test/src/TestConvert.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include "cv.h"

// Project Includes
#include "vision/include/Convert.h"
#include "vision/include/OpenCVImage.h"

using namespace ram;

SUITE(Convert)
{

TEST(test_rgb2xyz)
{
    // This test is only valid for NTSC RGB Model and D65 illuminant
    double gamma = 2.2;

    double ch1 = pow(0.5, gamma);
    double ch2 = pow(0.5, gamma);
    double ch3 = pow(0.0, gamma);

    vision::Convert::rgb2xyz(&ch1, &ch2, &ch3);
    
    CHECK_CLOSE(0.169843, ch1, 0.05);
    CHECK_CLOSE(0.192721, ch2, 0.05);
    CHECK_CLOSE(0.054385, ch3, 0.05);
}

TEST(test_rgb2luv)
{
    // This test is only valid for NTSC RGB Model and D65 illuminant
    double gamma = 2.2;

    double ch1 = pow(0.5, gamma);
    double ch2 = pow(0.5, gamma);
    double ch3 = pow(0.0, gamma);

    vision::Convert::rgb2xyz(&ch1, &ch2, &ch3);
    vision::Convert::xyz2luv(&ch1, &ch2, &ch3);
    
    CHECK_CLOSE(51.004099, ch1, 0.05);
    CHECK_CLOSE(13.952179, ch2, 0.05);
    CHECK_CLOSE(59.999115, ch3, 0.05);
}

TEST(test_rgb2lch)
{
    // This test is only valid for NTSC RGB Model and D65 illuminant
    double gamma = 2.2;

    {
        double ch1 = pow(0.5, gamma);
        double ch2 = pow(0.5, gamma);
        double ch3 = pow(0.0, gamma);

        vision::Convert::rgb2xyz(&ch1, &ch2, &ch3);
        vision::Convert::xyz2luv(&ch1, &ch2, &ch3);
        vision::Convert::luv2lch_uv(&ch1, &ch2, &ch3);
    
        CHECK_CLOSE(51.004099, ch1, 0.05);
        CHECK_CLOSE(61.599977, ch2, 0.05);
        CHECK_CLOSE(54.477284125, ch3, 0.05);
    }

    {
        double ch1 = pow(0.0, gamma);
        double ch2 = pow(0.5, gamma);
        double ch3 = pow(0.5, gamma);

        vision::Convert::rgb2xyz(&ch1, &ch2, &ch3);
        vision::Convert::xyz2luv(&ch1, &ch2, &ch3);
        vision::Convert::luv2lch_uv(&ch1, &ch2, &ch3);
    
        CHECK_CLOSE(45.985854, ch1, 0.05);
        CHECK_CLOSE(59.372290, ch2, 0.05);
        CHECK_CLOSE(140.505417917, ch3, 0.05);
    }

    {
        double ch1 = pow(0.5, gamma);
        double ch2 = pow(0.0, gamma);
        double ch3 = pow(0.5, gamma);

        vision::Convert::rgb2xyz(&ch1, &ch2, &ch3);
        vision::Convert::xyz2luv(&ch1, &ch2, &ch3);
        vision::Convert::luv2lch_uv(&ch1, &ch2, &ch3);
    
        CHECK_CLOSE(35.978829, ch1, 0.05);
        CHECK_CLOSE(73.778831, ch2, 0.05);
        CHECK_CLOSE(224.00384475, ch3, 0.05);
    }
}

}
