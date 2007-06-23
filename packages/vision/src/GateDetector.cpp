#include "vision/include/GateDetector.h"

using namespace std;
using namespace ram::vision;

GateDetector::GateDetector(OpenCVCamera* camera)
{
	cam = camera;
	frame = new ram::vision::OpenCVImage(640, 480);
	gateX=0;
	gateY=0;
	found=false;
}

GateDetector::~GateDetector()
{
	delete frame;
}

void GateDetector::update()
{	
	cam->waitForImage(frame);
	IplImage* image =(IplImage*)(*frame);
	IplImage* gateFrame =cvCreateImage(cvGetSize(frame),8,3);
	cvCopyImage(image,gateFrame);

	toRatios(image);
	found=gateDetect(image,gateFrame,&gateX,&gateY);
}
