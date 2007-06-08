/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/BasicImage.cpp
 */

// STD includes
#include <cassert>

// OpenCV Includes
#include <cxcore.h>

// Vision Includes
#include "vision/include/BasicImage.h"

namespace ram {
namespace vision {

BasicImage::BasicImage() :
    m_data(0),
    m_own(true),
    m_height(0),
    m_width(0),
    m_format(Image::PF_RGB_8),
    m_iplimage(0)
{
}

BasicImage::BasicImage(unsigned char* data, int width, int height,
                       bool ownership, Image::PixelFormat format) :
    m_data(data),
    m_own(ownership),
    m_height(width),
    m_width(height),
    m_format(Image::PF_RGB_8)
{
}
    
BasicImage::~BasicImage()
{
    if (m_own)
        delete m_data;

    if (m_iplimage)
        cvReleaseImageHeader(&m_iplimage);
}
    
unsigned char* BasicImage::setData(unsigned char* data, bool ownership)     
{
    m_own = ownership;
    unsigned char* tmp = m_data;
    m_data = data;
    return tmp;
}

void BasicImage::setWidth(int pixels)
{
    assert(pixels >= 0 && "Image can't have a negative width");
    m_width = pixels;
}

void BasicImage::setHeight(int pixels)
{
    assert(pixels >=0 && "Image can't have a negative height");
    m_height = pixels;
}

void BasicImage::setPixelFormat(Image::PixelFormat format)
{
    assert((PF_START > format) && (format < PF_END) && "Invalid pixel format");
    m_format = format;
}

BasicImage::operator IplImage* ()
{
    // Create an IplImage with the proper size and give it our data
    CvSize size;
    size.width = m_width;
    size.height = m_height;
    
    if (m_iplimage)
        cvReleaseImageHeader(&m_iplimage);
    m_iplimage = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);
    
    cvSetData(m_iplimage, m_data, m_width * 3);
    return m_iplimage;
}
    
} // namespace vision
} // namespace ram
