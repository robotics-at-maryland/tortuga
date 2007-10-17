#include "vision/include/BinDetector.h"

using namespace std;
using namespace ram::vision;
BinDetector::BinDetector(OpenCVCamera* camera)
{
	cam = camera;
	frame = new ram::vision::OpenCVImage(640, 480);
	binFrame =cvCreateImage(cvSize(480,640),8,3);
	rotated = cvCreateImage(cvSize(480,640),8,3);
	found=0;
	binX=-1;
	binY=-1;
	binCount=0;
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
	rotate90Deg(image,rotated);
	image=rotated;

	cvCopyImage(image,binFrame);
	
	to_ratios(image);
	binCount=white_detect(image,binFrame,&binx,&biny);
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

void BinDetector::show(char* window)
{
	cvShowImage(window,((IplImage*)(binFrame)));
}

IplImage* BinDetector::getAnalyzedImage()
{
	return (IplImage*)(binFrame);
}

double BinDetector::getX()
{
	return binX;
}

double BinDetector::getY()
{
	return binY;
}
