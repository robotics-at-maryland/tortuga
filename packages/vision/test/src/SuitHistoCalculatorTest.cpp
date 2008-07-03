#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include "vision/include/main.h"
#include <string.h>
#include <iostream>
#include "cv.h"

#define MAX_FILENAME_LEN 32

using namespace std;

using namespace ram;
using namespace vision;


void copyAndMask(IplImage* gray, IplImage* bnw)
{
	if (gray->width != bnw->width || gray->height != bnw->height)
	{
		cerr<<"Image sizes don't match, can't copy and mask!"<<endl;
		return;
	}
	
	int width=gray->width;
	int height=gray->height;
	unsigned char* data = (unsigned char*)gray->imageData;
	unsigned char* data2 = (unsigned char*)bnw->imageData;
	int count=0;
	
	for (int y=0; y<height; y++)
    {
		for (int x=0; x<width; x++)
		{
			data2[count]=(data[count]<=128)*255;
			count++;
		}
    }
}

// A Simple Camera Capture Framework
int main(int argc, char** argv) {
	cvNamedWindow("Display");

	IplImage* inputImage = NULL;
	
	if (2 != argc)
    {
		fprintf(stderr, "Argument error\nUsage: ImageAnalysisTest <image_file>\n");
		return -1;
    }
	
	inputImage = cvLoadImage(argv[1],0);//Negative flag means load as is, positive means force 3 channel, 0 means force grayscale
	
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
	IplImage* resized = cvCreateImage(cvSize(64,64),8,1);
    cvResize(inputImage,resized);
	IplImage* blackAndWhite=cvCreateImage(cvGetSize(resized),8,1);
    copyAndMask(resized,blackAndWhite);
	cvShowImage("Display",blackAndWhite);
	cvWaitKey(0);
    
    printf("Start Of Data\n");
    
    printf("int pixelCounts%s[] = {\n", argv[1]);
    unsigned char* data = (unsigned char*) blackAndWhite->imageData;
    int count = 0;
    int pixelCountsAlongLeft[64];
    for (int i = 0; i < 64; i++)
        pixelCountsAlongLeft[i]=0;
    
    for (int y = 0; y < 64; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            pixelCountsAlongLeft[y] += (data[count]>0);
            count++;
        }
    }
    
    for (int i = 0; i < 64; i ++)
    {
        printf("%d, ", pixelCountsAlongLeft[i]); 
    }
    printf("}\n");
    
	return 0;
}
