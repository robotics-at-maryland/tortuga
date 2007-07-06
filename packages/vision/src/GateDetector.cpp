#include "vision/include/GateDetector.h"

using namespace std;
using namespace ram::vision;

GateDetector::GateDetector(OpenCVCamera* camera)
{
	cam = camera;
	frame = new ram::vision::OpenCVImage(cam->width(),cam->height());
	gateX=0;
	gateY=0;
	found=false;
	//This frame will be a copy of the original rotated 90¼ counterclockwise.
	gateFrame =cvCreateImage(cvSize(cam->height(),cam->width()),8,3);
}

GateDetector::~GateDetector()
{
	delete frame;
	cvReleaseImage(&gateFrame);
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

	to_ratios(image);
	found=gateDetect(image,gateFrame,&gateX,&gateY);

	gateXNorm=gateX;
	gateYNorm=gateY;
	gateXNorm/=image->width;
	gateXNorm/=image->height;		
}
