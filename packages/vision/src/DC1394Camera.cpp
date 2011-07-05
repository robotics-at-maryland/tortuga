/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/DC1934Camera.cpp
 */

// STD Includes
#include <cassert>
#include <cstdlib>
#include <cstdio>

// Library Includes
#include <dc1394/video.h>
#include <dc1394/camera.h>
#include <dc1394/conversions.h>

#include <log4cpp/Category.hh>

#include <boost/algorithm/string/case_conv.hpp>

// Project includes
#include "vision/include/DC1394Camera.h"
#include "vision/include/OpenCVImage.h"

// Initialize static variables
dc1394_t* ram::vision::DC1394Camera::s_libHandle = 0;
size_t ram::vision::DC1394Camera::s_camCount = 0;
ram::core::ReadWriteMutex ram::vision::DC1394Camera::s_initMutex;

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Camera"));

namespace ram {
namespace vision {

static const int DMA_BUFFER_SIZE = 15;    

DC1394Camera::DC1394Camera(core::ConfigNode config) :
    m_width(0),
    m_height(0),
    m_fps(0),
    m_camera(0),
    m_frameNum(0)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(s_initMutex);
        initLibDC1394();
    }

    // Grab our list of cameras 
    dc1394camera_list_t *list;
    dc1394error_t err = dc1394_camera_enumerate(s_libHandle, &list);
    LOGGER.infoStream() << "Number of Cameras: " << list->num;
    assert(DC1394_SUCCESS == err && "Failed to enumerate cameras");
    assert(list->num > 0 && "No cameras found");

    // Create with the first camera 
    init(config, list->ids[0].guid);

    dc1394_camera_free_list(list);
}

DC1394Camera::DC1394Camera(core::ConfigNode config, size_t num) :
    m_width(0),
    m_height(0),
    m_fps(0),
    m_camera(0),
    m_frameNum(0)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(s_initMutex);
        initLibDC1394();
    }

    // Grab our list of cameras 
    dc1394camera_list_t * list;
    dc1394error_t err = dc1394_camera_enumerate(s_libHandle, &list);
    LOGGER.infoStream() << "Number of Cameras: " << list->num;
    assert(DC1394_SUCCESS == err && "Failed to enumerate cameras");
    assert(list->num > 0 && "No cameras found");
    assert(num < list->num && "Num too large");

    // Create with the first camera 
    init(config, list->ids[num].guid);

    dc1394_camera_free_list(list);
}   

DC1394Camera::DC1394Camera(core::ConfigNode config, uint64_t guid) :
    m_width(0),
    m_height(0),
    m_fps(0),
    m_camera(0),
    m_frameNum(0)
{
    {
        core::ReadWriteMutex::ScopedWriteLock lock(s_initMutex);
        initLibDC1394();
    }

    // Create out camera with the desired guid
    init(config, guid);
}

DC1394Camera::~DC1394Camera()
{
    // Stop any background image processing
    cleanup();

    LOGGER.infoStream() << m_guid << ": Turning off camera ";
    dc1394_video_set_transmission(m_camera, DC1394_OFF);
    dc1394_capture_stop(m_camera);

    // not sure why we would want to turn off the camera
    // especially since we never turn it on anywhere
    // dc1394_camera_set_power(m_camera, DC1394_OFF);

    dc1394_camera_free(m_camera);

    shutdownLibDC1394();
}

