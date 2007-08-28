#include "vision/include/RedLightDetector.h"

using namespace std;
using namespace ram::vision;

RedLightDetector::RedLightDetector(OpenCVCamera* camera)
{
	show_output=false;
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

	if (show_output)
	{
		cvNamedWindow("Flash",CV_WINDOW_AUTOSIZE);
		cvNamedWindow("raw",CV_WINDOW_AUTOSIZE);
		cvNamedWindow("LightFinder",CV_WINDOW_AUTOSIZE);
		cvNamedWindow("masked",CV_WINDOW_AUTOSIZE);
	}
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

void RedLightDetector::update()
{
	cam->getImage(frame);
	IplImage* sideways =(IplImage*)(*frame);
	rotate90Deg(sideways,image);
	cvCopyImage(image,raw);//Now both are rotated 90 degrees
	if (show_output)
		cvShowImage("raw", raw);
	cvCopyImage(image, flashFrame);
	
	to_ratios(image);
	CvPoint p;
	redMask(image,flashFrame);
	if (show_output)
		cvShowImage("masked",flashFrame);

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
	if (show_output)
		cvShowImage("LightFinder", raw);
}
