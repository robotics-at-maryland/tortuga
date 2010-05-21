/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/test/src/TestSegmentationFilter.cxx
 */

// Library Includes
#include "highgui.h"
#include <boost/filesystem.hpp>
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vision/include/SegmentationFilter.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/Utility.h"
#include "vision/test/include/UnitTestChecks.h"

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "images";
}

using namespace ram;


struct SegmentationFilterFixture
{
    SegmentationFilterFixture() :
        filter(0.5, 500, 50)
    {
    }

    vision::SegmentationFilter filter;
};

SUITE(SegmentationFilter) 
{

TEST_FIXTURE(SegmentationFilterFixture, FilterImage)
{
    // Load input image and expected output
    boost::filesystem::path inputpath =
        getImagesDir() / "testsegment" / "input.png";
    boost::filesystem::path outputpath =
        getImagesDir() / "testsegment" / "output.png";

    // Load a proper pixel format into each image
    vision::Image* input = vision::Image::loadFromFile(inputpath.string());
    vision::Image* output = vision::Image::loadFromFile(outputpath.string());

    // Filter the image
    filter.filterImage(input);

    // Check that both images are the same
    CHECK_CLOSE(*output, *input, 0);

    // Clean up
    delete input;
    delete output;
}

}
