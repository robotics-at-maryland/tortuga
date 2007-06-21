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

namespace ram {
namespace vision {

void Camera::getImage(Image* current)
{
    assert(current && "Can't copy into a null image");
    
    core::ReadWriteMutex::ScopedReadLock lock(m_imageMutex);

    // Copy over the image (uses copy assignment operator)
    *current = *m_publicImage;
}
    
void Camera::capturedImage(Image* newImage)
{
    assert(newImage && "Can't copy null image");
    
    {    
        core::ReadWriteMutex::ScopedWriteLock lock(m_imageMutex);

        // Copy over the image data to the public image
        *m_publicImage = *newImage;
    }
    
    // Our state has changed
    setChanged();

    // Tell all the observers about it
    notifyObservers(0, IMAGE_CAPTURED);
}
    
} // namespace vision
} // namespace ram
