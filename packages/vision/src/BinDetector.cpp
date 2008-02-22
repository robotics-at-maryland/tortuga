/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/BinDetector.cpp
 */


// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/BinDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"


namespace ram {
namespace vision {

BinDetector::BinDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0)
{
    init(config);
}
    
BinDetector::BinDetector(Camera* camera) :
    cam(camera)
{
    init(core::ConfigNode::fromString("{}"));
}

void BinDetector::init(core::ConfigNode)
{
	frame = new OpenCVImage(640, 480);
	rotated = cvCreateImage(cvSize(640,480),8,3);//Its only 480 by 640 if the cameras on sideways
	binFrame =cvCreateImage(cvGetSize(rotated),8,3);
	found=0;
	binX=-1;
	binY=-1;
	binCount=0;
}
    
BinDetector::~BinDetector()
{
	delete frame;
	cvReleaseImage(&binFrame);
	cvReleaseImage(&rotated);
}

void BinDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}
    
void BinDetector::processImage(Image* input, Image* output)
{
	int binx=-1;
	int biny=-1;
	/*First argument to white_detect is a ratios frame, then a regular one*/
	IplImage* image =(IplImage*)(*input);
	
	//This is only right if the camera is on sideways... again.
	//rotate90Deg(image,rotated);

	//Else just copy
	cvCopyImage(image,rotated);
	image=rotated;//rotated is poorly named when camera is on correctly... oh well.
	//Set image to a newly copied space so we don't write over opencv's private memory space...
	//since opencv has a bad habit of making assumptions about what I want to do. :)
	cvCopyImage(image,binFrame);
	
	to_ratios(image);
	binCount=white_detect(image,binFrame,&binx,&biny);
	if (biny!=-1 && binx!=-1)
	{
		binX=binx;
		binX/=image->width;
		binY=biny;
		binY/=image->height;
		found=true;
	}
	else
	{
		found=false;
		binX=-1;
		binY=-1;
	}

        if (output)
        {
            OpenCVImage temp(binFrame, false);
            output->copyFrom(&temp);
        }
}

void BinDetector::show(char* window)
{
	cvShowImage(window,((IplImage*)(binFrame)));
}

IplImage* BinDetector::getAnalyzedImage()
{
	return (IplImage*)(binFrame);
}

double BinDetector::getX()
{
	return binX;
}

double BinDetector::getY()
{
	return binY;
}

} // namespace vision
} // namespace ram
