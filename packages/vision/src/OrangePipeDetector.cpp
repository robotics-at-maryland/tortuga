/*
 *  OrangePipeDetector.cpp
 *  
 *
 *  Created by Daniel Hakim on 6/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "vision/include/OrangePipeDetector.h"

using namespace std;
using namespace ram::vision;

OrangePipeDetector::OrangePipeDetector(OpenCVCamera* camera)
{
	found=0;
	angle=0;
	cam = camera;
    frame = new ram::vision::OpenCVImage(640, 480);
}

OrangePipeDetector::~OrangePipeDetector()
{
	delete frame;
}

void OrangePipeDetector::update()
{
	//Plan is:  Search out orange with a strict orange filter, as soon as we see a good deal of orange
	// use a less strict filter, and reduce the amount we need to see.
	//In theory this makes us follow the pipeline as long as possible, but be reluctant to follow some
	//arbitrary blob until we know its of a large size and the right shade of orange.  
	//If the pipeline is found, the angle found by hough is reported.  
	
	//Mask orange takes frame, then alter image, then strictness (true=more strict, false=more lenient)
	cam->getImage(frame);
	IplImage* image =(IplImage*)(*frame);
	if (!found)
	{
		int orange_count=mask_orange(image,true,true);
		if (orange_count>1000)
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
	
	if (found)
		angle=hough(image);
}