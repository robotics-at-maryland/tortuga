/*Daniel Hakim*/
/*5:06 AM*/

#include <highgui.h>

#include "vision/include/DetectorTest.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/GateDetector.h"
#include "vision/include/RedLightDetector.h"
#include "vision/include/BinDetector.h"
#include "vision/include/Calibration.h"
#include "vision/include/Recorder.h"
#include "vision/include/VisionDemo.h"
#include <string>
#include <iostream>

using namespace std;
using namespace ram::vision;
#define SHOW_OUTPUT 0
#define DEMO 1
//forward is 1 for forwardcamera, 0 for downward camera
DetectorTest::DetectorTest(int camNum, bool forward)
{
	cout<<"I am compiled"<<endl;

	orangeOn=false;
	lightOn=false;
	gateOn=false;
	binOn=false;
	camera = new OpenCVCamera(camNum, forward);
	opDetect=new OrangePipeDetector(camera);
	gDetect=new GateDetector(camera);
	bDetect=new BinDetector(camera);
	rlDetect=new RedLightDetector(camera);
	if (forward)
		recorder=new Recorder(camera, "forward.avi");
	else
		recorder=new Recorder(camera, "downward.avi");
	undistorted=cvCreateImage(cvSize(640,480),8,3);
    differenceImage=cvCreateImage(cvSize(640,480),8,3);
	cout<<"Detector test being created, forward:"<<forward;
	if (SHOW_OUTPUT)
	{
		cvNamedWindow("Detector Test");
		cvNamedWindow("Undistorted");
		cvNamedWindow("Calibration");
	}
	if (!camera)
	{
		cout<<"No Camera"<<endl;
		return;
	}
	frame = new OpenCVImage(640,480);
	dest=cvCreateImage(cvSize(480,640),8,3);
	dataMove=cvCreateImage(cvSize(256,256),8,3);
	camera->background(30); //Silliness	
}

DetectorTest::DetectorTest(string movie)
{
	orangeOn=false;
	lightOn=false;
	gateOn=false;
	binOn=false;
	camera = new OpenCVCamera(movie);
	frame = new OpenCVImage(640,480);
	dest=cvCreateImage(cvSize(480,640),8,3);
	camera->background();
	recorder=NULL;
	opDetect=new OrangePipeDetector(camera);
	gDetect=new GateDetector(camera);
	bDetect=new BinDetector(camera);
	rlDetect=new RedLightDetector(camera);
	
	if (SHOW_OUTPUT)
	{
		cvNamedWindow("Detector Test");
		cvNamedWindow("Undistorted");
		cvNamedWindow("Calibration");
	}
	
	if (!camera)
	{
		cout<<"No Movie"<<endl;
		return;
	}
	dataMove=cvCreateImage(cvSize(256,256),8,3);
}

DetectorTest::~DetectorTest()
{
	delete camera;
	delete frame;
	delete opDetect;
	delete gDetect;
	delete bDetect;
	delete rlDetect;
	delete recorder;
	unbackground(true);
}

void DetectorTest::orangeDetectOn()
{
	cout<<"Turning On Orange Detection"<<endl;
	orangeOn=true;
}
void DetectorTest::lightDetectOn()
{
	cout<<"Turning On Light Detection"<<endl;
	lightOn=true;
}
void DetectorTest::gateDetectOn()
{
	cout<<"Turning on Gate Detection"<<endl;
	gateOn=true;
}
void DetectorTest::binDetectOn()
{
	cout<<"Turning on Bin Detection"<<endl;
	binOn=true;
}
void DetectorTest::orangeDetectOff()
{
	orangeOn=false;
}
void DetectorTest::lightDetectOff()
{
	lightOn=false;
}
void DetectorTest::gateDetectOff()
{
	gateOn=false;
}
void DetectorTest::binDetectOff()
{
	binOn=false;
}
OrangePipeDetector* DetectorTest::getOrangeDetector()
{
	return opDetect;
}
GateDetector* DetectorTest::getGateDetector()
{
	return gDetect;
}
BinDetector* DetectorTest::getBinDetector()
{
	return bDetect;
}
RedLightDetector* DetectorTest::getRedDetector()
{
	return rlDetect;
}

