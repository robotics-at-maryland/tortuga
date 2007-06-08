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

OpenCVImage::OpenCVImage(IplImage* image, bool ownership) :
    m_own(ownership),
    m_img(image)
{
}

OpenCVImage::OpenCVImage()
{
    assert(false && "Should not be called");
}

OpenCVImage::~OpenCVImage()
{
    if (m_own)
        cvReleaseImage(&m_img);
}

unsigned char* OpenCVImage::getData()
{
    return (unsigned char*)(m_img->imageData);
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

unsigned char* OpenCVImage::setData(unsigned char* data, bool ownership)
{
    m_own = ownership;
    cvSetImageData(m_img, data, cvGetSize(m_img).width * 3);
}

void  OpenCVImage::setWidth(int pixels)
{
    assert(false && "Not implemented");
}

void  OpenCVImage::setHeight(int pixels)
{
    assert(false && "Not implemented");
}

void  OpenCVImage::setPixelFormat(Image::PixelFormat format)
{
    assert(false && "Not implemented");
}

OpenCVImage::operator IplImage* ()
{
    return m_img;
}

} // namespace vision
} // namespace ram
