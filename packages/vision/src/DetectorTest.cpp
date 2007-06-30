/*Daniel Hakim*/
/*5:06 AM*/

#include <highgui.h>

#include "vision/include/OpenCVCamera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/GateDetector.h"
#include "vision/include/RedLightDetector.h"
#include "vision/include/BinDetector.h"
#include <iostream>

using namespace std;
using namespace ram::vision;
int main()
{
	OpenCVCamera* camera = new OpenCVCamera("out.avi");
	OpenCVImage* frame = new OpenCVImage(640,480);
	camera->background(); //Silliness
	OrangePipeDetector* opDetect=new OrangePipeDetector(camera);
	GateDetector* gDetect=new GateDetector(camera);
	BinDetector* bDetect=new BinDetector(camera);
	RedLightDetector* rlDetect=new RedLightDetector(camera);
	char key=' ';
	cvNamedWindow("Detector Test");
	while (key!='q')
	{
		camera->getImage(frame);
		IplImage* image =(IplImage*)(*frame);
		cvShowImage("Detector Test",image);
		key=cvWaitKey(25);
//		cout<<"Running Orange Pipeline Detection..."<<endl;
//		opDetect->update();
//		if (opDetect->found)
//			cout<<"Orange Pipeline Found!"<<endl;
//		else
//			cout<<"No Orange Pipeline Found."<<endl;
//
//		cout<<"Running Gate Detection..."<<endl;
//		gDetect->update();
//		if (gDetect->found)
//			cout<<"Gate Found!"<<endl;
//		else
//			cout<<"No Gate Found."<<endl;
//	
//		cout<<"Running Bin Detection..."<<endl;
//		bDetect->update();
//		if (bDetect->found)
//			cout<<"Bin Found!"<<endl;
//		else
//			cout<<"No Bin Found."<<endl;

		cout<<"Running Red Light Detection..."<<endl;
		rlDetect->update();
		if (rlDetect->found)
			cout<<"Red Light Found!"<<endl;
		else
			cout<<"No Red Light Found."<<endl;
	}
	
	delete camera;
	delete frame;
    return 0;
} 
