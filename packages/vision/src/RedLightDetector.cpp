#include "vision/include/RedLightDetector.h"

using namespace std;
using namespace ram::vision;

RedLightDetector::RedLightDetector(OpenCVCamera* camera)
{
	cam = camera;
    frame = new ram::vision::OpenCVImage(640,480);
	found=false;
	lightFramesOff=0;
	lightFramesOn=0;
	blinks=0;
	spooky=0;
	startCounting=false;
	lightCenter.x=0;
	lightCenter.y=0;
	image=cvCreateImage(cvSize(480,640),8,3);
	raw=cvCreateImage(cvGetSize(image),8,3);
	flashFrame=cvCreateImage(cvGetSize(image), 8, 3);
}

RedLightDetector::~RedLightDetector()
{
	delete frame;
	cvReleaseImage(&flashFrame);
	cvReleaseImage(&image);
	cvReleaseImage(&raw);
}

double RedLightDetector::getX()
{
	return redLightCenterX;
}

double RedLightDetector::getY()
{
	return redLightCenterY;
}

void RedLightDetector::show(char* window)
{
	//Chris:  If you want to see an image other than the raw image with a box drawn if light found
	//You need to change the IplImage* being drawn here.  To see the effects of the red mask you want to change this line to flashFrame.
	//To see the mask calibrations being used for the red mask, you need to open up main.cpp  and find redMask.
	cvShowImage(window,((IplImage*)(raw)));
}


IplImage* RedLightDetector::getAnalyzedImage()
{
	return (IplImage*)(raw);
}


void RedLightDetector::update()
{
	cam->getImage(frame);
	IplImage* sideways =(IplImage*)(*frame);
	rotate90Deg(sideways,image);
	cvCopyImage(image,raw);//Now both are rotated 90 degrees
	cvCopyImage(image, flashFrame);
	
	to_ratios(image);
	CvPoint p;
	redMask(image,flashFrame);

	int redPixelCount=histogram(flashFrame,&p.x,&p.y);

	if (redPixelCount<75)
	{
		p.x=p.y=-1;
		found=false; //Completely ignoring the state machine for the time being.
	}	
	else
	{
		found=true; //completely ignoring the state machine for the time being.
		cout<<"FOUND RED LIGHT"<<endl;
		CvPoint tl,tr,bl,br;
		tl.x=bl.x=max(p.x-4,0);
		tr.x=br.x=min(p.x+4,raw->width-1);
		tl.y=tr.y=min(p.y+4,raw->height-1);
		br.y=bl.y=max(p.y-4,0);
		
		cvLine(raw, tl, tr, CV_RGB(0,0,255), 3, CV_AA, 0 );
		cvLine(raw, tl, bl, CV_RGB(0,0,255), 3, CV_AA, 0 );
		cvLine(raw, tr, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
		cvLine(raw, bl, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
	}

	redLightCenterX=lightCenter.x;
	redLightCenterY=lightCenter.y;
	redLightCenterX/=image->width;
	redLightCenterY/=image->height;
}
