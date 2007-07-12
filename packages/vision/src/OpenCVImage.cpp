/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/include/CvCamera.h
 */

#include <cstdio>
#include <iostream>

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
    assert(width >= 1 && "Image can't have a negative or 0 width");
    assert(height >= 1 && "Image can't have a negative or 0 height");
    
    m_img = cvCreateImage(cvSize(width, height), 8, 3);

    assert(m_img && "Error creating OpenCV Image");
}
    
OpenCVImage::OpenCVImage(IplImage* image, bool ownership) :
    m_own(ownership),
    m_img(image)
{
}
    
void OpenCVImage::copyFrom (const Image* src)
{
    // Handle self copy
    if (this == src)
        return;
    
    // Create temporaty OpenCV image to smooth the copy process
    IplImage* tmp_img = cvCreateImageHeader(cvSize(src->getWidth(),
                                                   src->getHeight()), 8, 3);
    cvSetData(tmp_img, src->getData(), src->getWidth() * 3);

    // Resize image if needed (also copy)
    if ((getWidth() != src->getWidth()) ||
        (getHeight() != src->getHeight()) )
    {
        cvResize(tmp_img, m_img);
    }
    else
    {
        // Copy the internal image data over
        cvCopy(tmp_img, m_img);
    }

    cvReleaseImageHeader(&tmp_img);
    
    // Copy Other members
    m_own = src->getOwnership();
}
    
OpenCVImage::~OpenCVImage()
{
    if (m_own)
        cvReleaseImage(&m_img);
}

unsigned char* OpenCVImage::getData() const
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

bool OpenCVImage::getOwnership() const
{
    return m_own;
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
