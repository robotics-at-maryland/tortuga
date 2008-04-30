/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/SuitDetector.cpp
 */

//Library Includes
#include "cv.h"
#include "highgui.h"

//Project Includes
#include "vision/include/main.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/SuitDetector.h"
#include "vision/include/Camera.h"

namespace ram {
namespace vision {

SuitDetector::SuitDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0)
{
    init(config);
}
    
SuitDetector::SuitDetector(Camera* camera) :
    cam(camera)
{
    init(core::ConfigNode::fromString("{}"));
}

void SuitDetector::init(core::ConfigNode)
{
	suit = UNKNOWN;
	suitX = 0;
	suitY = 0;
	analyzedImage = cvCreateImage(cvSize(640,480), 8,3);
	ratioImage = cvCreateImage(cvSize(640,480),8,3);
}
	
SuitDetector::~SuitDetector()
{
	cvReleaseImage(&analyzedImage);
	cvReleaseImage(&ratioImage);
}

void SuitDetector::processImage(Image* input, Image* output)
{
	/*First argument to white_detect is a ratios frame, then a regular one*/
	IplImage* image = (IplImage*)(*input);
	cvCopyImage(image,ratioImage);//Now both are rotated 90 degrees
	
	to_ratios(ratioImage);
	redMask(ratioImage, image, 50, 100);
	//cvErode(image, image, NULL, 10);
	
	if (output)
	{
		OpenCVImage temp(image, false);
		output->copyFrom(&temp);
	}
}

double SuitDetector::getX()
{
	return suitX;
}

double SuitDetector::getY()
{
	return suitY;
}

Suit SuitDetector::getSuit()
{
	return suit;
}

IplImage* SuitDetector::getAnalyzedImage()
{
	return analyzedImage;
}

} // namespace vision
} // namespace ram