void DC1394Camera::update(double timestep)
{
    dc1394video_frame_t* frame = 0;
    dc1394error_t err = DC1394_FAILURE;
    
    // Grab a frame
    LOGGER.debugStream() << m_guid << ": Grabbing frame " << ++m_frameNum;
    err = dc1394_capture_dequeue(m_camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
    assert(DC1394_SUCCESS == err && "Could not capture a frame\n");

    // See if we have dropped any frames
    if(frame->frames_behind == DMA_BUFFER_SIZE - 1)
    {
        LOGGER.warn("PROBABLE FRAME DROP");
    }
    // Let us know if we are not getting the most recent frame
    LOGGER.debugStream() << m_guid << ": Frame Position: " << frame->id 
                         << " Frames Behind: " << frame->frames_behind
                         << " DMA Buffers: " << DMA_BUFFER_SIZE
                         << " DMA Timestamp: " << frame->timestamp;

    // Put the DC1394 buffer into a temporary Image so we can copy it
    // to the public side.
    OpenCVImage newImage(frame->image, m_width, m_height,
                         false, Image::PF_RGB_8);

    // Copy image to public side of the interface
    capturedImage(&newImage);

    // Free the space back up on the queue
    LOGGER.debugStream() << m_guid << ": Releasing frame " << m_frameNum;
    err = dc1394_capture_enqueue(m_camera, frame);
    assert(DC1394_SUCCESS == err && "Could not enqueue used frame\n");
}

size_t DC1394Camera::width()
{    
    return m_width;
}

size_t DC1394Camera::height()
{
    return m_height;
}

double DC1394Camera::fps()
{
    return m_fps;
}

double DC1394Camera::duration()
{
    return 0;
}

void DC1394Camera::seekToTime(double seconds)
{
}

double DC1394Camera::currentTime()
{
    return 0;
}

void DC1394Camera::init(core::ConfigNode config, uint64_t guid)
{
    LOGGER.infoStream() << guid << ": Initializing camera ";
    m_guid = guid;

    // Grab our camera 
    m_camera = dc1394_camera_new(s_libHandle, guid);
    if (!m_camera) {
        LOGGER.errorStream() <<
            "Failed to initialize camera with guid: " << guid;
        assert(m_camera && "Couldn't initialize camera");
    }

    dc1394switch_t isoOn;
    // Get the current iso status
    if(dc1394_video_get_transmission(m_camera, &isoOn) != DC1394_SUCCESS)
    {
        assert(false && "Could not get ISO status");
    }

    // If the ISO status is currently on, we want to stop it.
    if(isoOn == DC1394_ON)
    {
        std::cout << "ISO Transmission ON" << std::endl;
        if (dc1394_video_set_transmission(m_camera, DC1394_OFF) != DC1394_SUCCESS)
        {
            assert(false && "Could not stop ISO transmission");
        }
        else
        {
            if(dc1394_video_get_transmission(m_camera, &isoOn) != DC1394_SUCCESS)
            {
                assert(false && "Could not get ISO status");
            }
            if(isoOn == DC1394_OFF)
                std::cout << "ISO Transmission turned off successfully" << std::endl;
        }
    }

    // Release all of the iso bandwith and memory from previous failure
    std::cout << "Releasing old resources" << std::endl;
    dc1394_iso_release_all(m_camera);

    // Determines settings and frame size
    dc1394error_t err = DC1394_FAILURE;
    dc1394video_mode_t videoMode = DC1394_VIDEO_MODE_640x480_RGB8;
    dc1394framerate_t frameRate = DC1394_FRAMERATE_7_5;

    // Check for the whitebalance feature
    dc1394feature_info_t whiteBalance;
    whiteBalance.id = DC1394_FEATURE_WHITE_BALANCE;
    err = dc1394_feature_get(m_camera, &whiteBalance);
    assert(DC1394_SUCCESS == err && "Could not get white balance feature info");
    bool hasWhiteBalance = (whiteBalance.available == DC1394_TRUE);

    uint32_t uValue, vValue;
    if (hasWhiteBalance)
    {
        err = dc1394_feature_whitebalance_get_value(m_camera, &uValue, &vValue);
        LOGGER.infoStream() << m_guid
                            << ": Current U: " << uValue
                            << " V: " << vValue;
    }
    
    // Actually set the values if the user wants to
    if (config.exists("uValue") && config.exists("vValue"))
    {
        // Read in config values
        uint32_t u_b_value = static_cast<uint32_t>(config["uValue"].asInt());
        uint32_t v_r_value = static_cast<uint32_t>(config["vValue"].asInt());

        // Set values
        setWhiteBalance(u_b_value, v_r_value);
    }
    else if (config.exists("uValue") || config.exists("vValue"))
    {
        assert(false && "Must set both the U and V values for white balance");
    }

    err = dc1394_feature_whitebalance_get_value(m_camera, &uValue, &vValue);
    LOGGER.infoStream() << m_guid
                        << ": Set U: " << uValue 
                        << " V: " << vValue;

    if (config.exists("brightness"))
    {
        uint32_t value = static_cast<uint32_t>(config["brightness"].asInt());
        setBrightness(value);
    }

    if (config.exists("shutter"))
    {
        int ival = config["shutter"].asInt();
        uint32_t value = static_cast<uint32_t>(ival);
        if(ival > 0)
            setShutter(value);
        else
            setShutter(value, true);
    }

    if (config.exists("exposure"))
    {
        int ival = config["exposure"].asInt();
        uint32_t value = static_cast<uint32_t>(ival);
        if(ival > 0)
            setExposure(value);
        else
            setExposure(value, true);
    }

    if (config.exists("gamma"))
    {
        int ival = config["gamma"].asInt();
        uint32_t value = static_cast<uint32_t>(ival);
        if(ival > 0)
            setGamma(value);
        else
            setGamma(value, true);
    }
    
    if (config.exists("gain"))
    {
        int ival = config["gain"].asInt();
        uint32_t value = static_cast<uint32_t>(ival);
        if(ival > 0)
            setGain(value);
        else
            setGain(value, true);
    }

    // Grab image size
    err = dc1394_get_image_size_from_video_mode(m_camera, videoMode,
                                                &m_width, &m_height);
    assert(DC1394_SUCCESS == err && "Could not get image size");
    
    float fRate;
    err = dc1394_framerate_as_float(frameRate, &fRate);
    assert(DC1394_SUCCESS == err && "Could not get framerate as float");
    m_fps = fRate;

    // Setup the capture
    err = dc1394_video_set_iso_speed(m_camera, DC1394_ISO_SPEED_400);
    assert(DC1394_SUCCESS == err && "Could not set iso speed");
           
    err = dc1394_video_set_mode(m_camera, videoMode);
    assert(DC1394_SUCCESS == err && "Could not set video mode");

    err = dc1394_video_set_framerate(m_camera, frameRate);
    assert(DC1394_SUCCESS == err && "Could not set framerate");

    // Start data transfer
    err = dc1394_video_set_transmission(m_camera, DC1394_ON);
    assert(DC1394_SUCCESS == err && "Could not start camera iso transmission");

    err = dc1394_capture_setup(m_camera, DMA_BUFFER_SIZE,
                               DC1394_CAPTURE_FLAGS_DEFAULT);
    assert(DC1394_SUCCESS == err && "Could not setup camera make sure"
           " that the video mode and framerate are supported by your camera");
}

void DC1394Camera::setBrightness(uint32_t value, bool makeAuto)
{
    // Grab the white balance feature
    dc1394feature_info_t brightness;
    brightness.id = DC1394_FEATURE_BRIGHTNESS;

    if (makeAuto) {
        LOGGER.infoStream() << m_guid << ": Setting brightness automatically";
    } else {
        LOGGER.infoStream() << m_guid << ": Setting brightness to " << value;
    }

    dc1394error_t err = dc1394_feature_get(m_camera, &brightness);
    assert(DC1394_SUCCESS == err && "Could not get brightness feature info");
    
    // Make sure its available
    assert((brightness.available == DC1394_TRUE) &&
           "Brightness not supported by camera");

    if (makeAuto)
    {
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_BRIGHTNESS,
                                      DC1394_FEATURE_MODE_AUTO);
        assert(DC1394_SUCCESS == err && "Could not set brightness to auto");
    }
    else
    {
        // Set manual mode
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_BRIGHTNESS,
                                      DC1394_FEATURE_MODE_MANUAL);
        assert(DC1394_SUCCESS == err && "Could not set brightness to manual");

        // Error on out of bounds values
        if ((value > brightness.max) || (value < brightness.min))
        {
            fprintf(stderr, "ERROR: Brightness is out of bounds: (%u, %u)\n",
                    brightness.min, brightness.max);
            assert(false && "Brightness out of bounds");
        }
        
        // Set value
        err = dc1394_feature_set_value(m_camera, DC1394_FEATURE_BRIGHTNESS,
                                       value);
        assert(DC1394_SUCCESS == err && "Could not set brightness");
    }

    LOGGER.infoStream() << m_guid << ": Successfully set brightness";
}

