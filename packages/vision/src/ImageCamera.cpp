/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/ImageCamera.cpp
 */

// STD Includes
#include <iostream>

// Project Includes
#include "vision/include/ImageCamera.h"
#include "vision/include/OpenCVImage.h"
#include "cv.h"
namespace ram {
namespace vision {

ImageCamera::ImageCamera(size_t width, size_t height, double fps) :
    _backgrounded(false),
    m_fps(fps),
    m_width(width),
    m_height(height)
{
}

ImageCamera::~ImageCamera()
{
    cleanup();
}

void ImageCamera::newImage(ram::vision::Image* image)
{
	OpenCVImage sizedImage(m_width, m_height);
    sizedImage.copyFrom(image);
    sizedImage.setSize(m_width, m_height);
    capturedImage(&sizedImage);
}

void ImageCamera::update(double)
{
    //assert(false && "Cannot update an ImageCamera");
}
    
size_t ImageCamera::width()
{
    return m_width;
}

size_t ImageCamera::height()
{
    return m_height;
}

double ImageCamera::fps()
{
    return m_fps;
}
    
double ImageCamera::duration()
{
    return 0;
}

void ImageCamera::seekToTime(double seconds)
{
}

double ImageCamera::currentTime()
{
    return 0;
}

bool ImageCamera::backgrounded()
{
    return _backgrounded;
}
    
void ImageCamera::background(int)
{
    _backgrounded = true;
}

void ImageCamera::unbackground(bool join)
{
    _backgrounded = false;
    Camera::unbackground(join);
}

}  // namespace vision
}  // namespace ram
