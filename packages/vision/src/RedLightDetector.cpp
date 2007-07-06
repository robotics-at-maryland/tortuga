#include "vision/include/RedLightDetector.h"

using namespace std;
using namespace ram::vision;

RedLightDetector::RedLightDetector(OpenCVCamera* camera)
{
	cam = camera;
    frame = new ram::vision::OpenCVImage(cam->width(),cam->height());
	found=false;
	lightFramesOff=0;
	lightFramesOn=0;
	blinks=0;
	spooky=0;
	startCounting=false;
	lightCenter.x=0;
	lightCenter.y=0;
	image=cvCreateImage(cvSize(cam->height(),cam->width()),8,3);
	raw=cvCreateImage(cvGetSize(image),8,3);
	
	cvNamedWindow("Flash",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("raw",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("LightFinder",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("masked",CV_WINDOW_AUTOSIZE);
}

RedLightDetector::~RedLightDetector()
{
	delete frame;
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
	IplImage* image=cvCreateImage(cvSize(cvGetSize(sideways).height,cvGetSize(sideways).width),8,3);
	IplImage* raw=cvCreateImage(cvGetSize(image),8,3);
	rotate90Deg(sideways,image);
	cvCopyImage(image,raw);//Now both are rotated 90 degrees
	cvShowImage("raw", raw);
	
	IplImage* flashFrame=cvCreateImage(cvGetSize(image), 8, 3);
	if (lightFramesOff>20)
	{
		//					cout<<"Its been 20 frames without seeing the light-like object, maybe it was just a reflection, im starting the count over"<<endl;
		lightFramesOn=0;
		lightFramesOff=0;
		blinks=0;
		spooky=0;
		startCounting=false;
	}
	if (lightFramesOn>20)
	{
		//					cout<<"Its been 20 frames of seeing the light-like object, its not flashing, guess its just something shiny"<<endl;
		lightFramesOn=0;
		lightFramesOff=0;
		blinks=0;
		spooky=0;
		startCounting=false;
	}
	if (spooky>5)
	{
		//					cout<<"Somethings wrong, its staying off for too short/long, or staying on for too short/long, and its happened a spooky number of times, its not the light."<<endl;
		lightFramesOn=0;
		lightFramesOff=0;
		blinks=0;
		spooky=0;
		startCounting=false;
	}					
	if (blinks>3)
	{
//		found=true;
		//cout<<"This thing has blinked "<<blinks<<" times, WE FOUND THE LIGHT GUYS!!"<<endl;
	}
	else
	{
//		found=false;
	}
	cvCopyImage(image, flashFrame);
	to_ratios(image);
	CvPoint p;
	redMask(image,flashFrame);
	cvShowImage("masked",flashFrame);
	int redPixelCount=histogram(flashFrame,&p.x,&p.y);
	//Draw a box if its big enough for us to consider it a light
	if (redPixelCount<75)
	{
		p.x=p.y=-1;
		found=false; //Completely ignoring the state machine for the time being.
	}	
	else
	{
//		cout<<"p.x:"<<p.x<<"p.y"<<p.y<<endl;
		found=true; //completely ignoring the state machine for the time being.
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
	
	if (p.x!=-1 && p.y!=-1)
	{
		if (lightCenter.x==0 && lightCenter.y==0)
		{
			//	cout<<"I see a light-like object"<<endl;
			startCounting=true;
		}
		else {
			//if (lightCenter.x<p.x)
			//	cout<<"Its moving left"<<endl;
			//if (lightCenter.y<p.y)
			//	cout<<"Its moving up"<<endl;
			//if (lightCenter.x>p.x)
			//	cout<<"Its moving right"<<endl;
			//if (lightCenter.y>p.y)
			//	cout<<"Its moving down"<<endl;
			//if (lightCenter.x==p.x && lightCenter.y==p.y)
			//	cout<<"Its not moving... did someone put a flashing light on the sub or something... or are we stopped... uh oh..."<<endl;
		}
		lightCenter.x=p.x;
		lightCenter.y=p.y;
		if (startCounting)
		{
			if (lightFramesOff>0)
			{
				blinks++;
				//  cout<<"The light has been off for "<<lightFramesOff<<" frames, now its coming back on"<<endl;
			}
			
			if (lightFramesOff<MINFRAMESOFF || lightFramesOff>MAXFRAMESOFF)
			{
				spooky++;
			}
			lightFramesOn++;
			lightFramesOff=0;
		}
	} 
	else
	{
//		if (lightCenter.x!=-1 && lightCenter.y!=-1)
//			cout<<"Light's out"<<endl;			
		lightCenter.x=p.x;
		lightCenter.y=p.y;
		
		if (startCounting)
		{
			if (lightFramesOn>0)
			{
				//cout<<"The light has been on for "<<lightFramesOn<<" frames, now its gone"<<endl;
			}
			
			if (lightFramesOn<MINFRAMESON || lightFramesOn>MAXFRAMESON)
			{
				spooky++;
			}
			
			lightFramesOff++;
			lightFramesOn=0;
		}
		//	paused=true;
	}
	
	
	redLightCenterX=lightCenter.x;
	redLightCenterY=lightCenter.y;
	redLightCenterX/=image->width;
	redLightCenterY/=image->height;
//	cvShowImage("Flash",flashFrame);

	cvShowImage("LightFinder", raw);
}