void DC1394Camera::setExposure(uint32_t value, bool makeAuto)
{
    // Grab the white balance feature
    dc1394feature_info_t exposure;
    exposure.id = DC1394_FEATURE_EXPOSURE;

    dc1394error_t err = dc1394_feature_get(m_camera, &exposure);
    assert(DC1394_SUCCESS == err && "Could not get exposure feature info");
    
    // Make sure its available
    assert((exposure.available == DC1394_TRUE) &&
           "Exposure not supported by camera");

    if (makeAuto)
    {
        LOGGER.infoStream() << m_guid << ": Setting exposure automatically";
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_EXPOSURE,
                                      DC1394_FEATURE_MODE_AUTO);
        assert(DC1394_SUCCESS == err && "Could not set exposure to auto");
    }
    else
    {
        LOGGER.infoStream() << m_guid << ": Setting exposure to " << value;
        // Set manual mode
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_EXPOSURE,
                                      DC1394_FEATURE_MODE_MANUAL);
        assert(DC1394_SUCCESS == err && "Could not set exposure to manual");

        // Error on out of bounds values
        if ((value > exposure.max) || (value < exposure.min))
        {
            fprintf(stderr, "ERROR: Exposure is out of bounds: (%u, %u)\n",
                    exposure.min, exposure.max);
            assert(false && "Exposure out of bounds");
        }
        
        // Set value
        err = dc1394_feature_set_value(m_camera, DC1394_FEATURE_EXPOSURE,
                                       value);
        assert(DC1394_SUCCESS == err && "Could not set exposure");
    }

    LOGGER.infoStream() << m_guid << ": Successfully set exposure";
}

