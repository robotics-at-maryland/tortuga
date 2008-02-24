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

// Library Includes
#include "highgui.h"

// Project includes
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Calibration.h"

namespace ram {
namespace vision {

OpenCVCamera::OpenCVCamera() :
    m_calibration(0),
    m_camCapture(cvCaptureFromCAM(0))
{
    assert(m_camCapture && "Error creating camera (default constructor)");
}

//forward=true;
//downward=false;
OpenCVCamera::OpenCVCamera(int camNum, bool forward) :
    // Turn off calibration for now
    m_calibration(0),
//    m_calibration(new Calibration(this)),
    m_camCapture(cvCaptureFromCAM(camNum))
{
    //What does camnum say about which camera this is?
    //m_calibration->setCalibration(forward);

    /// TODO: Handle the more gracefully
    assert(m_camCapture && "Error creating camera");
}

OpenCVCamera::OpenCVCamera(std::string filename) :
    m_calibration(0),
    m_camCapture(cvCaptureFromFile(filename.c_str()))
{
    assert(m_camCapture && "error creating camera from file");
}

OpenCVCamera::~OpenCVCamera()
{
    // Have to stop background capture before we release the capture!
    cleanup();
    cvReleaseCapture(&m_camCapture);
}

void OpenCVCamera::update(double timestep)
{
    if (cvGrabFrame(m_camCapture))
    {
//        std::cout<<"Grabbed Frame\n";
        
        // Create a new image and return it, image does not own the wrapped
        // IplImage and thus will not delete it!
        Image* newImage = new OpenCVImage(cvRetrieveFrame(m_camCapture),
                                          false);
        
        // Copy image to public side of the interface        
        capturedImage(newImage);

        // Only deletes the "wrapper" Image object, not the IplImage
        delete newImage;
    }
    else
    {
        //std::cout<<"Failed to grab frame\n";
        /// TODO: handle gracefully
        //assert(false && "Cam Capture Failed");
    }
}

size_t OpenCVCamera::width()
{    
    int width = 0;
    //if open cv returns 0 for this, we have to assume a size
    width = (size_t)cvGetCaptureProperty(m_camCapture,
                                         CV_CAP_PROP_FRAME_WIDTH);
    if (width <= 0)
        return 640;
    return width;
}

size_t OpenCVCamera::height()
{
    int height = 0;
    //if open cv returns 0 for this, we have to assume a size
    height = (size_t)cvGetCaptureProperty(m_camCapture,
                                          CV_CAP_PROP_FRAME_HEIGHT);
    if (height <= 0)
        return 480;
    return height;
}

size_t OpenCVCamera::fps()
{
    int fps = 0;
    //if open cv returns 0 for this, we have to assume an fps
    fps = (size_t)cvGetCaptureProperty(m_camCapture, CV_CAP_PROP_FPS);

    if (fps <= 0)
        return 30;
    return fps;    
}
    
void OpenCVCamera::copyToPublic(Image* newImage, Image* publicImage)
{
    // Only calibration if we its possible
    if (m_calibration)
    {
        m_calibration->calibrateImage(((IplImage*)(*publicImage)),
                                      ((IplImage*)(*newImage)) );
    }
    else
    {
        Camera::copyToPublic(newImage, publicImage);
    }
}

} // namespace vision
} // namespace ram
