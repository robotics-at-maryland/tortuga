/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/SuitDetector.cpp
 */

#include "stdlib.h"

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

int SuitDetector::edgeRun(int startX, int startY, IplImage* img)
{
	int width = img->width;
	int height = img->height;
	unsigned char *data=(unsigned char *)img->imageData;
	int backups = 0;

	int x = startX;
	int y = startY;
	 
	int RIGHT = 3;
	int UP=-width*3;
	int DOWN=width*3;
	int LEFT=-3;
	
	//3 = right 
	//-width*3 = up 
	//-3 = left 
	//width*3 = down
	int dir = RIGHT;
	
	int count = 3*x+3*width*y;
	if (data[count] == 0)
	{
		printf("%s\n", "Invalid start location on edgeRun, must start in a white square");
		return -1;
	}
	
	while(true)
	{
		if (x+1 < width && data[count+3]!=0)
		{
			count+=3;
			data[count+1]=data[count+2]=0;
			x+=1;
		}
		else
		{
			//Catch a corner if there is one here, then break out.
			bool noBottom = false;
			bool noTop = false;
			if (y+1 >= height || data[count+3*width] == 0)
			{
				noTop = true;
			}
			if (y-1 < 0 || data[count-3*width] == 0)
			{
				noBottom = true;
			}
			if (noBottom && noTop)
			{
				backups++;
			}
			break;
		}
	}
	 
	startX = x;
	startY = y; //Should now have traversed to an edge of the shape.
	 
	dir = UP;
	int nextX;
	int nextY;
	while (true)
	{
		int startDir = dir;
		for (int attempt=0; attempt<4; attempt++)
		{
			//First attempt try turn right
			//second attempt try straight
			//third attempt try turn left
			//fourth attempt turn around
			
			if (attempt==0)
			{
				//Turn Right
				if (startDir == UP)
				{
					dir = RIGHT;
				}
				if (startDir == LEFT)
				{
					dir = UP;
				}
				if (startDir == DOWN)
				{
					dir = LEFT;
				}
				if (startDir ==RIGHT)
				{
					dir = DOWN;
				}
			}
			else if (attempt==1)
			{
				//Go straight
				dir = startDir;
			}
			else if (attempt==2)
			{
				if (startDir == UP)
				{
					dir = LEFT;
				}
				if (startDir == LEFT)
				{
					dir = DOWN;
				}
				if (startDir == DOWN)
				{
					dir = RIGHT;
				}
				if (startDir == RIGHT)
				{
					dir = UP;
				}
			}
			else
			{
				//Turn around
				dir = -startDir;
			}
			
			//Now figure out where we'd be heading
			nextX = x;
			nextY = y;
			if (dir == 3)
				nextX++;
			else if (dir == -3)
				nextX--;
			else if (dir > 0)
				nextY++;
			else if (dir < 0)
				nextY--;
			//if we made it, move and break out of the attempt loop
			if (nextX < width && nextX >= 0 && nextY < height && nextY >= 0 && data[count+dir]!=0)
			{
				count+=dir;
				data[count+1]=data[count+2]=0;
				x=nextX;
				y=nextY;
				if (attempt == 3)
					backups++;
				break;
			}
			else
			{
				//Otherwise, just go on to the next attempt.
			}
		}
		if (x == startX && y == startY)
		{
			//Done!
			printf("backups: %d\n",backups);
			return backups;
		}
	}	
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
	
	unsigned int r = (unsigned int)(rand())%image->width;
	unsigned int r2 = (unsigned int)(rand())%image->height;
	int corners = edgeRun(r,r2,image);
	
	if (corners == 0)
		printf("Club\n");
	else if (corners == 1)
		printf("Heart\n");
	else if (corners == 2)
		printf("Spade\n");
	else if (corners == 3)
		printf("Diamond\n");
	
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