void DC1394Camera::setShutter(uint32_t value, bool makeAuto)
{
    dc1394feature_info_t shutter;
    shutter.id = DC1394_FEATURE_SHUTTER;

    if(makeAuto)
    {
        LOGGER.infoStream() << m_guid << ": setting shutter automatically";
    }
    else
    {
        LOGGER.infoStream() << m_guid << ": Setting shutter to "
                            << "Shutter Value: " << value;
    }

    dc1394error_t err = dc1394_feature_get(m_camera, &shutter);
    assert(DC1394_SUCCESS == err && "Could not get shutter feature info");
    
    // Make sure its available
    assert((shutter.available == DC1394_TRUE) &&
           "Shutter not supported by camera");

    // Turn it on if its off and switchable
    dc1394bool_t isSwitchable;
    if(dc1394_feature_is_switchable(m_camera, shutter.id, &isSwitchable) != 
       DC1394_SUCCESS)
    {
        assert(false && "could not determine if shutter is switchable");
    }
    else
    {
        if(isSwitchable == DC1394_TRUE)
        {
            dc1394switch_t isPowered;
            if(dc1394_feature_get_power(m_camera, shutter.id, &isPowered) !=
               DC1394_SUCCESS)
            {
                assert(false && "could not determine if shutter is powered");
            }
            else
            {
                if(dc1394_feature_set_power(m_camera, shutter.id, DC1394_ON) !=
                   DC1394_SUCCESS)
                {
                    assert(false && "could not set shutter power");
                }
                else
                {
                    LOGGER.infoStream() << m_guid << 
                        ": Successfully set shutter power";
                }
            }
        }
    }

    uint32_t shutterMin = shutter.min;
    uint32_t shutterMax = shutter.max;

    if (makeAuto)
    {
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_SHUTTER,
                                      DC1394_FEATURE_MODE_AUTO);
        assert(DC1394_SUCCESS == err && "Could not set shutter to auto");
    }
    else
    {
        // Set manual mode
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_SHUTTER,
                                      DC1394_FEATURE_MODE_MANUAL);
        assert(DC1394_SUCCESS == err && "Could not set shutter to manual");

        // Error on out of bounds values
        if (value > shutterMax || value < shutterMin)
        {
            fprintf(stderr, "ERROR: Shutter out of bounds: (%u, %u)\n",
                    shutterMin, shutterMax);
            assert(false && "White balance out of bounds");
        }
        
        // Set values
        err = dc1394_feature_set_value(m_camera, DC1394_FEATURE_SHUTTER,
                                       value);
        assert(DC1394_SUCCESS == err && "Could not set shutter");
    }

    LOGGER.infoStream() << m_guid << ": Successfully set shutter";
}
    
