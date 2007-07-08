/*Daniel Hakim*/
/*5:06 AM*/

#include <highgui.h>

#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/GateDetector.h"
#include "vision/include/RedLightDetector.h"
#include "vision/include/BinDetector.h"
#include "vision/include/Calibration.h"
#include <iostream>

using namespace std;
using namespace ram::vision;
#define SHOW_OUTPUT 1

int main(int argc, char** argv)
{
	Calibration* cal;
	OpenCVCamera* camera=NULL;
	if (argc==2)
		camera=new OpenCVCamera(argv[1]);
	else
	{
		camera = new OpenCVCamera();
		if (!camera)
		{
			cout<<"No Camera"<<endl;
			return -1;
		}
	}
	OpenCVImage* frame = new OpenCVImage(640,480);
	camera->background(); //Silliness
	cal=new Calibration(camera);
	cvWaitKey(0);

	//cal->setCalibrationGarbage();
	cal->printCalibrations();
	//	OrangePipeDetector* opDetect=new OrangePipeDetector(camera);
	//	GateDetector* gDetect=new GateDetector(camera);
	//	BinDetector* bDetect=new BinDetector(camera);
	//      RedLightDetector* rlDetect=new RedLightDetector(camera);
	char key=' ';
	cvNamedWindow("Detector Test", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Undistorted");
	IplImage* rotated=cvCreateImage(cvSize(480,640),8,3);
	IplImage* undistorted=cvCreateImage(cvSize(640,480),8,3);
	IplImage* differenceImage=cvCreateImage(cvSize(640,480),8,3);
	while (key!='q')
	{
		camera->getImage(frame);
		IplImage* image =(IplImage*)(*frame);
		cvShowImage("Detector Test", image);

		//rotate90Deg(image,rotated);
		//Distortion Correction
		cout<<"Attempting to undistort"<<endl;
		cal->calibrateImage(image, undistorted);
		cout<<"Finished correcting distortion"<<endl;
		if (undistorted->width==640 && undistorted->height==480)
		  cvShowImage("Undistorted",undistorted);
		key=cvWaitKey(0);
		//Orange pipeline
//		if (SHOW_OUTPUT)
//			cout<<"Running Orange Pipeline Detection..."<<endl;
//
//		opDetect->update();
//		if (opDetect->found && opDetect->getAngle()!=-10)
//		{
//			//Found in this case refers to the finding of a thresholded number of pixels
//			//Angle will be equal to -10 if those pixels do not form pipeline shaped objects
//			//center will be -1 -1 whenever angle is -10
//			if (SHOW_OUTPUT)
//			{
//				cout<<"Orange Pipeline Found!"<<endl;
//				cout<<"Useful Data:"<<endl;
//				cout<<"Angle:"<<opDetect->getAngle()<<endl;
//				cout<<"Center(X,Y):"<<"("<<opDetect->getX()<<","<<opDetect->getY()<<")"<<endl<<endl;
//			}
//		}
//		else
//		{
//			if (SHOW_OUTPUT)
//			{
//				cout<<"No Orange Pipeline Found."<<endl<<endl;
//			}
//		}
//		
//		//Gate Detection
//		if (SHOW_OUTPUT)
//			cout<<"Running Gate Detection..."<<endl;
//		
//		gDetect->update();
//		if (gDetect->found)
//		{
//			if (SHOW_OUTPUT)
//			{
//				cout<<"Gate Found!"<<endl;
//				cout<<"Useful Data:"<<endl;
//				cout<<"Center(X,Y):"<<"("<<gDetect->getX()<<","<<gDetect->getY()<<")"<<endl<<endl;
//			}
//		}
//		else
//		{
//			if (SHOW_OUTPUT)
//			{
//				cout<<"No Gate Found."<<endl;
//			}
//		}
//		
//		//Bin Detection
//		if (SHOW_OUTPUT)
//			cout<<"Running Bin Detection..."<<endl;
//		bDetect->update();
//		if (bDetect->found)
//		{
//			if (SHOW_OUTPUT)
//			{
//				cout<<"Bin Found!"<<endl;
//				cout<<"Useful Data:"<<endl;
//				cout<<"Center(X,Y):"<<"("<<bDetect->getX()<<","<<bDetect->getY()<<")"<<endl<<endl;
//			}
//		}
//		else
//		{
//			if (SHOW_OUTPUT)
//			{
//				cout<<"No Bin Found."<<endl;
//			}
//		}
//		
//		//Red Light Detection
//		if (SHOW_OUTPUT)
//			cout<<"Running Red Light Detection..."<<endl;
//		rlDetect->update();
//		if (rlDetect->found)
//		{
//			if (SHOW_OUTPUT)
//			{
//				cout<<"Red Light Found!"<<endl;
//				cout<<"Useful Data:"<<endl;
//				cout<<"Center(X,Y):"<<"("<<rlDetect->getX()<<","<<rlDetect->getY()<<")"<<endl<<endl;
//			}
//		}
//		else
//		{
//			if (SHOW_OUTPUT)
//			{
//				cout<<"No Red Light Found."<<endl;
//			}
//		}
	}
	delete camera;
	delete frame;
    return 0;
} 
