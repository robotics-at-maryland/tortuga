#include "vision/include/Calibration.h"

using namespace std;
using namespace ram::vision; 

Calibration::Calibration(OpenCVCamera* camera)
{
	cam=camera;
	calibrated=false;
	frame = new ram::vision::OpenCVImage(cam->width(),cam->height());
	cvNamedWindow("Calibration");
}

Calibration::~Calibration()
{
	delete frame;
}

void Calibration::calculateCalibrations()
{
	const int NUMIMAGES=10;

	if (calibrated)
		cout<<"Warning: This calibration is already set up.  Are you recalibrating before undistorting each image?"<<endl;
	
	CvPoint2D32f array[36*NUMIMAGES];
	int goodImages=0;
	int arrayIndex=0;
	int cornerCountsArray[NUMIMAGES];
	
	while (goodImages<NUMIMAGES)
	{
		cam->getImage(frame);
		IplImage* image =(IplImage*)(*frame);
		cvShowImage("Calibration",image);
		int cornerCount=findCorners(image,&array[arrayIndex]);

		if (cornerCount>=25)
		{
			cornerCountsArray[goodImages]=cornerCount;
			arrayIndex+=cornerCount;
			goodImages++;
			cout<<goodImages<<endl;
		}
	}
	
	//Multiply all these by number of images
	CvPoint3D32f buffer[36*NUMIMAGES];
	for (int x=0; x<arrayIndex;x++)
	{
		buffer[x].x=array[x].x;
		buffer[x].y=array[x].y;
		buffer[x].z=0;
	}
	calibrateCamera(cam->width(), cam->height(), cornerCountsArray, distortion,cameraMatrix,transVects,rotMat,NUMIMAGES,array,buffer);
	calibrated=true;
	cout<<"Calibration Complete"<<endl;
}

void Calibration::calibrateImage(IplImage* src, IplImage* dest)
{
	undistort(src, dest, cameraMatrix, distortion);
}

void Calibration::printCalibrations()
{
	int i;
	int j;
	if (!calibrated)
		calculateCalibrations();
	
	cout<<"Distortion Coefficients are:"<<endl;
	for (i=0; i<4; i++)
		cout<<distortion[i]<<endl;
		
	cout<<"The camera matrix is:"<<endl<<endl;
	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
			cout<<cameraMatrix[i*3+j]<<"  ";
		cout<<endl;
	}
	cout<<endl<<"The rotation vector is: "<<endl<<endl;
	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
		  cout<<rotMat[i*3+j]<<"  ";
		cout<<endl;
	}
	cout<<endl<<"The translation vector is: "<<endl<<endl;
	for (i=0; i<3; i++)
		cout<<transVects[i]<<"  ";
	cout<<endl;
}

void Calibration::setCalibrationManual(float* distortion2, float* cameraMatrix2, float* rotMat2, float* transVects2)
{
	if (calibrated)
		cout<<"Warning: This has already been calibrated.  Are you purposefully manually recalibrating?"<<endl;
	int i;
	for (i=0; i<4; i++)
		distortion[i]=distortion2[i];
	for (i=0; i<9; i++)
		cameraMatrix[i]=cameraMatrix2[i];
	for (i=0; i<9; i++)
		rotMat[i]=rotMat2[i];
	for (i=0; i<3;i++)
		transVects[i]=transVects2[i];
		
	calibrated=true;
}
