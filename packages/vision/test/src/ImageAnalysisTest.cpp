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
		for (int x=0; x<width; x++)
		{
			data2[count]=(data[count]<=128)*255;
			count++;
		}
}

void closeDoors(IplImage* shortLines, IplImage* stretchedLines)
{
	if ((shortLines->width != stretchedLines->width) || (shortLines->height != stretchedLines->height))
	{
		cerr<<"Image sizes don't match, can't copy and mask!"<<endl;
		return;
	}

	int width=shortLines->width;
	int height=shortLines->height;
	unsigned char* data = (unsigned char*) shortLines->imageData;
	unsigned char* data2 = (unsigned char*) stretchedLines->imageData;

	int gapsClosed[100];
	for (int i=0; i<100; i++)
		gapsClosed[i]=0;
		
	int doorCount=0;
		
	int count=0;
	int stretch=0;
	int pixelsInARow=0;
	const int minPixelsInWall=7;
	for (int y=0; y<height; y++)
	{
		stretch=0;
		pixelsInARow=0;
		
		for (int x=0; x<width; x++)
		{
			if (data[count]==0)
			{
				if (stretch>0)//door closed
				{
					if (stretch>100)
						cout<<"stretch too big too fit"<<endl;
					else
					{
						gapsClosed[stretch]++;
						doorCount++;
					}
					stretch=0;
				}
				data2[count]=0;
				pixelsInARow++;
			}
			else if (pixelsInARow >= minPixelsInWall)
			{
				stretch++;
				pixelsInARow=0;
				data2[count]=255;
			}
			else if (stretch > 0)
			{
//				data2[count]=128;
				data2[count]=255;
				stretch++;
			}
			else
			{
				data2[count]=255;
				pixelsInARow=0;
			}
				
			count++;
		}
	}
	
	count--; //Set count to last element of imagedata. and go backwards
	for (int y=height-1; y>=0; y--)
	{
		stretch=0;
		pixelsInARow=0;
		
		for (int x=width-1; x>=0; x--)
		{
			if (data[count]==0)
			{
				if (stretch>0)//door closed
				{
					if (stretch>100)
						cout<<"stretch too big too fit"<<endl;
					else
					{
						gapsClosed[stretch]++;
						doorCount++;
					}
					stretch=0;
				}
				data2[count]=0;
				pixelsInARow++;
			}
			else if (pixelsInARow >= minPixelsInWall)
			{
				stretch++;
				pixelsInARow=0;
				data2[count]=255;
			}
			else if (stretch > 0)
			{
//				data2[count]=128;
				data2[count]=255;
				stretch++;
			}
			else
			{
				data2[count]=255;
				pixelsInARow=0;
			}
			
			count--;
		}
	}
	
	for (int i=0; i<100; i++)
		cout<<i<<" : "<<gapsClosed[i]<<endl;

	cout<<"Total Doors"<<doorCount<<endl;
		
	float percent=0;
	int doorLength=0;
	for (int i=1; i<100; i++)
	{
		gapsClosed[i]=gapsClosed[i-1]+gapsClosed[i];
		percent=(gapsClosed[i]/(float)doorCount*100.0);
		cout<<i<<" : " <<gapsClosed[i]<<"   " << percent<<endl;
		if (percent>50)
		{
			cout<<"Guessing " << i << " pixels is approximate door length."<<endl;
			doorLength=i;
			break;
		}
	}
	
	//Now shut these doors!
	/////////////////////////////////////////
	for (int y=0; y<height; y++)
	{
		stretch=0;
		pixelsInARow=0;
		
		for (int x=0; x<width; x++)
		{
			if (data[count]==0)
			{
				if (stretch>0)//door closed
				{
					if (stretch <= doorLength)
						for (int z=stretch; z>=0; z--)
							data2[count-z]=128;
					stretch=0;
				}
				data2[count]=0;
				pixelsInARow++;
			}
			else if (pixelsInARow >= minPixelsInWall)
			{
				stretch++;
				pixelsInARow=0;
				data2[count]=255;
			}
			else if (stretch > 0 )
			{
				data2[count]=255;
				stretch++;
			}
			else
			{
				data2[count]=255;
				pixelsInARow=0;
			}
				
			count++;
		}
	}
	
	count--; //Set count to last element of imagedata. and go backwards
	for (int y=height-1; y>=0; y--)
	{
		stretch=0;
		pixelsInARow=0;
		
		for (int x=width-1; x>=0; x--)
		{
			if (data[count]==0)
			{
				if (stretch>0)//door closed
				{
					if (stretch <= doorLength)
						for (int z=stretch; z>=0; z--)
							data2[count+z]=128;
					stretch=0;
				}
				data2[count]=0;
				pixelsInARow++;
			}
			else if (pixelsInARow >= minPixelsInWall)
			{
				stretch++;
				pixelsInARow=0;
				data2[count]=255;
			}
			else if (stretch > 0 )
			{
				data2[count]=255;
				stretch++;
			}
			else
			{
				data2[count]=255;
				pixelsInARow=0;
			}
				
			count--;
		}
	}
}

