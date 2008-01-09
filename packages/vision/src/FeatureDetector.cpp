#include "vision/include/FeatureDetector.h"

using namespace std;
using namespace ram::vision;

FeatureDetector::FeatureDetector(OpenCVCamera* camera, int maxFeatures)
{
	cam = camera;
    frame = new ram::vision::OpenCVImage(640,480);
	image=cvCreateImage(cvSize(640,480),8,3);//480 by 640 if we put the camera on sideways again...
	raw=cvCreateImage(cvGetSize(image),8,3);
	eigImage=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
	tempImage=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
	this->maxFeatures=maxFeatures;
	features=(CvPoint2D32f*) calloc(maxFeatures,sizeof(CvPoint2D32f));
	grayscale=cvCreateImage(cvGetSize(image),8,1);
	edgeDetected=cvCreateImage(cvGetSize(image),8,1);
}

FeatureDetector::~FeatureDetector()
{
	delete frame;
	cvReleaseImage(&image);
	cvReleaseImage(&raw);
	cvReleaseImage(&eigImage);
	cvReleaseImage(&tempImage);
	free(features);
}

void FeatureDetector::show(char* window)
{
	cvShowImage(window,((IplImage*)(image)));
}

IplImage* FeatureDetector::getAnalyzedImage()
{
	return (IplImage*)(image);
}

//This should be 3 channel to 1 channel image copying, channel is 0 1 or 2, for the channel to copy.
void FeatureDetector::copyChannel(IplImage* src, IplImage* dest, int channel)
{
	char* data=src->imageData;
	char* data2=dest->imageData;
	int width=image->width;
	int height=image->height;
	int count=0;
	int count2=0;
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			data2[count2]=data[count+channel];
			count+=3;
			count2++;
		}
}

void FeatureDetector::update()
{
	cam->getImage(frame);
	raw=(IplImage*)(*frame);
	cvCopyImage(raw,image);
//	copyChannel(image,grayscale,2);//Lets try just copying the red channel
	cvCvtColor(image,grayscale,CV_BGR2GRAY);
	cvCanny(grayscale,edgeDetected, 50, 100, 3 );
	//The two floats are the minimum quality of features, and minimum euclidean distance between features.
	//The NULL says use the entire image.
	int numFeatures=maxFeatures;
	cvGoodFeaturesToTrack(edgeDetected,eigImage,tempImage,features,&numFeatures,.15,30,NULL);
	
	for (int i=0; i<numFeatures; i++)
	{
		CvPoint topLeft;
		CvPoint bottomRight;
		topLeft.x=features[i].x-2;
		topLeft.y=features[i].y+2;
		bottomRight.x=features[i].x+2;
		bottomRight.y=features[i].y-2;
		cvLine(image,topLeft,bottomRight,CV_RGB(0,0,255),1,CV_AA,0);
		int temp;
		temp=topLeft.y;
		topLeft.y=bottomRight.y;
		bottomRight.y=temp;
		cvLine(image,topLeft,bottomRight,CV_RGB(0,0,255),1,CV_AA,0);
	}
}


