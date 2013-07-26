/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/GateDetector.cpp
 */


// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/GateDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"
#include "vision/include/VisionSystem.h"
#include "vision/include/TableColorFilter.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/GateDetectorKate.h"


using namespace std;
using namespace cv;


namespace ram {
namespace vision {

GateDetector::GateDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0)
{
    init(config);
}
    
GateDetector::GateDetector(Camera* camera) :
    cam(camera)
{
    init(core::ConfigNode::fromString("{}"));

}

void GateDetector::init(core::ConfigNode config)
{
	m_found = false;
	frame = new OpenCVImage(640,480);
	gateX=0;
	gateY=0;
	found=false;
	//This frame will be a copy of the original rotated 90¼ counterclockwise.  
	//But only if the camera is on sideways, otherwise we do 640 by 480 like usual.
	//ie 640 by 480 if cameras on right (Or completely upsidedown!... sigh), else 480 by 640.
	gateFrame =cvCreateImage(cvSize(640,480),8,3);
	gateFrameRatios = cvCreateImage(cvGetSize(gateFrame),8,3);

	core::PropertySetPtr propSet(getPropertySet());

    // Color filter properties
//Kate edit: trying to edit the VisionToolV2 gui to allow sliders    
	propSet->addProperty(config, false, "RedHuemin",
                         "RedHuemin",
                         9, &m_redminH, 0, 255);
	propSet->addProperty(config, false, "RedHuemax",
                         "RedHuemax",
                         127, &m_redmaxH, 0, 255);
	propSet->addProperty(config, false, "GreenHuemin",
                         "GreenHuemin",
                         25, &m_greenminH, 0, 255);
	propSet->addProperty(config, false, "GreenHuemax",
                         "GreenHuemax",
                         80, &m_greenmaxH, 0, 255);
	
	//saturaion
	propSet->addProperty(config, false, "Saturationmin",
                         "Saturationmin",
                         25, &m_minS, 0, 255);
	propSet->addProperty(config, false, "Saturationmax",
                         "Saturationmax",
                         95, &m_maxS, 0, 255);
	//value
	propSet->addProperty(config, false, "Valuemin",
                         "Valuemin",
                         0, &m_minV, 0, 255);
	propSet->addProperty(config, false, "Valuemax",
                         "Valuemax",
                         255, &m_maxV, 0, 255);

	propSet->addProperty(config, false, "diff",
                         "diff",
                         100, &m_maxdiff, 0,300);
   	propSet->addProperty(config, false, "thresholdwithred",
                         "thresholdwithred",
                         false, &m_checkRed);
	propSet->addProperty(config, false, "ErosionSize",
                         "ErosionSize",
                         2, &m_erosion_size, 0,5);
	propSet->addProperty(config, false, "MaxAngleDiff",
                         "MaxAngleDiff",
                         20, &m_maxanglediff, 0,45);

	propSet->addProperty(config, false, "maxAspectRatio",
                         "maxAspectRatio",
                         6.0, &m_maxAspectRatio, 1.0,10.0);

	propSet->addProperty(config, false, "minAspectRatio",
                         "minAspectRatio",
                         0.25, &m_minAspectRatio, 0.0,10.0);

	propSet->addProperty(config, false, "minArea",
                         "minArea",
                         200, &m_minArea, 0,5000);

propSet->addProperty(config, false, "RefelctionDiff",
                         "ReflectionDiff",
                         50, &m_reflectiondiff, 0,500);

propSet->addProperty(config, false, "CannyLow",
                         "CannyLow",
                         50, &m_cannylow, 0,900);


propSet->addProperty(config, false, "CannyHigh",
                         "CannyHigh",
                         150, &m_cannyhigh, 0,5000);

propSet->addProperty(config, false, "ToptoIgnore",
                         "ToptoIgnore",
                         150, &m_minY, 0,500);

propSet->addProperty(config, false, "Dilate",
                         "Dilate",
                         1, &m_dilatesize, 0,5);



}
    
GateDetector::~GateDetector()
{
	delete frame;
	cvReleaseImage(&gateFrame);
	cvReleaseImage(&gateFrameRatios);
}

double GateDetector::getX()
{
	return gateXNorm;
}

double GateDetector::getY()
{
	return gateYNorm;
}

void GateDetector::show(char* window)
{
	cvShowImage(window,((IplImage*)(gateFrame)));
}

IplImage* GateDetector::getAnalyzedImage()
{
	return (IplImage*)(gateFrame);
}

    
void GateDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}
    
