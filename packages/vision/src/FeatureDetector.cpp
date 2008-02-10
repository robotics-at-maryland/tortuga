/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/FeatureDetector.cpp
 */


// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/FeatureDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"


namespace ram {
namespace vision {

FeatureDetector::FeatureDetector(Camera* camera, int maxFeatures)
{
	cam = camera;
    frame = new OpenCVImage(640,480);
	image=cvCreateImage(cvSize(640,480),8,3);//480 by 640 if we put the camera on sideways again...
	raw=cvCreateImage(cvGetSize(image),8,3);
	eigImage=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
	tempImage=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
	this->maxFeatures=maxFeatures;
	features=(CvPoint2D32f*) calloc(maxFeatures,sizeof(CvPoint2D32f));
	grayscale=cvCreateImage(cvGetSize(image),8,1);
	edgeDetected=cvCreateImage(cvGetSize(image),8,1);
}

FeatureDetector::~FeatureDetector()
{
	delete frame;
	cvReleaseImage(&image);
	cvReleaseImage(&raw);
	cvReleaseImage(&eigImage);
	cvReleaseImage(&tempImage);
	free(features);
}

void FeatureDetector::show(char* window)
{
	cvShowImage(window,((IplImage*)(image)));
}

IplImage* FeatureDetector::getAnalyzedImage()
{
	return (IplImage*)(image);
}

//This should be 3 channel to 1 channel image copying, channel is 0 1 or 2, for the channel to copy.
void FeatureDetector::copyChannel(IplImage* src, IplImage* dest, int channel)
{
	char* data=src->imageData;
	char* data2=dest->imageData;
	int width=image->width;
	int height=image->height;
	int count=0;
	int count2=0;
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			data2[count2]=data[count+channel];
			count+=3;
			count2++;
		}
}

void FeatureDetector::update()
{
	cam->getImage(frame);
	raw=(IplImage*)(*frame);
	cvCopyImage(raw,image);
//	copyChannel(image,grayscale,2);//Lets try just copying the red channel
	cvCvtColor(image,grayscale,CV_BGR2GRAY);
	cvCanny(grayscale,edgeDetected, 50, 100, 3 );
	//The two floats are the minimum quality of features, and minimum euclidean distance between features.
	//The NULL says use the entire image.
	int numFeatures=maxFeatures;
	cvGoodFeaturesToTrack(edgeDetected,eigImage,tempImage,features,&numFeatures,.15,30,NULL);
	
	for (int i=0; i<numFeatures; i++)
	{
		CvPoint topLeft;
		CvPoint bottomRight;
		topLeft.x = (int)features[i].x-2;
		topLeft.y = (int)features[i].y+2;
		bottomRight.x = (int)features[i].x+2;
		bottomRight.y = (int)features[i].y-2;
		cvLine(image,topLeft,bottomRight,CV_RGB(0,0,255),1,CV_AA,0);
		int temp;
		temp=topLeft.y;
		topLeft.y=bottomRight.y;
		bottomRight.y=temp;
		cvLine(image,topLeft,bottomRight,CV_RGB(0,0,255),1,CV_AA,0);
	}
}

} // namespace vision
} // namespace ram
