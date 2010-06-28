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
#include "cv.h"

// Project Includes
#include "vision/include/Exception.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"

using namespace ram;

struct RGBImageFixture
{
    RGBImageFixture() :
        img(1, 1, vision::Image::PF_RGB_8)
    {
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 0; data[2] = 0;
    }

    vision::OpenCVImage img;
};

struct BGRImageFixture
{
    BGRImageFixture() :
        img(1, 1, vision::Image::PF_BGR_8)
    {
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 0; data[2] = 0;
    }

    vision::OpenCVImage img;
};

struct GrayImageFixture
{
    GrayImageFixture() :
        img(1, 1, vision::Image::PF_GRAY_8)
    {
        unsigned char* data = img.getData();
        data[0] = 128;
    }

    vision::OpenCVImage img;
};

SUITE(OpenCVImage) {

/**
TEST(multiChannelFiltering)
{
    // Test cvCanny for multi channel filtering
    // Create a three channel rgb image
    vision::OpenCVImage source(640, 480);
    vision::OpenCVImage destination(640, 480, vision::Image::PF_GRAY_8);
    vision::OpenCVImage output(640, 480, vision::Image::PF_GRAY_8);
    
    vision::makeColor(&source, 0, 0, 0);
    vision::makeColor(&output, 0, 0, 0);

    vision::drawLine(&source, 150, 240, 515, 240, 3, CV_RGB(0, 255, 0));

    // Throw it into cvCanny, no crash = pass
    // This should filter red and blue channels which are black
    // and should return a black image
    cvCanny(source->asIplImage(), destination->asIplImage(),
            80, 200, 3, 1, 0, 1);

    CHECK_CLOSE(destination, output);
}
**/

TEST(copyFrom)
{
    vision::OpenCVImage source(640, 480);
    vision::OpenCVImage destination(10, 10);

    vision::makeColor(&source, 255, 0, 0);

    // Copy image over
    destination.copyFrom(&source);
    
    // Should be exactly the same
    CHECK_CLOSE(&source, &destination,
                0);
}

TEST(test_copyFromDifferentPixelFormat)
{
    vision::OpenCVImage normal(100, 100, vision::Image::PF_RGB_8);
    CHECK_EQUAL(vision::Image::PF_RGB_8, normal.getPixelFormat());

    vision::OpenCVImage newimg(50, 75, vision::Image::PF_GRAY_8);
    CHECK_EQUAL(50u, newimg.getWidth());
    CHECK_EQUAL(75u, newimg.getHeight());
    CHECK_EQUAL(vision::Image::PF_GRAY_8, newimg.getPixelFormat());

    // Perform the copy, this should change the properties of newimg
    newimg.copyFrom(&normal);
    CHECK_EQUAL(100u, newimg.getWidth());
    CHECK_EQUAL(100u, newimg.getHeight());
    CHECK_EQUAL(vision::Image::PF_RGB_8, newimg.getPixelFormat());
}

TEST(test_copyFrom_channelOnly)
{
    vision::OpenCVImage normal(100, 100, vision::Image::PF_RGB_8);
    CHECK_EQUAL(vision::Image::PF_RGB_8, normal.getPixelFormat());

    vision::OpenCVImage newimg(100, 100, vision::Image::PF_GRAY_8);
    CHECK_EQUAL(vision::Image::PF_GRAY_8, newimg.getPixelFormat());

    // If this isn't done correctly, OpenCV will crash
    newimg.copyFrom(&normal);
    CHECK_EQUAL(vision::Image::PF_RGB_8, newimg.getPixelFormat());
}

TEST(Default_Image)
{
    vision::OpenCVImage def(640, 480);
    IplImage* image = def.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(3, image->nChannels);
}

TEST(RGB_Image)
{
    vision::OpenCVImage rgb(640, 480, vision::Image::PF_RGB_8);
    IplImage* image = rgb.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(3, image->nChannels);
    CHECK_EQUAL(vision::Image::PF_RGB_8, rgb.getPixelFormat());
}

TEST(BGR_Image)
{
    vision::OpenCVImage bgr(640, 480, vision::Image::PF_BGR_8);
    IplImage* image = bgr.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(3, image->nChannels);
    CHECK_EQUAL(vision::Image::PF_BGR_8, bgr.getPixelFormat());
}

TEST(YUV444_Image)
{
    vision::OpenCVImage yuv(640, 480, vision::Image::PF_YUV444_8);
    IplImage* image = yuv.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(3, image->nChannels);
    CHECK_EQUAL(vision::Image::PF_YUV444_8, yuv.getPixelFormat());
}

TEST(Gray_Image)
{
    vision::OpenCVImage gray(640, 480, vision::Image::PF_GRAY_8);
    IplImage* image = gray.asIplImage();

    CHECK_EQUAL(8, image->depth);
    CHECK_EQUAL(1, image->nChannels);
    CHECK_EQUAL(vision::Image::PF_GRAY_8, gray.getPixelFormat());
}

TEST(ConversionFailure)
{
    vision::OpenCVImage image(640, 480);
    try {
        image.setPixelFormat(vision::Image::PF_RGB_8);
        CHECK(false && "Invalid conversion did not throw an exception");
    } catch (vision::ImageConversionException& ex) {
        std::cout << ex.what() << std::endl;
    }
}

TEST_FIXTURE(RGBImageFixture, RGB_to_BGR)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(255, beforeData[0]); // Red
    CHECK_EQUAL(  0, beforeData[1]); // Green
    CHECK_EQUAL(  0, beforeData[2]); // Blue
    CHECK_EQUAL(vision::Image::PF_RGB_8, img.getPixelFormat());

    // Convert to BGR
    img.setPixelFormat(vision::Image::PF_BGR_8);

    unsigned char* afterData = img.getData();
    CHECK_EQUAL(  0, afterData[0]); // Blue
    CHECK_EQUAL(  0, afterData[1]); // Green
    CHECK_EQUAL(255, afterData[2]); // Red
    CHECK_EQUAL(vision::Image::PF_BGR_8, img.getPixelFormat());
}

