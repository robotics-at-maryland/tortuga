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

// Vision Includes
#include "vision/include/BasicImage.h"

namespace ram {
namespace vision {

BasicImage::BasicImage() :
    m_data(0),
    m_height(0),
    m_width(0),
    m_format(Image::PF_RGB_8)
{
}

BasicImage::BasicImage(unsigned char* data, int width, int height,
           Image::PixelFormat format) :
    m_data(data),
    m_height(width),
    m_width(height),
    m_format(Image::PF_RGB_8)
{
}

BasicImage::~BasicImage()
{
    delete m_data;
}

unsigned char* BasicImage::setData(unsigned char* data)
{
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
    
} // namespace vision
} // namespace ram
