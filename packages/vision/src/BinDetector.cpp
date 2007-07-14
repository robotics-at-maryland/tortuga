#include "vision/include/BinDetector.h"

using namespace std;
using namespace ram::vision;
BinDetector::BinDetector(OpenCVCamera* camera)
{
	show_output=false;
	cam = camera;
	frame = new ram::vision::OpenCVImage(640, 480);
	binFrame =cvCreateImage(cvSize(480,640),8,3);
	rotated = cvCreateImage(cvSize(480,640),8,3);
	found=0;
	binX=-1;
	binY=-1;
	binCount=0;
	if (show_output)
		cvNamedWindow("bin");
}

BinDetector::~BinDetector()
{
	delete frame;
	cvReleaseImage(&binFrame);
	cvReleaseImage(&rotated);
}

void BinDetector::update()
{
	int binx=-1;
	int biny=-1;
	/*First argument to white_detect is a ratios frame, then a regular one*/
	cam->getUncalibratedImage(frame);
	IplImage* image =(IplImage*)(*frame);
	rotate90DegClockwise(image,rotated);
	image=rotated;

	cvCopyImage(image,binFrame);
	
	to_ratios(image);
	binCount=white_detect(image,binFrame,&binx,&biny);
	if (show_output)
	{
		cvShowImage("bin",binFrame);
	}
	if (biny!=-1 && binx!=-1)
	{
		binX=binx;
		binX/=image->width;
		binY=biny;
		binY/=image->height;
		found=true;
	}
	else
	{
		found=false;
		binX=-1;
		binY=-1;
	}
	
}

double BinDetector::getX()
{
	return binX;
}

double BinDetector::getY()
{
	return binY;
}
