/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/include/CvCamera.h
 */

#include "vision/include/OpenCvImage.h"
#include "highgui.h"

namespace ram
{
namespace vision
{

CvCamera::CvCamera(int camNum)
{
	CvCapture* camCapture=cvCaptureFromCAM(camNum);
}

OpenCvImage* CvCamera::getImage()
{
	if (cvGrabFrame(camCapture))
	{
		return new OpenCvImage(cvRetrieveFrame(camCapture));
	}
	else
	{
		/// TODO: handle gracefully
		assert(false && "Cam Capture Failed");
	}
	
}

}
}