TEST_FIXTURE(RGBImageFixture, RGB_to_GRAY)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(255, beforeData[0]); // Red
    CHECK_EQUAL(  0, beforeData[1]); // Green
    CHECK_EQUAL(  0, beforeData[2]); // Blue
    CHECK_EQUAL(vision::Image::PF_RGB_8, img.getPixelFormat());

    // Convert to Grayscale
    img.setPixelFormat(vision::Image::PF_GRAY_8);

    unsigned char* afterData = img.getData();
    // 255 * .299 (as specified in the OpenCV documentation)
    CHECK_EQUAL((int) (255 * .299), afterData[0]);
    CHECK_EQUAL(vision::Image::PF_GRAY_8, img.getPixelFormat());
}

TEST_FIXTURE(BGRImageFixture, BGR_to_RGB)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(255, beforeData[0]); // Blue
    CHECK_EQUAL(  0, beforeData[1]); // Green
    CHECK_EQUAL(  0, beforeData[2]); // Red
    CHECK_EQUAL(vision::Image::PF_BGR_8, img.getPixelFormat());

    // Convert to RGB
    img.setPixelFormat(vision::Image::PF_RGB_8);

    unsigned char* afterData = img.getData();
    CHECK_EQUAL(  0, afterData[0]); // Red
    CHECK_EQUAL(  0, afterData[1]); // Green
    CHECK_EQUAL(255, afterData[2]); // Blue
    CHECK_EQUAL(vision::Image::PF_RGB_8, img.getPixelFormat());
}

TEST_FIXTURE(BGRImageFixture, BGR_to_Gray)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(255, beforeData[0]); // Blue
    CHECK_EQUAL(  0, beforeData[1]); // Green
    CHECK_EQUAL(  0, beforeData[2]); // Red
    CHECK_EQUAL(vision::Image::PF_BGR_8, img.getPixelFormat());

    // Convert to Grayscale
    img.setPixelFormat(vision::Image::PF_GRAY_8);

    unsigned char* afterData = img.getData();
    // 255 * .114 (as specified in the OpenCV documentation)
    CHECK_EQUAL((int) (255 * .114), afterData[0]);
    CHECK_EQUAL(vision::Image::PF_GRAY_8, img.getPixelFormat());
}