void DC1394Camera::setGamma(uint32_t value, bool makeAuto)
{
    dc1394feature_info_t gamma;
    gamma.id = DC1394_FEATURE_GAMMA;

    if(makeAuto)
    {
        LOGGER.infoStream() << m_guid << ": setting gamma automatically";
    }
    else
    {
        LOGGER.infoStream() << m_guid << ": Setting gamma to "
                            << "Gamma Value: " << value;
    }

    dc1394error_t err = dc1394_feature_get(m_camera, &gamma);
    assert(DC1394_SUCCESS == err && "Could not get gamma feature info");
    
    // Make sure its available
    assert((gamma.available == DC1394_TRUE) &&
           "Gamma not supported by camera");

    uint32_t gammaMin = gamma.min;
    uint32_t gammaMax = gamma.max;

    if (makeAuto)
    {
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_GAMMA,
                                      DC1394_FEATURE_MODE_AUTO);
        assert(DC1394_SUCCESS == err && "Could not set gamma to auto");
    }
    else
    {
        // Set manual mode
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_GAMMA,
                                      DC1394_FEATURE_MODE_MANUAL);
        assert(DC1394_SUCCESS == err && "Could not set gamma to manual");

        // Error on out of bounds values
        if (value > gammaMax || value < gammaMin)
        {
            fprintf(stderr, "ERROR: Gamma out of bounds: (%u, %u)\n",
                    gammaMin, gammaMax);
            assert(false && "White balance out of bounds");
        }
        
        // Set values
        err = dc1394_feature_set_value(m_camera, DC1394_FEATURE_GAMMA,
                                       value);
        assert(DC1394_SUCCESS == err && "Could not set gamma");
    }

    LOGGER.infoStream() << m_guid << ": Successfully set gamma";
}


void DC1394Camera::setGain(uint32_t value, bool makeAuto)
{
    dc1394feature_info_t gain;
    gain.id = DC1394_FEATURE_GAIN;

    if(makeAuto)
    {
        LOGGER.infoStream() << m_guid << ": setting gain automatically";
    }
    else
    {
        LOGGER.infoStream() << m_guid << ": Setting gain to "
                            << "Gain Value: " << value;
    }

    dc1394error_t err = dc1394_feature_get(m_camera, &gain);
    assert(DC1394_SUCCESS == err && "Could not get gain feature info");
    
    // Make sure its available
    assert((gain.available == DC1394_TRUE) &&
           "Gain not supported by camera");

    uint32_t gainMin = gain.min;
    uint32_t gainMax = gain.max;

    if (makeAuto)
    {
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_GAIN,
                                      DC1394_FEATURE_MODE_AUTO);
        assert(DC1394_SUCCESS == err && "Could not set gain to auto");
    }
    else
    {
        // Set manual mode
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_GAIN,
                                      DC1394_FEATURE_MODE_MANUAL);
        assert(DC1394_SUCCESS == err && "Could not set gain to manual");

        // Error on out of bounds values
        if (value > gainMax || value < gainMin)
        {
            fprintf(stderr, "ERROR: Gain out of bounds: (%u, %u)\n",
                    gainMin, gainMax);
            assert(false && "White balance out of bounds");
        }
        
        // Set values
        err = dc1394_feature_set_value(m_camera, DC1394_FEATURE_GAIN,
                                       value);
        assert(DC1394_SUCCESS == err && "Could not set gain");
    }

    LOGGER.infoStream() << m_guid << ": Successfully set gain";
}


