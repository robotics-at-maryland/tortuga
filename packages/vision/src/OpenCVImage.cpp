/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/include/CvCamera.h
 */

// Library Includes
/// TODO: Limit this to just the needed headers if possible
#include "cv.h" // OpenCV Functions

// Project incldues
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

OpenCVImage::OpenCVImage(IplImage* image) :
    m_img(image)
{
}

OpenCVImage::OpenCVImage()
{
    assert(false && "Should not be called");
}

OpenCVImage::~OpenCVImage()
{
    cvReleaseImage(&m_img);
}

size_t OpenCVImage::getWidth()
{
    return cvGetSize(m_img).width;
}

size_t OpenCVImage::getHeight()
{
    return cvGetSize(m_img).height;
}

Image::PixelFormat OpenCVImage::getPixelFormat()
{
    ///TODO:Make sure this format is actually the format that the image is in.
    return PF_BGR_8;
}

unsigned char* OpenCVImage::setData(unsigned char* data)
{
    cvSetImageData(m_img, data, cvGetSize(m_img).width * 3);
}

void setWidth(int pixels)
{
    assert(false && "Not implemented");
}

void setHeight(int pixels)
{
    assert(false && "Not implemented");
}

void setPixelFormat(Image::PixelFormat format)
{
    assert(false && "Not implemented");
}

} // namespace vision
} // namespace ram
