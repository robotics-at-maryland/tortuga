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
#include <iostream>
#include <stdio.h>

// Library Includes
#include "highgui.h"

// Project includes
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

OpenCVCamera::OpenCVCamera(int camNum) :
    m_camCapture(new cv::VideoCapture(camNum)),
    m_live(true)
{
}

OpenCVCamera::OpenCVCamera(std::string filename) :
    m_camCapture(new cv::VideoCapture(filename)),
    m_live(false)
{
}

OpenCVCamera::~OpenCVCamera()
{
    // Have to stop background capture before we release the capture!
    cleanup();
    delete m_camCapture;
}

void OpenCVCamera::update(double timestep)
{
    // Capture image to the buffer
    if (m_camCapture->grab())
    {
        cv::Mat mat;
        m_camCapture->retrieve(mat);

        // Create a new image and return it, image does not own the wrapped
        // IplImage and thus will not delete it!
        IplImage img = (IplImage) mat;
        OpenCVImage newImage(&img, false, Image::PF_BGR_8);
        
        // Copy image to public side of the interface        
        capturedImage(&newImage);
    }
}

size_t OpenCVCamera::width()
{    
    int width = 0;
    //if open cv returns 0 for this, we have to assume a size
    width = (size_t) m_camCapture->get(CV_CAP_PROP_FRAME_WIDTH);
    if (width <= 0)
        return 640;
    return width;
}

size_t OpenCVCamera::height()
{
    int height = 0;
    //if open cv returns 0 for this, we have to assume a size
    height = (size_t) m_camCapture->get(CV_CAP_PROP_FRAME_HEIGHT);
    if (height <= 0)
        return 480;
    return height;
}

double OpenCVCamera::fps()
{
    return m_camCapture->get(CV_CAP_PROP_FPS);
}
    
double OpenCVCamera::duration()
{
    if (m_live)
    {
        return 0;
    } else {
        double frames = m_camCapture->get(CV_CAP_PROP_FRAME_COUNT);
        return frames / fps();
    }
}

void OpenCVCamera::seekToTime(double seconds)
{
    if (!m_live)
    {
        m_camCapture->set(CV_CAP_PROP_POS_FRAMES, seconds * fps());
    }
}

double OpenCVCamera::currentTime()
{
    if (m_live)
    {
        return 0;
    } else {
        double frames = m_camCapture->get(CV_CAP_PROP_POS_FRAMES);
        return frames / fps();
    }
}

} // namespace vision
} // namespace ram