int GateDetector::returnRedmin(void)
{
return(m_redminH);
}
int GateDetector::returnRedmax(void)
{
return(m_redmaxH);
}




Mat GateDetector::processImageColor(Image*input)
{

	int red_maxH = m_redmaxH;
	int red_minH = m_redminH;
	int green_minH = m_greenminH;
	int green_maxH = m_greenmaxH;
	int minV = m_minV;
	int maxV = m_maxV;
	double minS = (double)m_minS;
	double maxS = (double)m_maxS;

	//printf("\n saving img from input");
	Mat img = input->asIplImage();

	img_whitebalance = WhiteBalance(img);
	Mat temphsv;
	cvtColor(img_whitebalance,temphsv,CV_BGR2HSV);
	vector<Mat> hsv_planes;
	split(temphsv,hsv_planes);
	Mat tempWhite;
	equalizeHist(hsv_planes[1], hsv_planes[1]);
	equalizeHist(hsv_planes[2], hsv_planes[2]);
	merge(hsv_planes,temphsv);
	cvtColor(temphsv,img_whitebalance,CV_HSV2BGR);

	

	//printf("\n processImageColor");
	cv::Mat img_hsv(img_whitebalance.size(),CV_8UC3); //HSV image
	cv::Mat img_red(img_whitebalance.size(),CV_8UC1); //red image
	cv::Mat img_green(img_whitebalance.size(),CV_8UC1); //green image
	cv::Mat erosion_dst(img_whitebalance.size(),CV_8UC1); //Erosion destination
	cv::Mat img_saturation(img_whitebalance.size(),CV_8UC1); //S filter
	cv::Mat img_luminance(img_whitebalance.size(),CV_8UC1);	//Vlaue filter
	cv::Mat temp(img_whitebalance.size(),CV_8UC1); //temporary image for use when bitwising
	cv::Mat dilate_dst(img_whitebalance.size(),CV_8UC1); //temporary image for use when bitwising


	//printf("\n entering whitebalance");
	//cvtColor(img_whitebalance,img_hsv,CV_BGR2HSV);
	//vector<Mat> hsv_planes;
	split(img_whitebalance,hsv_planes);


	//first take any value higher than max and converts it to 0
	//red is a special case because the hue value for red are 0-10 and 170-1980
	//same filter as the other cases followed by an invert

	//filter images
	blobfinder blob;

	int erosion_type = 0; //morph rectangle type of erosion
	int erosion_size = m_erosion_size;
	Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

	if (m_checkRed == true)
	{
		img_red =blob.RedFilter(hsv_planes,red_minH,red_maxH);	
		if (minS != 0 || maxS != 255)	
		{
			img_saturation = blob.SaturationFilter(hsv_planes,minS,maxS);
			bitwise_and(img_saturation,img_red,temp,noArray());
			img_red = temp;
			//imshow("Sat ReD",img_saturation);
		}
		if (minV != 0 || maxV != 255)	
		{
			Mat img_luminance_red = blob.LuminanceFilter(hsv_planes,minV,maxV);
			bitwise_and(img_luminance_red,img_red,temp,noArray());
			img_red = temp;
			//imshow("Luminance Red",img_Luminance);
		}	
	  	erode(img_red, erosion_dst, element);

		int dilate_size = m_dilatesize;
		cv::Mat dilate_element = getStructuringElement( erosion_type,
                                       Size( 2*dilate_size + 1, 2*dilate_size+1 ),
                                       Point( dilate_size, dilate_size ) );
		dilate(erosion_dst, dilate_dst, dilate_element );

	}
	else
	{
		img_green =blob.OtherColorFilter(hsv_planes,green_minH,green_maxH);	
		if (minS != 0 || maxS != 255)	
		{
			img_saturation = blob.SaturationFilter(hsv_planes,minS,maxS);
			bitwise_and(img_saturation,img_green,temp,noArray());
			img_green = temp;
			//imshow("Sat",img_saturation);
		}
		if (minV != 0 || maxV != 255)	
		{
			img_luminance = blob.LuminanceFilter(hsv_planes,minV,maxV);
			bitwise_and(img_luminance,img_green,temp,noArray());
			//imshow("Luminance",img_Luminance);
			img_green = temp;
		}	
	  	erode(img_green, erosion_dst, element);

		int dilate_size = m_dilatesize;
		cv::Mat dilate_element = getStructuringElement( erosion_type,
                                       Size( 2*dilate_size + 1, 2*dilate_size+1 ),
                                       Point( dilate_size, dilate_size ) );
		dilate(erosion_dst, dilate_dst, dilate_element );


	}
	//imshow("AND",erosion_dst);

	return(dilate_dst);
}
void GateDetector::FindContours(Mat img_src)
{
	//do color filter
	//then find contours
	//get a series of contours
	//I want the ones that have 90 degree angles and 0 degrees angle

	//is that enough? Also make sure the height is about 5x the width? maybe? 
	//lets try it

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(img_src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	unsigned int minContourSize = 10;
	//int numbertokeeptrack = 5;
	
	//contourblob vertical[numbertokeeptrack];
	//contourblob horizontal[numbertokeeptrack];
	//keep track of the two largest horizontal and two largest vertical

	finalGate.area = 0;	
	contourblob Vertical_large;
	contourblob Vertical_small;
	contourblob Vertical_smaller;
	contourblob Vertical_smallest;

	contourblob Horizontal_large;
	contourblob Horizontal_small;
	Horizontal_large.area = 0;
	Vertical_large.area = 0;
	Vertical_small.area = 0;
	Vertical_smaller.area = 0;
	Vertical_smallest.area = 0;
	Horizontal_small.area =0;

	RotatedRect temp;
	double area=0;
	double angle = 0;
	Point2f vertices[4];

	int minX=0,minY=0,maxX=0,maxY=0;
	int maxdistance=0, maxdistance2=0,distance=9;
	int midpointx=0,midpointy=0;
	int midpointx1=0,midpointy1=0,midpointx2=0,midpointy2=0;
	double rise=0.0, run=0.0;
	double foundaspectdiff=0.0;
	int minArea = m_minArea;

	for(unsigned int j=0; j<contours.size(); j++)
	{
		if (contours[j].size() > minContourSize)	
		{
			minX = 9000;
			minY=9000;
			maxX=0;
			maxY=0;
			temp = minAreaRect(contours[j]); //finds the rectangle that will encompass all the points
			area = temp.size.width*temp.size.height;
			foundaspectdiff = abs(temp.size.height/temp.size.width);
			temp.points(vertices);
			if (temp.center.y > m_minY)
			{
				//get angle
				maxdistance =0;
				maxdistance2 = 0;
				foundaspectdiff = abs(temp.size.height/temp.size.width);
				for (int i = 0; i < 4; i++)
				{
					midpointx=(int)((vertices[i].x+vertices[(i+1)%4].x)/2);
					midpointy =(int)((vertices[i].y+vertices[(i+1)%4].y)/2);

					distance = abs(temp.center.x-midpointx)+abs(temp.center.y-midpointy);
					if (distance > maxdistance)
					{
						maxdistance2 = maxdistance;
						midpointx2=midpointx1;
						midpointy2=midpointy1;
						maxdistance = distance;
						midpointx1 = midpointx;
						midpointy1 = midpointy;
					}
					else if (distance > maxdistance2)
					{
						maxdistance2 = distance;
						midpointx2 = midpointx;
						midpointy2 = midpointy;
					}
					if (vertices[i].x < minX)
						minX = vertices[i].x;
					if (vertices[i].x>maxX)
						maxX = vertices[i].x;
					if (vertices[i].y < minY)
						minY = vertices[i].y;
					if (vertices[i].y > maxY)
						maxY = vertices[i].y;
				
				}
				foundaspectdiff = double(maxY-minY)/double(maxX-minX);
				//lower point always first
				rise = (midpointy2-midpointy1);
				run = (midpointx2-midpointx1);
		
	
				if (run != 0)
				{
					angle =atan((double)rise/(double)run)*(180.0/3.14);
					if (angle < 0)
					{
						angle = angle + 90; //correction to make vertical = 0
					}
					else if (angle > 0)
					{
						angle = angle -90; //correction to make vertical = 0
					}		
				}		
				else
				{
					angle = 0;
				}

				angle = abs(angle);
				drawContours(img_whitebalance,contours,j, Scalar(0,150,0), 2, 8, hierarchy, 0, Point() ); 
			}
			else
			{
				area = 0;		
			}
			//printf("\n area = %f, aspectratio = %f, angle = %f, rise = %f, run=%f",area, foundaspectdiff,angle,rise,run);
			if (area > minArea && abs(angle-90)<m_maxanglediff&& (foundaspectdiff < m_minAspectRatio))
			{
				//Horizontal
				if (area > Horizontal_large.area)
				{
				 	Horizontal_small = Horizontal_large;
				 	Horizontal_large.area =area;
				  	Horizontal_large.angle = angle;
					Horizontal_large.contournumber = j;
					Horizontal_large.width = temp.size.width;
					Horizontal_large.height = temp.size.height;
					Horizontal_large.centerx = temp.center.x;
					Horizontal_large.centery = temp.center.y;
				}		
				else if (area > Horizontal_small.area)
				{
				 	Horizontal_small.area =area;
				  	Horizontal_small.angle = angle;
					Horizontal_small.contournumber = j;
					Horizontal_small.width = temp.size.width;
					Horizontal_small.height = temp.size.height;
					Horizontal_small.centerx = temp.center.x;
					Horizontal_small.centery = temp.center.y;
				}
				//drawContours(img_whitebalance,contours,j, Scalar(255,0,0), 2, 8, hierarchy, 0, Point() ); 
				//for (int i = 0; i < 4; i++)
				//{
				//	line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,255,0),5);
				//}

			}
			if (area > minArea && abs(angle)<m_maxanglediff && (foundaspectdiff > m_maxAspectRatio))// || foundaspectdiff < m_minAspectRatio))
			{
				//vertical

				if (area > Vertical_large.area)
				{
					Vertical_smallest = Vertical_smaller;
					Vertical_smaller = Vertical_small;
				 	Vertical_small = Vertical_large;
				 	Vertical_large.area =area;
				  	Vertical_large.angle = angle;
					Vertical_large.contournumber = j;
					Vertical_large.width = temp.size.width;
					Vertical_large.height = temp.size.height;
					Vertical_large.centerx = temp.center.x;
					Vertical_large.centery = temp.center.y;
				}		
				else if (area > Vertical_small.area)
				{
					Vertical_smallest = Vertical_smaller;
					Vertical_smaller = Vertical_small;
				 	Vertical_small.area =area;
				  	Vertical_small.angle = angle;
					Vertical_small.contournumber = j;
					Vertical_small.width = temp.size.width;
					Vertical_small.height = temp.size.height;
					Vertical_small.centerx = temp.center.x;
					Vertical_small.centery = temp.center.y;
				}
				else if (area > Vertical_smaller.area)
				{
					Vertical_smallest = Vertical_smaller;
				 	Vertical_smaller.area =area;
				  	Vertical_smaller.angle = angle;
					Vertical_smaller.contournumber = j;
					Vertical_smaller.width = temp.size.width;
					Vertical_smaller.height = temp.size.height;
					Vertical_smaller.centerx = temp.center.x;
					Vertical_smaller.centery = temp.center.y;
				}
				else if (area > Vertical_smallest.area)
				{
				 	Vertical_smallest.area =area;
				  	Vertical_smallest.angle = angle;
					Vertical_smallest.contournumber = j;
					Vertical_smallest.width = temp.size.width;
					Vertical_smallest.height = temp.size.height;
					Vertical_smallest.centerx = temp.center.x;
					Vertical_smallest.centery = temp.center.y;
				}

				//drawContours(img_whitebalance,contours,j, Scalar(255,255,0), 2, 8, hierarchy, 0, Point() ); //
				//for (int i = 0; i < 4; i++)
				//{
				//	line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,255,255),5);
				//}
			}

		}//end min contour size
	}//end for j


	int Xdistance=9000,Xdistance1=9000;
	int Xdistance_vert1=0,Xdistance_vert2=0;
	//have all the horizontal and vertical pieces. Now to assemble them
	//Given: just two vertical, which makes a gate
	//given: two vertical and a horizontal, need to see if the horizontal is about where the vertical ends
	//given one vertical and one horizonal, need to see if the horizontal is about where the vertical ends

	//DEALING WITH REFLECTION FOR VERTICALS
	int reflection_small=0,reflection_smaller=0,reflection_smallest =0;
	//printf("\n Large = %d, small %d, smaller %d, smallest %d",Vertical_large.centerx, Vertical_small.centerx, Vertical_smaller.centerx, Vertical_smallest.centerx);
	if (Vertical_small.area >minArea && abs(Vertical_small.centerx-Vertical_large.centerx)<m_reflectiondiff)
	{
		//Then small is a reflection of large
		//do dont want to use
		reflection_small =1;
	}
	if (Vertical_smaller.area >minArea && abs(Vertical_smaller.centerx-Vertical_large.centerx)<m_reflectiondiff)
	{
		//Then small is a reflection of large
		//do dont want to use
		reflection_smaller =1;
	}

	if (Vertical_smallest.area >minArea && abs(Vertical_smallest.centerx-Vertical_large.centerx)<m_reflectiondiff)
	{
		//Then small is a reflection of large
		//do dont want to use
		reflection_smallest =1;
	}

	if (reflection_small == 1 && reflection_smaller == 0)
	{
		Vertical_small = Vertical_smaller;
	}
	else if (reflection_small ==1 && reflection_smallest == 0)
	{
		Vertical_small = Vertical_smallest;
	}

	//found gate
	
	if (Vertical_large.area >minArea && Vertical_small.area <=minArea && Horizontal_large.area <=minArea)
	{
		finalGate.area = Vertical_large.area;
		finalGate.centerx = Vertical_large.centerx;
		finalGate.centery = Vertical_large.centery;
		finalGate.width = Vertical_large.width;
		finalGate.height = Vertical_large.height;
		finalGate.gatepieces = 1;
		finalGate.angle = Vertical_large.angle;
	}	
	else if (Horizontal_large.area >minArea && Vertical_large.area < minArea)
	{
		//only have horizontal
		finalGate.area = Horizontal_large.area;
		finalGate.centerx = Horizontal_large.centerx;
		finalGate.centery = Horizontal_large.centery;
		finalGate.width = Horizontal_large.width;
		finalGate.height = Horizontal_large.height;
		finalGate.gatepieces = 1;
		finalGate.angle = Horizontal_large.angle;
	}
	else if ((Horizontal_large.area < minArea || Horizontal_large.area < Vertical_small.area) && Vertical_large.area >minArea && Vertical_small.area >minArea)
	{
		//have two verticals which we have to assume form a gate
		//information to save: center of gate and range, range => height information
		finalGate.area = Vertical_large.area;//(Vertical_large.area + Vertical_small.area)/2;
		finalGate.centerx = (Vertical_large.centerx + Vertical_small.centerx)/2;
		finalGate.centery = (Vertical_large.centery + Vertical_small.centery)/2;
		finalGate.width = abs(Vertical_large.centerx-Vertical_small.centerx);
		finalGate.height = (Vertical_large.height + Vertical_small.height)/2;
		finalGate.gatepieces = 2;
		finalGate.angle = (Vertical_large.angle+Vertical_small.angle)/2;
	}
	else if (Horizontal_large.area >minArea && Vertical_large.area >minArea )
	{
		//have atleast 1 Horizontals and atleast 1 Vertical
		//lets see which is the best corner- minimum x distance from center of verticals to edge of horizontal

		//to do that, I need to find the minX and maxX value for the horizontals
		temp = minAreaRect(contours[Horizontal_large.contournumber]); //finds the rectangle that will encompass all the points
		temp.points(vertices);
		finalGate.gatepieces = 2;
		int minX = 9000;
		int maxX =0;
		
		//Horizontal Line 1
		for (int i = 0; i < 4; i++)
		{
			if (vertices[i].x < minX)
				minX = vertices[i].x;
			if (vertices[i].x>maxX)
				maxX = vertices[i].x;
		}
		//have four distance measurements, want to find the minimum one
		Xdistance = abs(minX-Vertical_large.centerx);
		Xdistance_vert1 = 1;
		if (abs(maxX-Vertical_large.centerx) < Xdistance)
		{
			Xdistance = abs(maxX-Vertical_large.centerx);
			Xdistance_vert1=1;
		}
		if (Vertical_small.area >minArea)
		{
			if (abs(minX-Vertical_small.centerx) < Xdistance)
			{
				Xdistance = abs(minX-Vertical_small.centerx);
				Xdistance_vert1=2;
			}
			if ( abs(maxX-Vertical_small.centerx) < Xdistance)
			{
				Xdistance =  abs(maxX-Vertical_small.centerx);
				Xdistance_vert1=2;
			}
		}
		Xdistance1 = Xdistance;
		//printf("\n minDistance for Horizontal_large = %d with line %d",Xdistance1,Xdistance_vert1);
		//Horizontal Line 2
		Xdistance = 9000;
		if (Horizontal_small.area >minArea)
		{
			temp = minAreaRect(contours[Horizontal_small.contournumber]); //finds the rectangle that will encompass all the points
			temp.points(vertices);
			int minX = 9000;
			int maxX =0;
			for (int i = 0; i < 4; i++)
			{
				if (vertices[i].x < minX)
					minX = vertices[i].x;
				if (vertices[i].x>maxX)
					maxX = vertices[i].x;
			}
			//have four distance measurements, want to find the minimum one
			Xdistance = abs(minX-Vertical_large.centerx);
			Xdistance_vert2=1;
			if (abs(maxX-Vertical_large.centerx) < Xdistance)
			{
				Xdistance = abs(maxX-Vertical_large.centerx);
				Xdistance_vert2=1;
			}
			if (Vertical_small.area >minArea)
			{	
				if (abs(minX-Vertical_small.centerx) < Xdistance)
				{
					Xdistance = abs(minX-Vertical_small.centerx);
					Xdistance_vert2=2;
				}
				if ( abs(maxX-Vertical_small.centerx) < Xdistance)
				{
					Xdistance =  abs(maxX-Vertical_small.centerx);
					Xdistance_vert2=2;
				}

			}
		//printf("\n minDistance for Horizontal_small = %d with line %d",Xdistance,Xdistance_vert2);
		}
		//compare two horizontal lines
		if(Xdistance <Xdistance1)
		{
			//Horizontal_small is closer so use that
			if (Xdistance_vert2 == 1)
			{
				//best corner = Horizontal_small+Vertical_large
				finalGate.area = Vertical_large.area;//(Vertical_large.height*Horizontal_small.width);
				finalGate.centerx = Horizontal_small.centerx;
				finalGate.centery = Vertical_large.centery;
				finalGate.width = Horizontal_small.width;
				finalGate.height = Vertical_large.height;
				finalGate.angle = Horizontal_small.angle;
			}
			else if (Xdistance_vert2 ==2)
			{
				//best corner = Horizontal_small+Vertical_small
				finalGate.area = Vertical_large.area;
				finalGate.centerx = Horizontal_small.centerx;
				finalGate.centery = Vertical_small.centery;
				finalGate.width = Horizontal_small.width;
				finalGate.height = Vertical_small.height;
				finalGate.angle = Horizontal_small.angle;
			}
		}
		else
		{
			//Horizontal_large is closer so use that
			if (Xdistance_vert1 == 1)
			{
				//best corner = Horizontal_large+Vertical_large
				finalGate.area = Vertical_large.area;
				finalGate.centerx = Horizontal_large.centerx;
				finalGate.centery = Vertical_large.centery;
				finalGate.width = Horizontal_large.width;
				finalGate.height = Vertical_large.height;
				finalGate.angle = Horizontal_large.angle;
			}
			else if (Xdistance_vert1 ==2)
			{
				//best corner = Horizontal_large+Vertical_small
				finalGate.area = Horizontal_large.area; //so not right, but its not used so whatever
				finalGate.centerx = Horizontal_large.centerx;
				finalGate.centery = Vertical_small.centery;
				finalGate.width = Horizontal_large.width;
				finalGate.height = Vertical_small.height;
				finalGate.angle = Horizontal_large.angle;
			}
		}
	}

	if (Horizontal_large.area > minArea)
		drawContours(img_whitebalance,contours,Horizontal_large.contournumber, Scalar(255,0,0), 2, 8, hierarchy, 0, Point() ); 
	if (Horizontal_small.area > minArea)
		drawContours(img_whitebalance,contours,Horizontal_small.contournumber, Scalar(150,0,0), 2, 8, hierarchy, 0, Point() ); 
	if (Vertical_large.area > minArea)
		drawContours(img_whitebalance,contours,Vertical_large.contournumber, Scalar(0,0,255), 2, 8, hierarchy, 0, Point() ); 
	if (Vertical_small.area > minArea)
		drawContours(img_whitebalance,contours,Vertical_small.contournumber, Scalar(0,0,150), 2, 8, hierarchy, 0, Point() ); 

	//given just horizontals or one vertical: nothing, cannot see
	if (finalGate.area >minArea)
	{
		circle(img_whitebalance, Point(finalGate.centerx, finalGate.centery),5,Scalar( 0, 255, 255),-1,8 );
		//printf("\n Range: %d", finalGate.width);
	}
	//imshow("Contours",img_whitebalance);
}


void GateDetector::processImage(Image* input, Image* output)
{	
	
	//Mat img = input->asIplImage();
	//Mat img_whitebalance2 = WhiteBalance(img);
	//Mat bw;
	//cvtColor(img_whitebalance2,bw,CV_BGR2HSV);

	Mat imgprocess = processImageColor(input);
	//imshow("ImgProcess",imgprocess);

	//cv::Canny(bw,bw, m_cannylow, m_cannyhigh, 3);
	//imshow("Canny",bw);

	FindContours(imgprocess);
	//foundLines::parallelLinesPairs final= gate.gateblob(imgprocess,img_whitebalance); //built in redfilter
	//Mat img_red = gate.hedgeblob(img_whitebalance);  //built in green filter

	//imshow("results",img_red);
	//img_gate = gate.rectangle(img_red, img_whitebalance);
	//cvtColor(img_whitebalance,img_whitebalance,CV_BGR2RGB);
	//printf("\n done with gateblob");

	if (finalGate.area > m_minArea)
	{
		m_found = TRUE;
		if (m_checkRed == true)
			publishFoundEventContour(finalGate,Color::RED);
		else
			publishFoundEventContour(finalGate,Color::GREEN);
	}
	else if (m_found == TRUE)
	{
		m_found = FALSE;
		if (m_checkRed == true)
			publishLostEventBuoy(Color::RED);
		else
			publishLostEventBuoy(Color::GREEN);
	}
/*
	if (final.foundtwosides == 1 || final.foundHorizontal == 1)
	{
		m_found = TRUE;
	}
	else if  (m_found == TRUE)
	{ //found it before but cannot find it now- lostEvent
		m_found = FALSE;
		if (m_checkRed == true)
			publishLostEventBuoy(Color::RED);
		else
			publishLostEventBuoy(Color::GREEN);
		
        }

	if (m_found==TRUE)
	{
		if (m_checkRed == true)
			publishFoundEventBuoy(final,Color::RED);
		else
			publishFoundEventBuoy(final,Color::GREEN);

		//printf("\n gate found publishing event");
	}
*/
//	printf("\n outputting!");
	cvtColor(img_whitebalance,img_whitebalance,CV_BGR2RGB);
	input->setData(img_whitebalance.data,false);
	frame->copyFrom(input);


	if(output)
	    {
		//imshow("Blue",erosion_dst_blue);
		//imshow("Green",erosion_dst_green);
		//imshow("Red",erosion_dst_red);
		//imshow("AND",erosion_dst);
		output->copyFrom(frame);
		//if (m_debug >= 1) {
		//    output->copyFrom(frame);
		//} //endif mdebug==1

	    } //end if output
	
	
        

//------------end Kate


       // output->setData(output_blob2.data,false);
        //tempImage->imageData = (char *)output_blob2.data;


//  These lines are correct only if the camera is on sideways again.
//	rotate90Deg(image,gateFrame);//Rotate image into gateFrame, so that it will be vertical.
//	rotate90Deg(image,gateFrameRatios); 

// Otherwise just copy like this:
	
	//Could just say gateFrame=(IplImage*)(*frame), 
	//but I've heard openCV gets angry if you write pixels 
	//to image when it is getting images from a camera.
	//TODO: Before changing this, check the Camera class to see if it automatically copies anyway.
/*KATE
	cvCopyImage(image,gateFrame);
	cvCopyImage(gateFrame, gateFrameRatios);

	to_ratios(gateFrameRatios);
	found=gateDetect(gateFrameRatios,gateFrame,&gateX,&gateY) ? 1 : 0;
	gateXNorm=gateX;
	gateYNorm=gateY;
	gateXNorm/=image->width;
	gateXNorm/=image->height;
*/
	
};

void GateDetector::publishLostEventBuoy(Color::ColorType color)
{
    BuoyEventPtr event(new BuoyEvent());
    event->color = color;
    
    publish(EventType::GATE_LOST, event);
}

void GateDetector::publishFoundEventContour(contourblob contour, Color::ColorType color)
{
  static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    //static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    //static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

   BuoyEventPtr event(new BuoyEvent()); 

    double centerX = 0, centerY = 0;
    Detector::imageToAICoordinates(frame, contour.centerx, contour.centery,
                                   centerX, centerY);

    event->x = centerX;
    event->y = centerY;
    event->range = contour.width;
    event->azimuth = finalGate.gatepieces;
    event->elevation =finalGate.angle;
    event->color = color;
    event->touchingEdge = false;

    publish(EventType::GATE_FOUND, event);

}

void GateDetector::publishFoundEvent(foundLines::parallelLinesPairs finalPairs)
{
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

   GateEventPtr event = GateEventPtr(new GateEvent());
    
    double centerX = 0, centerY = 0;
    Detector::imageToAICoordinates(frame, finalPairs.center.x, finalPairs.center.y,
                                   centerX, centerY);

    int minX = finalPairs.line1_lower.x-finalPairs.width;
    int maxX = finalPairs.line2_lower.x+finalPairs.width;
    int minY = finalPairs.line1_lower.y-finalPairs.line1_height;
    int maxY = finalPairs.line1_upper.y-finalPairs.line1_height;;

    bool touchingEdge = false;
    if(minX == 0 || minY == 0 || maxX == xPixelWidth || maxY == yPixelHeight)
        touchingEdge = true;

    event->range = finalPairs.width;
    //event->color = color;
    //event->touchingEdge = touchingEdge;
	if (finalPairs.foundleft == 1)
	{
	    	event->leftX = finalPairs.line1_lower.x;
	    	event->leftY = finalPairs.line1_lower.y;
	    	event->haveLeft = true;
	     }
	if (finalPairs.foundright == 1)
	{
	    event->rightX = finalPairs.line2_lower.x;
	    event->rightY = finalPairs.line2_lower.y;
	    event->haveRight = true;
	}

      publish(EventType::GATE_FOUND, event);
};

void GateDetector::publishFoundEventBuoy(foundLines::parallelLinesPairs finalPairs, Color::ColorType color)
{
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

   BuoyEventPtr event(new BuoyEvent()); 

    double centerX = 0, centerY = 0;
    Detector::imageToAICoordinates(frame, finalPairs.center.x, finalPairs.center.y,
                                   centerX, centerY);

    int minX = finalPairs.line1_lower.x-finalPairs.width;
    int maxX = finalPairs.line2_lower.x+finalPairs.width;
    int minY = finalPairs.line1_lower.y-finalPairs.line1_height;
    int maxY = finalPairs.line1_upper.y-finalPairs.line1_height;;

    bool touchingEdge = false;
    if(minX == 0 || minY == 0 || maxX == xPixelWidth || maxY == yPixelHeight)
        touchingEdge = true;

      event->x = centerX;
    event->y = centerY;
    event->range = finalPairs.width;
    event->azimuth = math::Degree((-1) * (xFOV / 2) * centerX);
    event->elevation = math::Degree((yFOV / 2) * centerY);
    event->color = color;
    event->touchingEdge = touchingEdge;

    publish(EventType::GATE_FOUND, event);
};


int GateDetector::getmaxdiff(void)
{
	return(m_maxdiff);
};
} // namespace vision
} // namespace ram