void DetectorTest::update(double timestep)
{
	cout<<"I am compiled"<<endl;
	cout<<orangeOn<<" "<<binOn<<" "<<lightOn<<" "<<gateOn<<endl;
	//char key=' ';
	cout<<frame->getWidth()<<" "<<frame->getHeight()<<" ";
	camera->getImage(frame);
	IplImage* image=(IplImage*)(*frame);
	if (SHOW_OUTPUT)
		cvShowImage("Detector Test", image);

	//Recording
	if (recorder!=NULL)
	{
		recorder->update();
	}
	//Orange pipeline	
	if (orangeOn)
	{
		if (SHOW_OUTPUT)
			cout<<"Running Orange Pipeline Detection..."<<endl;
			
		opDetect->update();
		if (opDetect->found && opDetect->getAngle()!=-10)
		{
			//Found in this case refers to the finding of a thresholded number of pixels
			//Angle will be equal to -10 if those pixels do not form pipeline shaped objects
			//center will be -1 -1 whenever angle is -10
			if (SHOW_OUTPUT)
			{
				cout<<"Orange Pipeline Found!"<<endl;
				cout<<"Useful Data:"<<endl;
				cout<<"Angle:"<<opDetect->getAngle()<<endl;
				cout<<"Center(X,Y):"<<"("<<opDetect->getX()<<","<<opDetect->getY()<<")"<<endl<<endl;
			}
		}
		else
		{
			if (SHOW_OUTPUT)
			{
				cout<<"No Orange Pipeline Found."<<endl<<endl;
			}
		}
	}
	
	//Gate Detection
	if (gateOn)
	{
		if (SHOW_OUTPUT)
			cout<<"Running Gate Detection..."<<endl;
		
		gDetect->update();
		if (gDetect->found)
		{
			if (SHOW_OUTPUT)
			{
				cout<<"Gate Found!"<<endl;
				cout<<"Useful Data:"<<endl;
				cout<<"Center(X,Y):"<<"("<<gDetect->getX()<<","<<gDetect->getY()<<")"<<endl<<endl;
			}
		}
		else
		{
			if (SHOW_OUTPUT)
			{
				cout<<"No Gate Found."<<endl;
			}
		}
	}
	
	//Bin Detection
	if (binOn)
	{
		if (SHOW_OUTPUT)
			cout<<"Running Bin Detection..."<<endl;
		bDetect->update();
		if (bDetect->found)
		{
			if (SHOW_OUTPUT)
			{
				cout<<"Bin Found!"<<endl;
				cout<<"Useful Data:"<<endl;
				cout<<"Center(X,Y):"<<"("<<bDetect->getX()<<","<<bDetect->getY()<<")"<<endl<<endl;
			}
		}
		else
		{
			if (SHOW_OUTPUT)
			{
				cout<<"No Bin Found."<<endl;
			}
		}
	}
	//Red Light Detection
	if (lightOn)
	{
		if (SHOW_OUTPUT)
			cout<<"Running Red Light Detection..."<<endl;
		rlDetect->update();
		if (rlDetect->found)
		{
			if (SHOW_OUTPUT)
			{
				cout<<"Red Light Found!"<<endl;
				cout<<"Useful Data:"<<endl;
				cout<<"Center(X,Y):"<<"("<<rlDetect->getX()<<","<<rlDetect->getY()<<")"<<endl<<endl;
			}
		}
		else
		{
			if (SHOW_OUTPUT)
			{
				cout<<"No Red Light Found."<<endl;
			}
		}
	}
	
	if (DEMO)
	{
		IplImage* ptr=NULL;
		if (orangeOn)
		{
			ptr=opDetect->getAnalyzedImage();
		}
		else if (gateOn)
		{
			ptr=gDetect->getAnalyzedImage();
		}
		else if (binOn)
		{
			ptr=bDetect->getAnalyzedImage();
		}
		else if (lightOn)
		{
			ptr=rlDetect->getAnalyzedImage();
		}
		else
			ptr=image;
		cout<<"Sending to dataCopy"<<endl;
		cvResize(ptr,dataMove);
		dataCopy((unsigned char*)(dataMove->imageData),dataMove->width,dataMove->height);
		cout<<"Done sending to dataCopy"<<endl;
	}
	return;
} 
