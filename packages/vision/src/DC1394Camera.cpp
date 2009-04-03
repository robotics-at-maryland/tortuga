/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/include/OpenCVCamera.h
 */

// STD Includes
#include <cassert>
#include <cstdlib>
#include <iostream>

// Library Includes
#include <dc1394/video.h>
#include <dc1394/camera.h>
#include <dc1394/conversions.h>

// Project includes
#include "vision/include/DC1394Camera.h"
#include "vision/include/OpenCVImage.h"

// Initialize static variables
dc1394_t* ram::vision::DC1394Camera::s_libHandle = 0;
size_t ram::vision::DC1394Camera::s_camCount = 0;

namespace ram {
namespace vision {

static const int DMA_BUFFER_SIZE = 10;    
    
DC1394Camera::DC1394Camera() :
    m_width(0),
    m_height(0),
    m_fps(0),
    m_camera(0),
    m_newFrame(0)
{
    initLibDC1394();

    // Grab our list of cameras 
    dc1394camera_list_t * list;
    dc1394error_t err = dc1394_camera_enumerate(s_libHandle, &list);
    assert(DC1394_SUCCESS == err && "Failed to enumerate cameras");
    assert(list->num > 0 && "No cameras found");

    // Create with the first camera 
    init(list->ids[0].guid);

    dc1394_camera_free_list(list);
}

DC1394Camera::DC1394Camera(size_t num) :
    m_width(0),
    m_height(0),
    m_fps(0),
    m_camera(0),
    m_newFrame(0)
{
    initLibDC1394();

    // Grab our list of cameras 
    dc1394camera_list_t * list;
    dc1394error_t err = dc1394_camera_enumerate(s_libHandle, &list);
    assert(DC1394_SUCCESS == err && "Failed to enumerate cameras");
    assert(list->num > 0 && "No cameras found");
    assert(num < list->num && "Num too large");

    // Create with the first camera 
    init(list->ids[num].guid);

    dc1394_camera_free_list(list);
}   

DC1394Camera::DC1394Camera(uint64_t guid) :
    m_width(0),
    m_height(0),
    m_fps(0),
    m_camera(0),
    m_newFrame(0)
{
    initLibDC1394();

    // Create out camera with the desired guid
    init(guid);
}

DC1394Camera::~DC1394Camera()
{
    // Stop any background image processing
    cleanup();
    
    dc1394_video_set_transmission(m_camera, DC1394_OFF);
    dc1394_capture_stop(m_camera);
    dc1394_camera_set_power(m_camera, DC1394_OFF);
    dc1394_camera_free(m_camera);

    free(m_newFrame->image);
    free(m_newFrame);
    
    shutdownLibDC1394();
}

void DC1394Camera::update(double timestep)
{
    dc1394video_frame_t* frame = 0;
    dc1394error_t err = DC1394_FAILURE;
    
    // Grab a frame
    err = dc1394_capture_dequeue(m_camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
    assert(DC1394_SUCCESS == err && "Could not capture a frame\n");
    
    // Convert to RGB and place in our new frame
    dc1394_convert_frames(frame, m_newFrame);
//    dc1394_convert_to_RGB8(frame->image, m_newFrame->getData(), 640, 480,
//                           frame->yuv_byte_order, frame->color_coding, 8);

    // Free the space back up on the queue
    err = dc1394_capture_enqueue(m_camera, frame);
    assert(DC1394_SUCCESS == err && "Could not enqueue used frame\n");


    // Copy image to public side of the interface        
    Image* newImage = new OpenCVImage(m_newFrame->image, 640, 480, false);

    // Flip RGB -> BGR
    unsigned char* data = newImage->getData();
    size_t numPixels = m_width * m_height;
    for (size_t i = 0; i < numPixels; ++i)
    {
        unsigned char tmp = *data;
        *data = *(data + 2);
        *(data + 2) = tmp;
        data += 3;
    }
    
    capturedImage(newImage);
    delete newImage;
}

size_t DC1394Camera::width()
{    
    return m_width;
}

size_t DC1394Camera::height()
{
    return m_height;
}

size_t DC1394Camera::fps()
{
    return (size_t)m_fps;    
}

void DC1394Camera::init(uint64_t guid)
{
    // Grab our camera 
    m_camera = dc1394_camera_new(s_libHandle, guid);
    if (!m_camera) {
        std::cout << "Failed to initialize camera with guid: " << guid
                  << std::endl;
        assert(m_camera && "Couldn't initialize camera");
    }

    // Determines settings and frame size
    dc1394error_t err = DC1394_FAILURE;
    dc1394video_mode_t videoMode = DC1394_VIDEO_MODE_640x480_YUV411;
    dc1394framerate_t frameRate = DC1394_FRAMERATE_30;
    
    err = dc1394_get_image_size_from_video_mode(m_camera, videoMode,
                                          &m_width, &m_height);
    assert(DC1394_SUCCESS == err && "Could not get image size");
    
    float fRate;
    err = dc1394_framerate_as_float(frameRate, &fRate);
    assert(DC1394_SUCCESS == err && "Could not get framerate as float");
    m_fps = fRate;

    // Create our RGB version of the frame
    m_newFrame = (dc1394video_frame_t*)calloc(1,sizeof(dc1394video_frame_t));
    m_newFrame->color_coding = DC1394_COLOR_CODING_RGB8;
    
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
    
void DC1394Camera::initLibDC1394()
{
    if (s_camCount == 0)
    {
        // Create library handle
        s_libHandle = dc1394_new();
    }

    // Increment reference count
    s_camCount++;
}
    
void DC1394Camera::shutdownLibDC1394()
{
    assert(s_camCount > 0 && "Invalid reference count");

    // Decrement reference count
    s_camCount--;
    
    if (s_camCount == 0)
    {
        // Free library
        dc1394_free(s_libHandle);
        s_libHandle = 0;
    }
}
    
} // namespace vision
} // namespace ram
