/*Daniel Hakim*/
/*5:06 AM*/

#include <highgui.h>

#include "core/include/EventPublisher.h"
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
#include "core/include/Event.h"
#include <string>
#include <iostream>

using namespace std;
using namespace ram::vision;
#define SHOW_OUTPUT 1
#define DEMO 0
//forward is 1 for forwardcamera, 0 for downward camera
DetectorTest::DetectorTest(int camNum, bool forward)
{
	fromMovie=false;
	orangeOn=false;
	lightOn=false;
	gateOn=false;
	binOn=false;
	camera = new OpenCVCamera(camNum, forward);
	opDetect=new OrangePipeDetector(camera);
	gDetect=new GateDetector(camera);
	bDetect=new BinDetector(camera);
	rlDetect=new RedLightDetector(camera);
	fDetect=new FeatureDetector(camera);
	
	recorder=NULL;
	if (forward)
		recorder=new Recorder(camera, "forward.avi");
	else
		recorder=new Recorder(camera, "downward.avi");
	undistorted=cvCreateImage(cvSize(640,480),8,3);
    differenceImage=cvCreateImage(cvSize(640,480),8,3);
//	cout<<"Detector test being created, forward:"<<forward;
	if (SHOW_OUTPUT)
	{
		cvNamedWindow("Detector Test");
	}
	if (!camera)
	{
		cout<<"No Camera"<<endl;
		return;
	}
	frame = new OpenCVImage(640,480);
	dest=cvCreateImage(cvSize(480,640),8,3);
	dataMove=cvCreateImage(cvSize(256,256),8,3);
	camera->background(30);//Camera is backgrounded when dealing with a camera such that each detector when it queries the camera, deals only with the latest image.
	//I really think that each detector should simply ask the camera for the latest image when it starts... but instead we are taking every image from the camera, 
	//spending the time to copy it, and then dumping most of them.  If more than this class were dealing with the camera then maybe this is correct, as something could record
	//the run, but this still seems wrong when this is the only class dealing with the camera.  Honestly if the camera should be backgrounded, it shouldn't be backgrounded here.
	//Unfortunately, if we don't do it here, and no one else does it either, then this class has to call update on the camera before getting new frames to analyze,
	//And then if someone else backgrounds the camera, it all screws up...  yay threads.
}

DetectorTest::DetectorTest(string movie)
{
	fromMovie=true;
	orangeOn=false;
	lightOn=false;
	gateOn=false;
	binOn=false;
	camera = new OpenCVCamera(movie);
	frame = new OpenCVImage(640,480);
	dest=cvCreateImage(cvSize(480,640),8,3);
	recorder=NULL;
	recorder=new Recorder(camera, "analysis.avi");
	opDetect=new OrangePipeDetector(camera);
	gDetect=new GateDetector(camera);
	bDetect=new BinDetector(camera);
	rlDetect=new RedLightDetector(camera);
	fDetect=new FeatureDetector(camera);
	
	if (SHOW_OUTPUT)
	{
		cvNamedWindow("Detector Test");
	}
	
	if (!camera)
	{
		cout<<"No Movie"<<endl;
		return;
	}
	dataMove=cvCreateImage(cvSize(256,256),8,3);
	//camera->background();//There is NO valid reason to background the camera when dealing with a movie, we will just skip frames!
	//Instead we should call camera->update directly at the start of the update call.  
}

DetectorTest::~DetectorTest()
{
    unbackground(true);
	delete camera;
	delete frame;
	delete opDetect;
	delete gDetect;
	delete bDetect;
	delete rlDetect;
	delete recorder;
}

