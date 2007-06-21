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

OpenCVImage::OpenCVImage(int width, int height) :
    m_own(true),
    m_img(0)
    
{
    assert(width >= 0 && "Image can't have a negative width");
    assert(height >= 0 && "Image can't have a negative height");
    
    m_img = cvCreateImage(cvSize(width, height), 8, 3);
}
    
OpenCVImage::OpenCVImage(IplImage* image, bool ownership) :
    m_own(ownership),
    m_img(image)
{
}

OpenCVImage::OpenCVImage()
{
    assert(false && "Should not be called");
}

OpenCVImage& OpenCVImage::operator= (const OpenCVImage& src)
{
    // Handle self assignment
    if (this == &src)
        return *this;

    // Resize image if needed (also copy)
    if ((getWidth() != src.getWidth()) ||
        (getHeight() != src.getHeight()) )
    {
        cvResize(src.m_img, m_img);
    }
    else
    {
        // Copy the internal image data over
        cvCopy(src.m_img, m_img);
    }

    // Copy Other members
    m_own = src.m_own;
    
    return *this;
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
    
size_t OpenCVImage::getWidth() const
{
    return cvGetSize(m_img).width;
}

size_t OpenCVImage::getHeight() const
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
    unsigned char* tmp = getData();
    cvSetImageData(m_img, data, cvGetSize(m_img).width * 3);
    return tmp;
}

void  OpenCVImage::setSize(int width, int height)
{
    
    assert(false && "OpenCVImage::setSize Not implemented");
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
