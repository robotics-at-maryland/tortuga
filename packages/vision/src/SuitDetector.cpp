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
	IplImage* gray = cvCreateImage(cvGetSize(image), 8, 1);
    CvMemStorage* storage = cvCreateMemStorage(0);
    
	to_ratios(ratioImage);
	redMask(ratioImage, image, 50, 100);
	cvErode(image, image, NULL, 10);

	
	cvCvtColor(image, gray, CV_BGR2GRAY);
    cvSmooth(gray, gray, CV_GAUSSIAN, 9, 9); 
	
    CvSeq* circles = cvHoughCircles(gray, storage, 
        CV_HOUGH_GRADIENT, 2, gray->height/4, 200, 40);
    int i;

    for (i = 0; i < circles->total; i++) 
    {
         float* p = (float*)cvGetSeqElem( circles, i );
         cvCircle( image, cvPoint(cvRound(p[0]),cvRound(p[1])), 
             3, CV_RGB(0,255,0), -1, 8, 0 );
         cvCircle( image, cvPoint(cvRound(p[0]),cvRound(p[1])), 
             cvRound(p[2]), CV_RGB(255,0,0), 3, 8, 0 );
    }

	if (output)
	{
		OpenCVImage temp(image, false);
		output->copyFrom(&temp);
	}
	
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&gray);
	
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
