/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestOpenCVImage.cxx
 */

// STD Includes
#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "vision/include/Exception.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"

struct RGBImageFixture
{
    RGBImageFixture() :
        img(1, 1, ram::vision::Image::PF_RGB_8)
    {
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 0; data[2] = 0;
    }

    ram::vision::OpenCVImage img;
};

struct BGRImageFixture
{
    BGRImageFixture() :
        img(1, 1, ram::vision::Image::PF_BGR_8)
    {
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 0; data[2] = 0;
    }

    ram::vision::OpenCVImage img;
};

struct GrayImageFixture
{
    GrayImageFixture() :
        img(1, 1, ram::vision::Image::PF_GRAY_8)
    {
        unsigned char* data = img.getData();
        data[0] = 128;
    }

    ram::vision::OpenCVImage img;
};

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

TEST(Default_Image)
{
    ram::vision::OpenCVImage def(640, 480);
    IplImage* image = def.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(3, image->nChannels);
}

TEST(RGB_Image)
{
    ram::vision::OpenCVImage rgb(640, 480, ram::vision::Image::PF_RGB_8);
    IplImage* image = rgb.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(3, image->nChannels);
    CHECK_EQUAL(ram::vision::Image::PF_RGB_8, rgb.getPixelFormat());
}

TEST(BGR_Image)
{
    ram::vision::OpenCVImage bgr(640, 480, ram::vision::Image::PF_BGR_8);
    IplImage* image = bgr.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(3, image->nChannels);
    CHECK_EQUAL(ram::vision::Image::PF_BGR_8, bgr.getPixelFormat());
}

TEST(YUV444_Image)
{
    ram::vision::OpenCVImage yuv(640, 480, ram::vision::Image::PF_YUV444_8);
    IplImage* image = yuv.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(3, image->nChannels);
    CHECK_EQUAL(ram::vision::Image::PF_YUV444_8, yuv.getPixelFormat());
}

TEST(Gray_Image)
{
    ram::vision::OpenCVImage gray(640, 480, ram::vision::Image::PF_GRAY_8);
    IplImage* image = gray.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(1, image->nChannels);
    CHECK_EQUAL(ram::vision::Image::PF_GRAY_8, gray.getPixelFormat());
}

TEST(ConversionFailure)
{
    ram::vision::OpenCVImage image(640, 480);
    try {
        image.setPixelFormat(ram::vision::Image::PF_RGB_8);
        CHECK(false && "Invalid conversion did not throw an exception");
    } catch (ram::vision::ImageConversionException& ex) {
        std::cout << ex.what() << std::endl;
    }
}

TEST_FIXTURE(RGBImageFixture, RGB_to_BGR)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(255, beforeData[0]); // Red
    CHECK_EQUAL(  0, beforeData[1]); // Green
    CHECK_EQUAL(  0, beforeData[2]); // Blue
    CHECK_EQUAL(ram::vision::Image::PF_RGB_8, img.getPixelFormat());

    // Convert to BGR
    img.setPixelFormat(ram::vision::Image::PF_BGR_8);

    unsigned char* afterData = img.getData();
    CHECK_EQUAL(  0, afterData[0]); // Blue
    CHECK_EQUAL(  0, afterData[1]); // Green
    CHECK_EQUAL(255, afterData[2]); // Red
    CHECK_EQUAL(ram::vision::Image::PF_BGR_8, img.getPixelFormat());
}

TEST_FIXTURE(RGBImageFixture, RGB_to_GRAY)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(255, beforeData[0]); // Red
    CHECK_EQUAL(  0, beforeData[1]); // Green
    CHECK_EQUAL(  0, beforeData[2]); // Blue
    CHECK_EQUAL(ram::vision::Image::PF_RGB_8, img.getPixelFormat());

    // Convert to Grayscale
    img.setPixelFormat(ram::vision::Image::PF_GRAY_8);

    unsigned char* afterData = img.getData();
    // 255 * .299 (as specified in the OpenCV documentation)
    CHECK_EQUAL((int) (255 * .299), afterData[0]);
    CHECK_EQUAL(ram::vision::Image::PF_GRAY_8, img.getPixelFormat());
}

TEST_FIXTURE(BGRImageFixture, BGR_to_RGB)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(255, beforeData[0]); // Blue
    CHECK_EQUAL(  0, beforeData[1]); // Green
    CHECK_EQUAL(  0, beforeData[2]); // Red
    CHECK_EQUAL(ram::vision::Image::PF_BGR_8, img.getPixelFormat());

    // Convert to RGB
    img.setPixelFormat(ram::vision::Image::PF_RGB_8);

    unsigned char* afterData = img.getData();
    CHECK_EQUAL(  0, afterData[0]); // Red
    CHECK_EQUAL(  0, afterData[1]); // Green
    CHECK_EQUAL(255, afterData[2]); // Blue
    CHECK_EQUAL(ram::vision::Image::PF_RGB_8, img.getPixelFormat());
}

TEST_FIXTURE(BGRImageFixture, BGR_to_Gray)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(255, beforeData[0]); // Blue
    CHECK_EQUAL(  0, beforeData[1]); // Green
    CHECK_EQUAL(  0, beforeData[2]); // Red
    CHECK_EQUAL(ram::vision::Image::PF_BGR_8, img.getPixelFormat());

    // Convert to Grayscale
    img.setPixelFormat(ram::vision::Image::PF_GRAY_8);

    unsigned char* afterData = img.getData();
    // 255 * .114 (as specified in the OpenCV documentation)
    CHECK_EQUAL((int) (255 * .114), afterData[0]);
    CHECK_EQUAL(ram::vision::Image::PF_GRAY_8, img.getPixelFormat());
}

TEST_FIXTURE(GrayImageFixture, Gray_to_RGB)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(128, beforeData[0]);
    CHECK_EQUAL(ram::vision::Image::PF_GRAY_8, img.getPixelFormat());

    // Convert to RGB
    img.setPixelFormat(ram::vision::Image::PF_RGB_8);

    unsigned char* afterData = img.getData();
    CHECK_EQUAL(128, afterData[0]); // Red
    CHECK_EQUAL(128, afterData[1]); // Green
    CHECK_EQUAL(128, afterData[2]); // Blue
    CHECK_EQUAL(ram::vision::Image::PF_RGB_8, img.getPixelFormat());
}

TEST_FIXTURE(GrayImageFixture, Gray_to_BGR)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(128, beforeData[0]);
    CHECK_EQUAL(ram::vision::Image::PF_GRAY_8, img.getPixelFormat());

    // Convert to RGB
    img.setPixelFormat(ram::vision::Image::PF_BGR_8);

    unsigned char* afterData = img.getData();
    CHECK_EQUAL(128, afterData[0]); // Blue
    CHECK_EQUAL(128, afterData[1]); // Green
    CHECK_EQUAL(128, afterData[2]); // Red
    CHECK_EQUAL(ram::vision::Image::PF_BGR_8, img.getPixelFormat());
}

} // SUITE(OpenCVImage)
