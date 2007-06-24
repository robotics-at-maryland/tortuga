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
	cam->getImage(frame);
	IplImage* image =(IplImage*)(*frame);
	IplImage* gateFrame =cvCreateImage(cvGetSize(image),8,3);
	cvCopyImage(image,gateFrame);

	to_ratios(image);
	found=gateDetect(image,gateFrame,&gateX,&gateY);
}
