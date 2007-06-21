/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Camera.h
 */

// Project Includes
#include "vision/include/Camera.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

Camera::Camera() :
    m_publicImage(0)
{
    /// TODO: Make me a basic image, and check that copying work properly
    m_publicImage = new OpenCVImage(640, 480);
}

Camera::~Camera()
{
    delete m_publicImage;
}
    
void Camera::getImage(Image* current)
{
    assert(current && "Can't copy into a null image");
    
    core::ReadWriteMutex::ScopedReadLock lock(m_imageMutex);

    // printf("Copying public image to given image\n");
    // Copy over the image (uses copy assignment operator)
    current->copyFrom(m_publicImage);
}
    
void Camera::capturedImage(Image* newImage)
{
    assert(newImage && "Can't copy null image");
    
    {    
        core::ReadWriteMutex::ScopedWriteLock lock(m_imageMutex);

        // Copy over the image data to the public image
        // printf("Copying to public image\n");
        m_publicImage->copyFrom(newImage);
    }
    
    // Our state has changed
    setChanged();

    // Tell all the observers about it
    notifyObservers(0, IMAGE_CAPTURED);
}
    
} // namespace vision
} // namespace ram
