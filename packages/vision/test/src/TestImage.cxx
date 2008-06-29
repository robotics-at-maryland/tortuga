/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestImage.cxx
 */


// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/filesystem.hpp>

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"

#ifndef RAM_WINDOWS

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "images";
}

using namespace ram;

SUITE(Image) {

TEST(rotateAndScale)
{
    vision::Image* reference = vision::Image::loadFromFile(
        (getImagesDir() / "testrotate" / "upright.png").string());
    vision::Image* expected30 = vision::Image::loadFromFile(
        (getImagesDir() / "testrotate" / "30right.png").string());
    vision::Image* expected90 = vision::Image::loadFromFile(
        (getImagesDir() / "testrotate" / "90right.png").string());
    vision::Image* expectedNeg25Scale = vision::Image::loadFromFile(
        (getImagesDir() / "testrotate" / "25left_20scale.png").string());
    vision::Image* result = new vision::OpenCVImage(640, 480);

    // Rotate Image right 30 degrees
    vision::Image::rotateAndScale(reference, result, math::Degree(30));
    CHECK_CLOSE(*expected30, *result, 1.5);

    // Rotate Image right 90 degrees
    vision::Image::rotateAndScale(reference, result, math::Degree(90));
    CHECK_CLOSE(*expected90, *result, 1);

    // Rotate left 25 degrees and scale 20%
    vision::Image::rotateAndScale(reference, result, math::Degree(-25), 1.2);
    CHECK_CLOSE(*expectedNeg25Scale, *result, 3.0);
    
    delete reference;
    delete expected30;
    delete expected90;
    delete result;
}

TEST(blitImage)
{
    vision::Image* expected = vision::Image::loadFromFile(
        (getImagesDir() / "testblit" / "complete.png").string());
    vision::Image* source = vision::Image::loadFromFile(
        (getImagesDir() / "testblit" / "start.png").string());
    vision::Image* toBlit = vision::Image::loadFromFile(
        (getImagesDir() / "testblit" / "toblit.png").string());
    vision::Image* blankBlit = new vision::OpenCVImage(640, 480);
    vision::Image* result = new vision::OpenCVImage(640, 480);

    // Completely blank image should result in no change
    makeColor(blankBlit, 255, 255, 255);
    vision::Image::blitImage(blankBlit, source, result);
    CHECK_CLOSE(*source, *result, 0);

    // Now a normal blit
    vision::Image::blitImage(toBlit, source, result);
    CHECK_CLOSE(*result, *expected, 0);

    // Do a blit with a transform in it
    int transX = 200;
    int transY = 50;
    
    makeColor(source, 255, 255, 255);

    makeColor(toBlit, 255, 255, 255);
    drawSquare(toBlit, 200, 200, 50, 100, 0.0, CV_RGB(0, 0, 0));
    
    makeColor(expected, 255, 255, 255);
    drawSquare(expected, transX + 200, transY + 200, 50, 100, 0,
               CV_RGB(0, 0, 0));

    vision::Image::blitImage(toBlit, source, result, 255, 255, 255,
                             transX, transY);
    
    CHECK_CLOSE(*result, *expected, 0);
}

}

#endif // RAM_WINDOWS
