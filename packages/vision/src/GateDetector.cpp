/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/GateDetector.cpp
 */


// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/GateDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"


namespace ram {
namespace vision {

GateDetector::GateDetector(Camera* camera)
{
	cam = camera;
	frame = new OpenCVImage(640,480);
	gateX=0;
	gateY=0;
	found=false;
	//This frame will be a copy of the original rotated 90¼ counterclockwise.  
	//But only if the camera is on sideways, otherwise we do 640 by 480 like usual.
	//ie 640 by 480 if cameras on right (Or completely upsidedown!... sigh), else 480 by 640.
	gateFrame =cvCreateImage(cvSize(640,480),8,3);
	gateFrameRatios = cvCreateImage(cvGetSize(gateFrame),8,3);
}

GateDetector::~GateDetector()
{
	delete frame;
	cvReleaseImage(&gateFrame);
	cvReleaseImage(&gateFrameRatios);
}

double GateDetector::getX()
{
	return gateXNorm;
}

double GateDetector::getY()
{
	return gateYNorm;
}

void GateDetector::show(char* window)
{
	cvShowImage(window,((IplImage*)(gateFrame)));
}

IplImage* GateDetector::getAnalyzedImage()
{
	return (IplImage*)(gateFrame);
}

void GateDetector::update()
{	
	cam->getImage(frame);
	IplImage* image =(IplImage*)(*frame);
//  These lines are correct only if the camera is on sideways again.
//	rotate90Deg(image,gateFrame);//Rotate image into gateFrame, so that it will be vertical.
//	rotate90Deg(image,gateFrameRatios); 

// Otherwise just copy like this:
	
	//Could just say gateFrame=(IplImage*)(*frame), 
	//but I've heard openCV gets angry if you write pixels 
	//to image when it is getting images from a camera.
	//TODO: Before changing this, check the Camera class to see if it automatically copies anyway.
	cvCopyImage(image,gateFrame);
	cvCopyImage(gateFrame, gateFrameRatios);

	to_ratios(gateFrameRatios);
	found=gateDetect(gateFrameRatios,gateFrame,&gateX,&gateY) ? 1 : 0;
	gateXNorm=gateX;
	gateYNorm=gateY;
	gateXNorm/=image->width;
	gateXNorm/=image->height;		
}

} // namespace vision
} // namespace ram
