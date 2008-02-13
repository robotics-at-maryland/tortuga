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
#include "vision/include/Events.h"

RAM_CORE_EVENT_TYPE(ram::vision::Camera, RAW_IMAGE_CAPTURED);
RAM_CORE_EVENT_TYPE(ram::vision::Camera, IMAGE_CAPTURED);

namespace ram {
namespace vision {

Camera::Camera() :
    EventPublisher(core::EventHubPtr()),
    m_publicImage(0),
    m_imageLatch(1)
{
    /// TODO: Make me a basic image, and check that copying work properly
    m_publicImage = new OpenCVImage(640, 480);
}

Camera::~Camera()
{
    assert(m_cleanedUp && "You must call Camera::cleanup before you destruct"
                          "anything");
    assert(!backgrounded() &&
           "Camera must not be backgrounded for destruction");
    delete m_publicImage;
}

void Camera::getImage(Image* current)
{
    assert(current && "Can't copy into a null image");
    
    core::ReadWriteMutex::ScopedReadLock lock(m_imageMutex);

    // Copy over the image (uses copy assignment operator)
    current->copyFrom(m_publicImage);
}

bool Camera::waitForImage(Image* current)
{
    // We aren't even running in the background return
    if (!backgrounded())
        return false;
    
    // Wait for the next capturedImage call
    m_imageLatch.await();

    // Make sure we are still running the background
    bool inBackground = backgrounded();
    if (inBackground)
    {
        // Grab the image
        getImage(current);
    }
    
    return inBackground;
}

bool Camera::waitForImage(Image* current, const boost::xtime &xt)
{
    bool result = m_imageLatch.await(xt);
    
    if(result)
        getImage(current);

    return result;
}

void Camera::background(int rate)
{
    if (0 == m_imageLatch.getCount())
        m_imageLatch.resetCount(1);
    Updatable::background(rate);
}
    
void Camera::unbackground(bool join)
{
    Updatable::unbackground(join);
    m_imageLatch.countDown();
}
    
void Camera::cleanup()
{
    unbackground(true);
    m_cleanedUp = true;
}
    
void Camera::capturedImage(Image* newImage)
{
    assert(newImage && "Can't copy null image");
    
    {    
        core::ReadWriteMutex::ScopedWriteLock lock(m_imageMutex);

        // Copy over the image data to the public image
        // (Silently ignore a new image if the new image is null.)
	if (newImage)
        {
            // Before processing
            publish(Camera::RAW_IMAGE_CAPTURED,
                    ImageEventPtr(new ImageEvent(newImage)) );
                    
            copyToPublic(newImage, m_publicImage);

            // After processing
            publish(Camera::IMAGE_CAPTURED,
                    ImageEventPtr(new ImageEvent(m_publicImage)) );
        }
    }
    
    // Now release all waiting threads
    m_imageLatch.countDown();
    // Reset count to one
    m_imageLatch.resetCount(1);
}

void Camera::copyToPublic(Image* newImage, Image* publicImage)
{
    if (newImage && publicImage)
        publicImage->copyFrom(newImage);    
}
    
} // namespace vision
} // namespace ram