void DC1394Camera::setWhiteBalance(uint32_t uValue, uint32_t vValue,
                                   bool makeAuto)
{
    // Grab the white balance feature
    dc1394feature_info_t whiteBalance;
    whiteBalance.id = DC1394_FEATURE_WHITE_BALANCE;

    if (makeAuto) {
        LOGGER.infoStream() << m_guid << ": Setting white balance automatically";
    } else {
        LOGGER.infoStream() << m_guid << ": Setting white balance to "
                            << "U Value: " << uValue << " "
                            << "V Value: " << vValue;
    }

    dc1394error_t err = dc1394_feature_get(m_camera, &whiteBalance);
    assert(DC1394_SUCCESS == err && "Could not get white balance feature info");
    
    // Make sure its available
    assert((whiteBalance.available == DC1394_TRUE) &&
           "White balance not supported by camera");

    if (makeAuto)
    {
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_WHITE_BALANCE,
                                      DC1394_FEATURE_MODE_AUTO);
        assert(DC1394_SUCCESS == err && "Could not set whitebalance to auto");
    }
    else
    {
        // Set manual mode
        err = dc1394_feature_set_mode(m_camera, DC1394_FEATURE_WHITE_BALANCE,
                                      DC1394_FEATURE_MODE_MANUAL);
        assert(DC1394_SUCCESS == err && "Could not set whitebalance to manual");

        // Store the values
        uint32_t whiteMax = whiteBalance.max;
        uint32_t whiteMin = whiteBalance.min;
        
        // Error on out of bounds values
        if (((uValue > whiteMax) || (uValue < whiteMin)) ||
            ((vValue > whiteMax) || (vValue < whiteMin)))
        {
            fprintf(stderr, "ERROR: WhiteBalance out of bounds: (%u, %u)\n",
                    whiteMin, whiteMax);
            assert(false && "White balance out of bounds");
        }
        
        // Set values
        err = dc1394_feature_whitebalance_set_value(m_camera, uValue, vValue);
        assert(DC1394_SUCCESS == err && "Could not set whitebalance");
    }

    LOGGER.infoStream() << m_guid << ": Successfully set white balance";
}

    
void DC1394Camera::initLibDC1394()
{
    // Increment reference count
    s_camCount++;

    if (s_camCount == 1)
    {
//        int channel = 0;
        // ioctl(open("/dev/video1394/0", VIDEO1394_IOC_UNLISTEN_CHANNEL, &channel));
        // channel = 1;
        // ioctl(open("/dev/video1394/0", VIDEO1394_IOC_UNLISTEN_CHANNEL, &channel));
              
        // Create library handle
        LOGGER.infoStream() << "Creating dc1394 library handle";
        s_libHandle = dc1394_new();
    }
}
    
void DC1394Camera::shutdownLibDC1394()
{
    assert(s_camCount > 0 && "Invalid reference count");

    // Decrement reference count
    s_camCount--;
    
    if (s_camCount == 0)
    {
        // Free library
        LOGGER.infoStream() << "Freeing dc1394 library handle";
        dc1394_free(s_libHandle);
        s_libHandle = 0;
    }
}
    
} // namespace vision
} // namespace ram
