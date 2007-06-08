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

// Library Includes
#include "highgui.h"

// Project incldues
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

OpenCVCamera::OpenCVCamera(int camNum)
{
    CvCapture* m_camCapture = cvCaptureFromCAM(camNum);

    /// TODO: Handle the more gracefully
    assert(m_camCapture && "Error creating camera");
}

OpenCVCamera::~OpenCVCamera()
{
    cvReleaseCapture(&m_camCapture);
}

Image* OpenCVCamera::getImage()
{
    if (cvGrabFrame(m_camCapture))
    {
        // Create a new image and return it, image does not! own the wrapped
        // IplImage and thus will not detele it
        return new OpenCVImage(cvRetrieveFrame(m_camCapture),
                               false);
    }
    else
    {
        /// TODO: handle gracefully
        assert(false && "Cam Capture Failed");
    }
	
}

} // namespace vision
} // namespace ram
