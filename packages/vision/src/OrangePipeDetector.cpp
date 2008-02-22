/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/OrangePipeDetector.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"

using namespace std;

namespace ram {
namespace vision {

OrangePipeDetector::OrangePipeDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0)
{
    init(config);
}
    
OrangePipeDetector::OrangePipeDetector(Camera* camera) :
    cam(camera)
{
    init(core::ConfigNode::fromString("{}"));
}

void OrangePipeDetector::init(core::ConfigNode)
{
    found=0;
    angle=0;
    frame = new OpenCVImage(640, 480);
    rotated = cvCreateImage(cvSize(640,480),8,3);//480 by 640 if camera is on sideways, else 640 by 480.
    lineX=lineY=0;    
}
    
double OrangePipeDetector::getX()
{
	return lineX;
}

double OrangePipeDetector::getY()
{
	return lineY;
}

double OrangePipeDetector::getAngle()
{
	return angle;
}

OrangePipeDetector::~OrangePipeDetector()
{
	delete frame;
	cvReleaseImage(&rotated);
}

void OrangePipeDetector::show(char* window)
{
	cvShowImage(window,rotated);
}

IplImage* OrangePipeDetector::getAnalyzedImage()
{
	return (IplImage*)(rotated);
}

void OrangePipeDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}

void OrangePipeDetector::processImage(Image* input, Image* output)
{
	//Plan is:  Search out orange with a strict orange filter, as soon as we see a good deal of orange
	// use a less strict filter, and reduce the amount we need to see.
	//In theory this makes us follow the pipeline as long as possible, but be reluctant to follow some
	//arbitrary blob until we know its of a large size and the right shade of orange.  
	//If the pipeline is found, the angle found by hough is reported.  
	
	//Mask orange takes frame, then alter image, then strictness (true=more strict, false=more lenient)
	IplImage* image =(IplImage*)(*input);

//	rotate90Deg(image,rotated);//Only do this if the camera is attached sideways again.
	cvCopyImage(image,rotated);//Poorly named if the cameras not on sideways... oh well.
	image=rotated;
	if (!found)
	{
		int orange_count=mask_orange(image,true,true);
		if (orange_count>1000)//this number is in pixels.
		{
			found=1;
		}
		else
		{
			found=0;
		}
	}
	else if (found)
	{
		int orange_count=mask_orange(image,true,false);
		//int left_or_right=guess_line(image);//Left is negative, right is positive, magnitude is num pixels from center line
		
		if (orange_count<250)
			found=0;
	}
	
	int linex,liney;
	if (found)
	{
		angle=hough(image,&linex,&liney);
		if (angle==HOUGH_ERROR)
		{
			lineX=-1;
			lineY=-1;
		}
		else
		{
			//lineX and lineY are fields, both are doubles
			lineX=linex;
			lineX/=image->width;
			lineY=liney;
			lineY/=image->height;
			cout<<"(x,y):"<<"("<<lineX<<","<<lineY<<")"<<endl;
		}
	}

        if (output)
        {
            OpenCVImage temp(image, false);
            output->copyFrom(&temp);
        }
}

} // namespace vision
} // namespace ram