void DetectorTest::orangeDetectOn()
{
//	cout<<"Turning On Orange Detection"<<endl;
	lightOn=false;
	gateOn=false;
	binOn=false;
	orangeOn=true;
	featureOn=false;
	//Turn off others
}
void DetectorTest::lightDetectOn()
{
//	cout<<"Turning On Light Detection"<<endl;
	gateOn=false;
	binOn=false;
	orangeOn=false;
	lightOn=true;
	featureOn=false;
}
void DetectorTest::gateDetectOn()
{
//	cout<<"Turning on Gate Detection"<<endl;
	lightOn=false;
	binOn=false;
	orangeOn=false;
	gateOn=true;
	featureOn=false;
}
void DetectorTest::binDetectOn()
{
//	cout<<"Turning on Bin Detection"<<endl;
	lightOn=false;
	gateOn=false;
	orangeOn=false;
	binOn=true;
	featureOn=false;
}
void DetectorTest::featureDetectOn()
{
//	cout<<"Turning on feature detection"<<endl;
	lightOn=false;
	gateOn=false;
	orangeOn=false;
	binOn=false;
	featureOn=true;
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
void DetectorTest::featureDetectOff()
{
	featureOn=false;
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
//	cout<<"I am compiled"<<endl;
//	cout<<orangeOn<<" "<<binOn<<" "<<lightOn<<" "<<gateOn<<endl;
	//char key=' ';
//	cout<<frame->getWidth()<<" "<<frame->getHeight()<<" ";
	
	if (fromMovie)
	{
//		printf("Acquiring Next Frame\n");
		camera->update(timestep);//When running on a movie, the camera is not constantly grabbing frames, analyze each frame by itself.
	}
	camera->getImage(frame);
	IplImage* image=(IplImage*)(*frame);
	if (!orangeOn && !lightOn && !gateOn && !binOn)
		cvShowImage("Detector Test", image);

	//Recording
	//If we're on a camera, just record a movie of it for later analysis.
	//If we're on a movie, record the analyzed results of the movie.
	if (!fromMovie)
	{
		if (recorder!=NULL)
		{
			recorder->update();//We could just background recorder itself, but then we get a movie where frames recorded don't match up with frames analyzed.
		}
	}
	
	//Orange pipeline	
	if (orangeOn)
	{
		if (SHOW_OUTPUT)
			cout<<"Running Orange Pipeline Detection..."<<endl;
			
		opDetect->update();
		opDetect->show("Detector Test");
		if (fromMovie && recorder!=NULL)
			recorder->writeFrame(opDetect->getAnalyzedImage());
		if (opDetect->found && opDetect->getAngle()!=-10)
		{
			ram::core::EventPtr e(new ram::vision::PipeEvent(2*opDetect->getX()-1,1-2*opDetect->getY(),opDetect->getAngle()));
			publish(ram::vision::EventType::PIPE_FOUND,e);
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
		gDetect->show("Detector Test");
		if (fromMovie && recorder!=NULL)
			recorder->writeFrame(gDetect->getAnalyzedImage());

		if (gDetect->found)
		{
			ram::core::EventPtr e(new ram::core::Event());
			ram::core::EventPublisher::publish(ram::vision::EventType::GATE_FOUND,e);

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
		bDetect->show("Detector Test");
		if (fromMovie && recorder!=NULL)
			recorder->writeFrame(bDetect->getAnalyzedImage());

		if (bDetect->found)
		{
			ram::core::EventPtr e(new ram::core::Event());
			publish(ram::vision::EventType::BIN_FOUND,e);

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
//		if (SHOW_OUTPUT)
//			cout<<"Running Red Light Detection..."<<endl;
		rlDetect->update();
		rlDetect->show("Detector Test");
		if (fromMovie && recorder!=NULL)
			recorder->writeFrame(rlDetect->getAnalyzedImage());

		if (rlDetect->found)
		{
			ram::core::EventPtr e(new ram::vision::RedLightEvent((2*rlDetect->getX())-1,1-(2*rlDetect->getY())));
			publish(ram::vision::EventType::LIGHT_FOUND,e);

			if (SHOW_OUTPUT)
			{
//				cout<<"Red Light Found!"<<endl;
//				cout<<"Useful Data:"<<endl;
//				cout<<"Center(X,Y):"<<"("<<rlDetect->getX()<<","<<rlDetect->getY()<<")"<<endl<<endl;
			}
		}
		else
		{
//			if (SHOW_OUTPUT)
//			{
//				cout<<"No Red Light Found."<<endl;
//			}
		}
	}
	
	if (featureOn)
	{
		if (SHOW_OUTPUT)
			cout<<"Running feature detector..."<<endl;
		fDetect->update();
		if (SHOW_OUTPUT)
			fDetect->show("Detector Test");
		if (fromMovie && recorder!=NULL)
			recorder->writeFrame(fDetect->getAnalyzedImage());
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
