#include "vision/include/BinDetector.h"

using namespace std;
using namespace ram::vision;

BinDetector::BinDetector(OpenCVCamera* camera)
{
	cam = camera;
	frame = new ram::vision::OpenCVImage(640, 480);
	
	found=0;
	binX=0;
	binY=0;
	binCount=0;
}

BinDetector::~BinDetector()
{
	delete frame;
}

void BinDetector::update()
{
	/*First argument to white_detect is a ratios frame, then a regular one*/
	cam->getImage(frame);
	IplImage* image =(IplImage*)(*frame);

	IplImage* binFrame =cvCreateImage(cvGetSize(image),8,3);
	cvCopyImage(image,binFrame);
	
	to_ratios(image);
	binX=-1;
	binY=-1;
	binCount=white_detect(image,binFrame,&binX,&binY);
	if (binX!=-1 && binY!=-1)
		found=true;
	else
		found=false;
}
