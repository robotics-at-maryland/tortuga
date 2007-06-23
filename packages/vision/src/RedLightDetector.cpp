#include "vision/include/RedLightDetector.h"

using namespace std;
using namespace ram::vision;

RedLightDetector::RedLightDetector(OpenCVCamera* camera)
{
	cam = camera;
    frame = new ram::vision::OpenCVImage(640, 480);
	found=false;
	lightFramesOff=0;
	lightFramesOn=0;
	blinks=0;
	spooky=0;
	startCounting=false;
}

RedLightDetector::~RedLightDetector()
{
	delete frame;
}

void RedLightDetector::update()
{
	cam->waitForImage(frame);
	IplImage* image =(IplImage*)(*frame);

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
		//cout<<"This thing has blinked "<<blinks<<" times, WE FOUND THE LIGHT GUYS!!"<<endl;
	}
	else
	{

	}
	cvCopyImage(frame, flashFrame);
	CvPoint p=find_flash(flashFrame, show_flashing);
	if (p.x!=0 && p.y!=0)
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
//		if (lightCenter.x!=0 && lightCenter.y!=0)
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
}