/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/MockCamera.cxx
 */

// Project Includes
#include "vision/include/Image.h"
#include "vision/test/include/MockCamera.h"

MockCamera::MockCamera(ram::vision::Image* newImage,
                       ram::vision::Image* publicImage) :
    _fps(30),
    _backgrounded(false),
    m_newImage(newImage),
    m_publicImage(publicImage)
{
    
}

MockCamera::~MockCamera()
{
    cleanup();
}

void MockCamera::setNewImage(ram::vision::Image* newImage)
{
    m_newImage = newImage;
}

void MockCamera::setPublicImage(ram::vision::Image* publicImage)
{
    m_publicImage = publicImage;
}

void MockCamera::update(double timestep)
{
    capturedImage(m_newImage);
}
    
size_t MockCamera::width()
{
    if (m_publicImage)
        return m_publicImage->getWidth();
    if (m_newImage)
        return m_newImage->getWidth();
    return 640;
}

size_t MockCamera::height()
{
    if (m_publicImage)
        return m_publicImage->getHeight();
    if (m_newImage)
        return m_newImage->getHeight();
    return 480;
}

void MockCamera::copyToPublic(ram::vision::Image* newImage,
                              ram::vision::Image* publicImage)
{
    if (m_publicImage)
        publicImage->copyFrom(m_publicImage);
    else
        publicImage->copyFrom(newImage);
}