TEST_FIXTURE(GrayImageFixture, Gray_to_RGB)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(128, beforeData[0]);
    CHECK_EQUAL(vision::Image::PF_GRAY_8, img.getPixelFormat());

    // Convert to RGB
    img.setPixelFormat(vision::Image::PF_RGB_8);

    unsigned char* afterData = img.getData();
    CHECK_EQUAL(128, afterData[0]); // Red
    CHECK_EQUAL(128, afterData[1]); // Green
    CHECK_EQUAL(128, afterData[2]); // Blue
    CHECK_EQUAL(vision::Image::PF_RGB_8, img.getPixelFormat());
}

TEST_FIXTURE(GrayImageFixture, Gray_to_BGR)
{
    unsigned char* beforeData = img.getData();
    CHECK_EQUAL(128, beforeData[0]);
    CHECK_EQUAL(vision::Image::PF_GRAY_8, img.getPixelFormat());

    // Convert to RGB
    img.setPixelFormat(vision::Image::PF_BGR_8);

    unsigned char* afterData = img.getData();
    CHECK_EQUAL(128, afterData[0]); // Blue
    CHECK_EQUAL(128, afterData[1]); // Green
    CHECK_EQUAL(128, afterData[2]); // Red
    CHECK_EQUAL(vision::Image::PF_BGR_8, img.getPixelFormat());
}

TEST(test_rgb2xyz)
{
    // This test is only valid for NTSC RGB Model and D65 illuminant
    double gamma = 2.2;

    double ch1 = pow(0.5, gamma);
    double ch2 = pow(0.5, gamma);
    double ch3 = pow(0.0, gamma);

    vision::OpenCVImage::rgb2xyz(&ch1, &ch2, &ch3);
    
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

    vision::OpenCVImage::rgb2xyz(&ch1, &ch2, &ch3);
    vision::OpenCVImage::xyz2luv(&ch1, &ch2, &ch3);
    
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

        vision::OpenCVImage::rgb2xyz(&ch1, &ch2, &ch3);
        vision::OpenCVImage::xyz2luv(&ch1, &ch2, &ch3);
        vision::OpenCVImage::luv2lch_uv(&ch1, &ch2, &ch3);
    
        CHECK_CLOSE(51.004099, ch1, 0.05);
        CHECK_CLOSE(61.599977, ch2, 0.05);
        CHECK_CLOSE(54.477284125, ch3, 0.05);
    }

    {
        double ch1 = pow(0.0, gamma);
        double ch2 = pow(0.5, gamma);
        double ch3 = pow(0.5, gamma);

        vision::OpenCVImage::rgb2xyz(&ch1, &ch2, &ch3);
        vision::OpenCVImage::xyz2luv(&ch1, &ch2, &ch3);
        vision::OpenCVImage::luv2lch_uv(&ch1, &ch2, &ch3);
    
        CHECK_CLOSE(45.985854, ch1, 0.05);
        CHECK_CLOSE(59.372290, ch2, 0.05);
        CHECK_CLOSE(140.505417917, ch3, 0.05);
    }

    {
        double ch1 = pow(0.5, gamma);
        double ch2 = pow(0.0, gamma);
        double ch3 = pow(0.5, gamma);

        vision::OpenCVImage::rgb2xyz(&ch1, &ch2, &ch3);
        vision::OpenCVImage::xyz2luv(&ch1, &ch2, &ch3);
        vision::OpenCVImage::luv2lch_uv(&ch1, &ch2, &ch3);
    
        CHECK_CLOSE(35.978829, ch1, 0.05);
        CHECK_CLOSE(73.778831, ch2, 0.05);
        CHECK_CLOSE(224.00384475, ch3, 0.05);
    }
    
}


} // SUITE(OpenCVImage)
