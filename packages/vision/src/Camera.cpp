/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Camera.h
 */

// STD Includes
#include <sstream>

// Library Includes
#include <boost/regex.hpp>

// Project Includes
#include "vision/include/Camera.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"

// For createCamera factory method
#include "vision/include/ImageCamera.h"
#include "vision/include/NetworkCamera.h"
#include "vision/include/FFMPEGNetworkCamera.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/DC1394Camera.h"

#include "vision/include/VisionSystem.h"

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
        if (current)
            getImage(current);
    }
    
    return inBackground;
}

bool Camera::waitForImage(Image* current, const boost::xtime &xt)
{
    bool result = m_imageLatch.await(xt);
    
    if(result)
    {
        if (current)
            getImage(current);
    }

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

vision::Camera* Camera::createCamera(const std::string input,
                                     const std::string configPath,
                                     std::string& message,
                                     const std::string cameraName)
{
    std::stringstream ss;    
    
    // Look up the configuration for the camera
    bool found = false;
    std::string nodeUsed;
    core::ConfigNode config(core::ConfigNode::fromString("{}"));
    if ("NONE" != configPath)
    {
        core::ConfigNode cfg(core::ConfigNode::fromFile(configPath));
        
        if (cfg.exists(cameraName))
        {
            config = cfg[cameraName];
            found = true;
        }
        else
        {
            cfg = VisionSystem::findVisionSystemConfig(cfg, nodeUsed);
            if (nodeUsed.size() > 0 && cfg.exists(cameraName))
            {
                config = cfg[cameraName];
                found = true;
            }
            
        }
    }

    Camera* camera = 0;
    if (found)
    {
        ss << " Using section \"" << nodeUsed << "\" for "
           << " \"" << cameraName << "\" from config file: \""
           << configPath << "\"";

        camera = Camera::createCamera(input, config, message);
        message = message + ss.str();
    }
    else
    {
        camera = Camera::createCamera(input, core::ConfigNode::fromString("{}"),
                                      message);
    }

    return camera;
}

vision::Camera* Camera::createCamera(const std::string input,
                                     core::ConfigNode config,
                                     std::string& message)
{
    static boost::regex camnum("\\d+");
    static boost::regex hostnamePort("([a-zA-Z0-9.-_]+):(\\d{1,5})");
    static boost::regex image("[^\\.]+.(jpg|png)");
    std::stringstream ss;
    ss << "Images coming from: ";
    
    if (boost::regex_match(input, camnum))
    {
        int camnum = boost::lexical_cast<int>(input);
        if (camnum >= 100)
        {
            size_t num = (size_t)camnum - 100;
            ss << "DC1394 Camera num:" << num;

            message = ss.str();
            return new vision::DC1394Camera(config, num);
        }
        else
        {
            ss << "Using OpenCV Camera #" << camnum;
            message = ss.str();
            return new vision::OpenCVCamera(camnum, true);
        }
    }

    boost::smatch what;
    if (boost::regex_match(input, what, hostnamePort))
    {
        std::string hostname = what.str(1);
        boost::uint16_t port =
            boost::lexical_cast<boost::uint16_t>(what.str(2));
        ss << "Streaming images from host: \"" << hostname
           << "\" on port " << port;
//        return new vision::NetworkCamera(hostname, port);
        return new vision::FFMPEGNetworkCamera(hostname, port);
    }
	
    if (boost::regex_match(input, image))
    {
        ss <<"'" << input <<"' image file";
	
        vision::Image* img = vision::Image::loadFromFile(input);
        vision::ImageCamera* c = new vision::ImageCamera(
            img->getWidth(), img->getHeight(), 30);
        c->newImage(img);
        //delete img;
        message = ss.str();
        return c;
    }
    
    ss << "'" << input << "' video file";
    message = ss.str();
    return new vision::OpenCVCamera(input);
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
