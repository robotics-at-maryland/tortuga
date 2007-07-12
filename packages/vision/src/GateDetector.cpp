#include "vision/include/GateDetector.h"

using namespace std;
using namespace ram::vision;

GateDetector::GateDetector(OpenCVCamera* camera)
{
	cam = camera;
	frame = new ram::vision::OpenCVImage(640,480);
	gateX=0;
	gateY=0;
	found=false;
	//This frame will be a copy of the original rotated 90¼ counterclockwise.
	gateFrame =cvCreateImage(cvSize(480,640),8,3);
	gateFrameRatios = cvCreateImage(cvSize(480,640),8,3);
	cvNamedWindow("Gate");
}

GateDetector::~GateDetector()
{
	delete frame;
	cvReleaseImage(&gateFrame);
	cvReleaseImage(&gateFrameRatios);
}

double GateDetector::getX()
{
	return gateXNorm;
}

double GateDetector::getY()
{
	return gateYNorm;
}

void GateDetector::update()
{	
	cam->getImage(frame);
	IplImage* image =(IplImage*)(*frame);
	rotate90Deg(image,gateFrame);//Rotate image into gateFrame, so that it will be vertical.
	rotate90Deg(image,gateFrameRatios);
	to_ratios(gateFrameRatios);
	found=gateDetect(gateFrameRatios,gateFrame,&gateX,&gateY);
	cvShowImage("Gate",gateFrame);
	gateXNorm=gateX;
	gateYNorm=gateY;
	gateXNorm/=image->width;
	gateXNorm/=image->height;		
}
