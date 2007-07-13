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

OpenCVCamera::OpenCVCamera()
{
	m_calibration=NULL;
	m_camCapture=cvCaptureFromCAM(0);
	assert(m_camCapture && "Error creating camera (default constructor)");
}

//forward=true;
//downward=false;
OpenCVCamera::OpenCVCamera(int camNum, bool forward)
{
	//What does camnum say about which camera this is?
	m_calibration=new Calibration(this);
	m_calibration->setCalibration(forward);
    m_camCapture = cvCaptureFromCAM(camNum);

    /// TODO: Handle the more gracefully
    assert(m_camCapture && "Error creating camera");
}

OpenCVCamera::OpenCVCamera(std::string filename)
{
	m_camCapture = cvCaptureFromFile(filename.c_str());
	m_calibration=NULL;
	assert(m_camCapture && "error creating camera from file");
}

OpenCVCamera::~OpenCVCamera()
{
    // Have to stop background capture before we release the capture!
    cleanup();
    cvReleaseCapture(&m_camCapture);
}

//make the returning of calibrated images invisible to the vision code
void OpenCVCamera::getImage(Image* undistorted)
{
	std::cout<<"Image width and height"<<undistorted->getWidth()<<" "<<undistorted->getHeight()<<"\n";
	if (m_calibration!=NULL)
		getCalibratedImage(undistorted);
	else
		Camera::getImage(undistorted);
}

void OpenCVCamera::getUncalibratedImage(Image* distorted)
{
	Camera::getImage(distorted);
}

void OpenCVCamera::getCalibratedImage(Image* undistorted)
{
	assert(undistorted && "Can't calibrate into a null image");
	core::ReadWriteMutex::ScopedReadLock lock(m_imageMutex);
	// Copy over the image (uses copy assignment operator)
    m_calibration->calibrateImage(((IplImage*)(*m_publicImage)),((IplImage*)*undistorted));
}

void OpenCVCamera::update(double timestep)
{
    if (cvGrabFrame(m_camCapture))
    {
        // Create a new image and return it, image does not own the wrapped
        // IplImage and thus will not detele it!
        Image* newImage = new OpenCVImage(cvRetrieveFrame(m_camCapture),
                                          false);

        // Copy image to public side of the interface        
        capturedImage(newImage);
		delete newImage;
    }
    else
    {
        /// TODO: handle gracefully
        assert(false && "Cam Capture Failed");
    }
}

size_t OpenCVCamera::width()
{	
	int width;
    if (!(width=(size_t)cvGetCaptureProperty(m_camCapture, CV_CAP_PROP_FRAME_WIDTH)))
		return 640;
	return width;
}

size_t OpenCVCamera::height()
{
	int width;
    if (!(width=(size_t)cvGetCaptureProperty(m_camCapture, CV_CAP_PROP_FRAME_WIDTH)))
		return 480;
	return width;
}

} // namespace vision
} // namespace ram
