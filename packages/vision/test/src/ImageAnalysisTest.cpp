#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include "vision/include/main.h"
#include <string.h>
#include <iostream>
#define MAX_FILENAME_LEN 32

using namespace std;

using namespace ram;
using namespace vision;

char outName[MAX_FILENAME_LEN];

void clear(char* str, int size=MAX_FILENAME_LEN); //Clear a char* array
char* makeOut(char* description, int size=MAX_FILENAME_LEN);

char* makeOut(char* description, int size)
{
	char* outStarter="out-";
	char* outEnder=".jpg";
	clear(outName,size);
	sprintf(outName,"%s%s%s",outStarter,description,outEnder);
	return outName;
}

void clear(char* str, int size)
{
	for (int i=0; i<size; i++)
		str[i]='\0';
}

// A Simple Camera Capture Framework
int main(int argc, char** argv) {
	cvNamedWindow("raw");

	IplImage* inputImage = NULL;
	
	if (2 != argc)
    {
		fprintf(stderr, "Argument error\nUsage: ImageAnalysisTest <image_file>\n");
		return -1;
    }
	
	inputImage = cvLoadImage(argv[1],-1);//Negative flag means load as is, positive means force 3 channel, 0 means force grayscale
	
	
	if (inputImage==NULL)
	{
		cout<<"Couldn't load image"<<endl;
		return -1;
	}

	// get the image data
	cout<<"height = "<<inputImage->height <<" width = "<<inputImage->width <<endl<<"step = "<<inputImage->widthStep<<"channels = "<<inputImage->nChannels<<endl;
	cout<<"depth = " <<inputImage->depth<< "dataOrder = "<<inputImage->dataOrder<<endl;
	printf("color model =%c%c%c%c\n",inputImage->colorModel[0],inputImage->colorModel[1],inputImage->colorModel[2],inputImage->colorModel[3]);
/* IplImage
  |-- int  nChannels;     // Number of color channels (1,2,3,4)
  |-- int  depth;         // Pixel depth in bits: 
  |                       //   IPL_DEPTH_8U, IPL_DEPTH_8S, 
  |                       //   IPL_DEPTH_16U,IPL_DEPTH_16S, 
  |                       //   IPL_DEPTH_32S,IPL_DEPTH_32F, 
  |                       //   IPL_DEPTH_64F
  |-- int  width;         // image width in pixels
  |-- int  height;        // image height in pixels
  |-- char* imageData;    // pointer to aligned image data
  |                       // Note that color images are stored in BGR order
  |-- int  dataOrder;     // 0 - interleaved color channels, 
  |                       // 1 - separate color channels
  |                       // cvCreateImage can only create interleaved images
  |-- int  origin;        // 0 - top-left origin,
  |                       // 1 - bottom-left origin (Windows bitmaps style)
  |-- int  widthStep;     // size of aligned image row in bytes
  |-- int  imageSize;     // image data size in bytes = height*widthStep
  |-- struct _IplROI *roi;// image ROI. when not NULL specifies image
  |                       // region  to be processed.
  |-- char *imageDataOrigin; // pointer to the unaligned origin of image data
  |                          // (needed for correct image deallocation)
  |
  |-- int  align;         // Alignment of image rows: 4 or 8 byte alignment
  |                       // OpenCV ignores this and uses widthStep instead
  |-- char colorModel[4]; // Color model - ignored by OpenCV
*/

	cvShowImage("raw",inputImage);
	cvWaitKey(0);

	cvSaveImage(makeOut("raw"),inputImage);
	
	//REDLIGHT
	{
		IplImage* image=cvCreateImage(cvGetSize(inputImage),8,3);
		IplImage* flashFrame=cvCreateImage(cvGetSize(image), 8, 3);
		
		cvCopyImage(inputImage,image);
		cvCopyImage(image, flashFrame);
	
		to_ratios(image);
		CvPoint p;
		redMask(image,flashFrame);
		cvSaveImage(makeOut("redmask"),flashFrame);
		
		int redPixelCount=histogram(flashFrame,&p.x,&p.y);	
		if (redPixelCount<75)
		{
			cout<<"No red light found."<<endl;
		}	
		else
		{
			cout<<"FOUND RED LIGHT"<<endl;
			CvPoint tl,tr,bl,br;
			cvCopyImage(inputImage,image);
			
			tl.x=bl.x=max(p.x-4,0);
			tr.x=br.x=min(p.x+4,image->width-1);
			tl.y=tr.y=min(p.y+4,image->height-1);
			br.y=bl.y=max(p.y-4,0);
			
			cvLine(image, tl, tr, CV_RGB(0,0,255), 3, CV_AA, 0 );
			cvLine(image, tl, bl, CV_RGB(0,0,255), 3, CV_AA, 0 );
			cvLine(image, tr, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
			cvLine(image, bl, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
			cvSaveImage(makeOut("redfound"),image);
		}
	}

	return 0;
}
