/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/demos/src/houghDemo.cpp
 */

// STD Includes
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>

// System Includes
#include <unistd.h>

// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/demos/include/houghDemo.h"

using namespace std;
int main( int argc, char** argv )
{
    // load image in constructor: the image can be loaded either from bitmap (see cvLoadImage),
    // or from XML/YAML (see cvLoad)
//    CvImage img(argc > 1 ? argv[1] : "stopsign.jpg", 0, CV_LOAD_COLOR),
//        img_yuv, y, noise;
//    CvRNG rng = cvRNG(-1);
//
	CvImage img=cvLoadImage("screen13.jpg",0);
    if( !img.data() ) // check if the image has been loaded properly
        return -1;

    cvNamedWindow( "Hough Demo", CV_WINDOW_AUTOSIZE );
    img.show( "Hough Demo" ); // .show method is OO method of cvShowImage
    cvWaitKey(-1); //Wait until a key is pressed
	hough(img);
	cvWaitKey(-1);
	cout<<img.width()<<" "<<img.height()<<endl;

//while (true)
//{
//	uchar* data=img.data();
//	int count=0;
//	int width=img.width();
//	int height=img.height();
//	for (int y=0; y < height; y++)
//		for (int x=0; x < width; x++)
//			{
//				data[count]*=.99;
//				data[count+1]*=.99;
//				data[count+2]*=.99;
//
//				count+=3;
//			}
//
//	cvNamedWindow( "Hough Demo", CV_WINDOW_AUTOSIZE );
//    img.show( "Hough Demo" ); // .show method is OO method of cvShowImage
////	cvWaitKey(-1);
//}

//	CvCapture* auvsi_movie=cvCaptureFromFile("auvsi.mpg");
//	CvCapture* auvsi_movie=cvCaptureFromCAM(0);
	
//	IplImage* frame=NULL;
//	cvNamedWindow("Edges",1);
//	while (true)
//	{
//		if (!auvsi_movie)
//		{
//			cout<<"No camera found :("<<endl;
//			return -1;
//		}
//		int okay=cvGrabFrame(auvsi_movie);
//		frame=cvRetrieveFrame(auvsi_movie);
////		find_green(frame);
//		LU2filter(frame);
//		sobel(frame);
//		mask(frame,2,200);
//		hough(frame);
//		cvShowImage("Edges",frame);
//		sleep(0.003);
//	}	
//	return 0;

}

	void mask(IplImage* image, int bgr, unsigned int threshold)
	{
		if (bgr>2 || bgr < 0)
		{
			cerr<<"Dumbass."<<endl;
			return;
		}
		
		char* data=image->imageData;
		int width=image->width;
		int height=image->height;
		int count=0;
		bool flag=0;
		for (int y=0; y < height; y++)
			for (int x=0; x < width; x++)
			{
				unsigned int color=data[count+bgr];
				if (color>threshold)
					flag=1;
				else
					flag=0;
				data[count]=0;
				data[count+1]=0;
				data[count+2]=0;
				if (flag)
					data[count+bgr]=255;
				count+=3;
			}
	}

	void find_green(IplImage* image)
	{
		char* data=image->imageData;
		int width=image->width;
		int height=image->height;
		int count=0;
		for (int y=0; y < height; y++)
			for (int x=0; x < width; x++)
			{
				//Stupid OpenCV stores in BGR format
				int b= data[count];
				int g= data[count+1];
				int r= data[count+2];
				
				if ((g>r+10) && (g>b+10))
				{	
//					data[count]=data[count+2]=0;
					data[count+1]=255;
				}
				count+=3;
			}
	}

	void LU2filter(IplImage* image)
	{
		char* data=image->imageData;
		int width=image->width;
		int height=image->height;
		int count=0;
		unsigned int distance=0;
		for (int y=0; y < height; y++)
			for (int x=0; x < width; x++)
			{
				//BGR
				int b= data[count];
				int g= data[count+1];
				int r= data[count+2];
				distance=b*b+g*g+r*r / 3;
//				distance=(128-(unsigned int)data[count])*(128-(unsigned int)data[count]) +
//					 (128-(unsigned int)data[count+1])*(128-(unsigned int)data[count+1]) +
//					 (128-(unsigned int)data[count+2])*(128-(unsigned int)data[count+2]);
				distance= (unsigned int)sqrt((double)distance);
				data[count]=distance;
				data[count+1]=distance;
				data[count+2]=distance;
				count+=3;
			}
	}
	
	void sobel(IplImage* image)
	{
		char* data= image->imageData;
		int width = image->width;
		int height = image->height;
		int GX[][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
		int GY[][3]={{1,2,1},{0,0,0},{-1,-2,-1}};	

		int count=3*width;

		for (int y=1; y<height-1;++y)
		{
			count+=3;
			for (int x=1; x<width-1; ++x)
			{
				int sumX = 0;
				int	sumY = 0;
				int weightx;
				int weighty;
				for (int i=0; i<3;i++)
					for (int j=0; j<3; j++)
					{
						weightx=GX[i][j];
						weighty=GY[i][j];
						sumX+=data[count+(1-i)*(3)+(1-j)*(3*width)] * weightx;
						sumY+=data[count+(1-i)*(3)+(1-j)*(3*width)] * weighty;
					}

				int sum = abs(sumX) + abs(sumY);
		
				if(sum>255)
					sum=255;
				if(sum<0)
					sum=0;
				
				data[count+2]=sum;				
				count+=3;
			}
			count+=3;
		}
	}
	
	//cvHough
	void hough(IplImage* img)
	{
	    IplImage* src =img;//cvLoadImage("DSC00099.jpg", 0 );
		
		IplImage* dst;
		IplImage* color_dst;
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* lines = 0;
		int i;
	
		//if( !src )
		//	return;
		
		dst = cvCreateImage( cvGetSize(src), 8, 1 );
		color_dst = cvCreateImage( cvGetSize(src), 8, 3 );

//		IplConvKernel* element = cvCreateStructuringElementEx(2*2+1, 2*2+1, 2, 2, CV_SHAPE_RECT, 0 );
//		cvDilate(dst,dst,element);
		cvCvtColor( dst, color_dst, CV_GRAY2BGR );	
		cvCanny( src, dst, 50, 200, 3 );

	#if 0
		lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 200, 0, 0 );

		for( i = 0; i < MIN(lines->total,100); i++ )
		{
			float* line = (float*)cvGetSeqElem(lines,i);
			float rho = line[0];
			float theta = line[1];
			CvPoint pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000*(-b));
			pt1.y = cvRound(y0 + 1000*(a));
			pt2.x = cvRound(x0 - 1000*(-b));
			pt2.y = cvRound(y0 - 1000*(a));
			cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0), 3, CV_AA, 0 );
		}
	#else
		lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 100, 50, 10 );
		for( i = 0; i < lines->total; i++ )
		{
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
		}
	#endif

		cvShowImage( "Hough Demo", color_dst );
	}





//DanHough
//
//	hough(uchar imgArray[],int width, int height)
//	{
//		HashMap<Line, Integer> lines=new HashMap<Line,Integer>();
//		int count=0;
//		for (int y=0;y<height;y++)
//			for (int x=0; x<width; x++)
//			{
//				if (imgArray[x][y]>0)
//				{
//					for (double theta=0.0;theta<3.14159;theta+=3.14159/numLines)
//					{
//						int r=(int)(x * Math.sin(theta) + y * Math.cos(theta));//Not sure about this line, maybe..., seems to work for horizontal vertical and 45¼ lines at least
//						Line l=new Line(r, theta);
//						if (lines.containsKey(l))
//							lines.put(l,lines.get(l)+1);
//						else
//							lines.put(l, 0);
//					}
//				}
//			}
//		ArrayList<Line> realLines=new ArrayList<Line>();
//		for (Line l:lines.keySet())
//		{
//			if (lines.get(l)>pointsPerLine)
//				realLines.add(l);
//		}
//		
//		return realLines;
//	}
//







