/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/FlexImage.cpp
 */

// STD includes
#include <cassert>
#include <iostream>

// OpenCV Includes
#include <cxcore.h>

// Vision Includes
#include "vision/include/FlexImage.h"

namespace ram {
namespace vision {

FlexImage::FlexImage() :
    m_data(0),
    m_dataSize(0),
    m_width(0),
    m_height(0),
    m_format(Image::PF_RGB_8),
    m_iplimage(0)
{
}

FlexImage::FlexImage(int width, int height, Image::PixelFormat format) :
    m_data(0),
    m_dataSize(0),
    m_width(0),
    m_height(0),
    m_format(format),
    m_iplimage(0)
{
    
}
    
FlexImage::~FlexImage()
{
    delete[] m_data;

    if (m_iplimage)
        cvReleaseImageHeader(&m_iplimage);
}

void FlexImage::copyFrom(const Image* src)
{
    // Make our size reflect there size
    setSize(src->getWidth(), src->getHeight());
    // Copy over there data to our buffer
    memcpy(m_data, src->getData(), m_dataSize);
}

unsigned char* FlexImage::getData() const
{
    assert(m_data && "No FlexImage data, call setSize first");
    return m_data;
}

void FlexImage::setSize(int width, int height)
{
    assert(width >= 0 && "Image can't have a negative width");
    assert(height >= 0 && "Image can't have a negative width");

    // Store values
    m_width = width;
    m_height = height;

    // Calc buffer size
    size_t newDataSize = width * height * 3;
    if (m_dataSize < newDataSize)
    {
        delete[] m_data;
        m_data = new unsigned char[newDataSize];
        m_dataSize = newDataSize;
    }

    // Create an IplImage with the proper size and give it our data
    CvSize size;
    size.width = m_width;
    size.height = m_height;
    
    if (m_iplimage)
        cvReleaseImageHeader(&m_iplimage);
    m_iplimage = cvCreateImageHeader(size, IPL_DEPTH_8U, 3);
    cvSetData(m_iplimage, m_data, m_width * 3);
}
    
} // namespace vision
} // namespace ram