void stretch(IplImage* shortLines, IplImage* stretchedLines)
{
	if ((shortLines->width != stretchedLines->width) || (shortLines->height != stretchedLines->height))
	{
		cerr<<"Image sizes don't match, can't copy and mask!"<<endl;
		return;
	}

	int width=shortLines->width;
	int height=shortLines->height;
	unsigned char* data = (unsigned char*) shortLines->imageData;
	unsigned char* data2 = (unsigned char*) stretchedLines->imageData;
	
	int count=0;
	int stretch=0;
	
	const int stretchCap=3;
	
	const int pixelsPerStretch=5;
	int pixelsInARow=0;
	for (int y=0; y<height; y++)
	{
		stretch=0;
		pixelsInARow=0;
		
		for (int x=0; x<width; x++)
		{
			if (data[count]==0)
			{
				data2[count]=0;
				pixelsInARow++;
				if (pixelsInARow==pixelsPerStretch)
				{
					stretch++;
					if (stretch>stretchCap)
						stretch=stretchCap;
					pixelsInARow=0;
				}
			}
			else if (stretch > 0)
			{
				pixelsInARow=0;
				data2[count]=0;
				stretch--;
			}
			else
			{
				data2[count]=255;
				pixelsInARow=0;
			}
				
			count++;
		}
	}
	
	count--; //Set count to last element of imagedata.
	for (int y=height-1; y>=0; y--)
	{
		stretch=0;
		pixelsInARow=0;
		
		for (int x=width-1; x>=0; x--)
		{
			if (data[count]==0)
			{
				data2[count]=0;
				pixelsInARow++;
				if (pixelsInARow==pixelsPerStretch)
				{
					stretch++;
					if (stretch>stretchCap)
						stretch=stretchCap;

					pixelsInARow=0;
				}
			}
			else if (stretch > 0)
			{
				pixelsInARow=0;
				data2[count]=0;
				stretch--;
			}
			else
			{
				data2[count]=255;
				pixelsInARow=0;
			}
				
			count--;
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

	cvShowImage("Display",inputImage);
	cvWaitKey(0);
	
	IplImage* blackAndWhite=cvCreateImage(cvGetSize(inputImage),8,1);
	IplImage* colorVersion =cvCreateImage(cvGetSize(inputImage),8,3);
	cvConvertImage(inputImage, colorVersion, CV_GRAY2BGR);
	
	copyAndMask(inputImage, blackAndWhite);
	
//	cvShowImage("Display",blackAndWhite);
//	cvWaitKey(0);
//	
//	stretch(blackAndWhite,inputImage);
//	cvShowImage("Display",inputImage);
//	cvWaitKey(0);
//	
//	closeDoors(inputImage,blackAndWhite);
//	cvShowImage("Display",blackAndWhite);
//	cvWaitKey(0);
//	
	///////////////////////
	
//	IplImage* color_dst =img;//cvLoadImage("DSC00099.jpg", 0 );
//	IplImage* src = cvCreateImage(cvGetSize(img), 8, 1);
//	cvCvtColor(img,src,CV_BGR2GRAY);

	IplImage* src = inputImage;
	IplImage* dst = blackAndWhite;
	
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	int i;
	
	//if( !src )
	//	return;
	
	//dst = cvCreateImage( cvGetSize(src), 8, 1 );
	//		color_dst = cvCreateImage( cvGetSize(src), 8, 3 );
	
	IplConvKernel* element = cvCreateStructuringElementEx(2*2+1, 2*2+1, 2, 2, CV_SHAPE_RECT, 0 );
	cvDilate(dst,dst,element);
	//		cvCvtColor( dst, color_dst, CV_GRAY2BGR );	
	//cvCanny( src, dst, 50, 200, 3 );
	cvShowImage("Display",dst);
	cvWaitKey(0);

	//Image
	//Storage
	//Transform to use
	//rho
	//theta
	//threshold
	//linelength
	//linegap
	
	lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/4, 15, 15, 1 );
	CvPoint start,end;
	
	start.x=start.y=end.x=end.y=0;
	int missed = 0;
	int temp;
	for( i = 0; i < lines->total; i++ )
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
		if (((line[0].x < 5)&&(line[1].x < 5)) || ((line[0].x > src->width-5)&&(line[1].x > src->width-5)) 
			|| ((line[0].y < 5)&&(line[1].y < 5)) || ((line[0].y > src->height-5)&&(line[1].y > src->height-5))) 
		{
			missed++;
			//cout<<"Missed++"<<endl;
		}
		else 
		{
			if(line[0].y < line[1].y)
			{
				temp = line[0].y;
				line[0].y = line[1].y;
				line[1].y = temp;
				temp = line[0].x;
				line[0].x = line[1].x;
				line[1].x = temp;
			}
			start.x += line[0].x;
			start.y += line[0].y;
			end.x += line[1].x;
			end.y += line[1].y;
		}
		
		cvLine(colorVersion, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
	}
	cvShowImage("Display", blackAndWhite);
	cvWaitKey(0);
	
	cvShowImage("Display", colorVersion);
	cvWaitKey(0);
	return 0;
}
