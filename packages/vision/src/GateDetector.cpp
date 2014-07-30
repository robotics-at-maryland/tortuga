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
//#include "vision/include/saliency.h"
#include "vision/include/GateDetectorKate.h"
#include "vision/include/saliency.h"


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
	m_gotreference = 0;
	m_previousobjectnumber = 0;
	m_foundobjectnumber = 0;
	//This frame will be a copy of the original rotated 90¼ counterclockwise.  
	//But only if the camera is on sideways, otherwise we do 640 by 480 like usual.
	//ie 640 by 480 if cameras on right (Or completely upsidedown!... sigh), else 480 by 640.
	gateFrame =cvCreateImage(cvSize(640,480),8,3);
	gateFrameRatios = cvCreateImage(cvGetSize(gateFrame),8,3);

	core::PropertySetPtr propSet(getPropertySet());

    // Color filter properties
//Kate edit: trying to edit the VisionToolV2 gui to allow sliders    

/*
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
                         3.5, &m_maxAspectRatio, 1.0,10.0);

	propSet->addProperty(config, false, "minAspectRatio",
                         "minAspectRatio",
                         0.25, &m_minAspectRatio, 0.0,10.0);

	propSet->addProperty(config, false, "minArea",
                         "minArea",
                         800, &m_minArea, 0,5000);

propSet->addProperty(config, false, "RefelctionDiff",
                         "ReflectionDiff",
                         10, &m_reflectiondiff, 0,500);

propSet->addProperty(config, false, "CannyLow",
                         "CannyLow",
                         50, &m_cannylow, 0,900);


propSet->addProperty(config, false, "CannyHigh",
                         "CannyHigh",
                         150, &m_cannyhigh, 0,5000);

propSet->addProperty(config, false, "ToptoIgnore",
                         "ToptoIgnore",
                         150, &m_minY, 0,500);
*/
propSet->addProperty(config, false, "Dilate",
                         "Dilate",
                       1, &m_dilatesize, 0,5);
propSet->addProperty(config, false, "Erode",
                         "Erode",
                       1, &m_erodesize, 0,5);

propSet->addProperty(config, false, "ArcLengthRatio",
                         "ArcLengthRatio",
                         1.0, &m_arclengthratio, 0.0,5.0);
propSet->addProperty(config, false, "RefImage",
                         "RefImage",
                         1, &m_refimage, 1,50);
propSet->addProperty(config, false, "AspectRatio",
                         "AspectRatio",
                         1.0, &m_aspectratio, 0.0,10.0);
propSet->addProperty(config, false, "BestMatch",
                         "BestMatch",
                         20.0, &m_bestMatch, 0.0,500.0);
propSet->addProperty(config, false, "AreaRatio",
                         "AreaRatio",
                         1.0, &m_arearatio, 0.0,10.0);
	propSet->addProperty(config, false, "minRefArea",
                         "minRefArea",
                         100, &m_minrefarea, 0,5000);

	propSet->addProperty(config, false, "maxRefArea",
                         "maxRefArea",
                         500, &m_maxrefarea, 0,5000);

	propSet->addProperty(config, false, "Canny",
                         "Canny",
                         70, &m_cannylow, 0,300);

	propSet->addProperty(config, false, "MorphSize",
                         "MorphSize",
                         2, &m_morphsize, 0,50);
	propSet->addProperty(config, false, "MorphType ",
                         "MorphType",
                         4, &m_morphtype, 0,6);
	propSet->addProperty(config, false, "AdaptiveThresholdWindow",
                         "AdaptiveThresholdWindow",
                         13, &m_adwindow, 1,60);
	propSet->addProperty(config, false, "CannyorAdaptive",
                         "CannyorAdaptive",
                         0, &m_cannyoradaptive, 0,1);
	propSet->addProperty(config, false, "SalientThreshold",
                         "SalientThreshold",
                         0.0, &m_threshvalue, 0.0,200.0);


	propSet->addProperty(config, false, "RefCanny",
                         "RefCanny",
                         100, &m_ref_cannylow, 0,300);

	propSet->addProperty(config, false, "ref_MorphSize",
                         "ref_MorphSize",
                         2, &m_ref_morphsize, 0,50);
	propSet->addProperty(config, false, "ref_MorphType ",
                         "ref_MorphType",
                         4, &m_ref_morphtype, 0,6);
	propSet->addProperty(config, false, "ref_AdaptiveThresholdWindow",
                         "ref_AdaptiveThresholdWindow",
                         13, &m_ref_adwindow, 1,60);
	propSet->addProperty(config, false, "ref_CannyorAdaptive",
                         "ref_CannyorAdaptive",
                         0, &m_ref_cannyoradaptive, 0,1);
	propSet->addProperty(config, false, "ref_SalientThreshold",
                         "ref_SalientThreshold",
                         0.0, &m_ref_threshvalue, 0.0,200.0);
	propSet->addProperty(config, false, "GotReference",
                         "GotReference",
                         0, &m_gotreference, 0,1);
	propSet->addProperty(config, false, "MaxXAllowance",
                         "MaxXAllowance",
                         70, &m_xallowance, 0,500);

	propSet->addProperty(config, false, "MaxYAllowance",
                         "MaxYAllowance",
                         50, &m_yallowance, 0,500);

	propSet->addProperty(config, false, "AreaAllowance",
                         "AreaAllowance",
                         100.0, &m_areaallowance, 0.0,500.0);

	propSet->addProperty(config, false, "MinNmberofFrames",
                         "MinNumberofFrames",
                         0, &m_minframes, 0,10);
	
	propSet->addProperty(config, false, "Smoothing",
                         "Smoothing",
                         0, &m_smooth, 0,10);
	propSet->addProperty(config,false,"SmoothParameter",
			"SmoothParamter",
			0, &m_smoothparam, 0, 500);
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
	printf("ASLKDJLASDJ?");
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
	printf("Starting ProcessIMageColor");

	//int red_maxH = m_redmaxH;
	//int red_minH = m_redminH;
	//int green_minH = m_greenminH;
	//int green_maxH = m_greenmaxH;
	//int minV = m_minV;
	//int maxV = m_maxV;
	//double minS = (double)m_minS;
	//double maxS = (double)m_maxS;

	//printf("\n saving img from input");
	Mat img = input->asIplImage();
	//Mat dst(img.size(),img.type());

	printf("whitebalance");
	img_whitebalance=img.clone();

	//img_whitebalance = WhiteBalance(img);
	int gotreference = m_gotreference;
	if (gotreference < 1)
	{
		printf("\n LOADING REFERENCE");
		int iframe = m_refimage;
		//m_ref_canny = GetReference(iframe);

		//find contours for reference image
		Mat ref_canny = GetReference(iframe);
		//vector<Vec4i> ref_hierarchy;
		//vector<vector<Point> > ref_contours;
		findContours(ref_canny, m_ref_contours, m_ref_hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
		//m_gotreference = true;
		printf("got reference contours");
	}

	printf("\n going for real image now");

	namedWindow( "ImgKInput", CV_WINDOW_AUTOSIZE );
	imshow( "ImgKInput",img_whitebalance);

	printf("line 259");
	Mat img_outline = GetOutline(img_whitebalance);

	namedWindow( "OutlineOutput", CV_WINDOW_AUTOSIZE );
	imshow( "OutlineOutput",img_outline);

	Mat hsv_whitebalance;
	cvtColor(img_whitebalance,hsv_whitebalance,CV_BGR2HSV);
	printf("\n refcontour = %d",m_gotrefcontour);

	if (m_gotreference > 0 && m_gotrefcontour > 0)
	{
		printf("\n Entering FindShape");
		FindShape(img_outline,img_whitebalance, m_ref_canny,hsv_whitebalance);
		printf("\n Done with Find Shape");
	}
	else
	{
		m_foundobjectnumber = 0;
		//m_refcontour = 0;
	}
	//check multiple frames
	printf("\n do I check previousframes = %d, and %d",m_previousobjectnumber,m_foundobjectnumber);

	if (m_previousobjectnumber > 0 && m_foundobjectnumber > 0  && m_refcontour > 0)
	{
		MultipleFrames(1,img_whitebalance);
	}
	
	for (int i=0;i<m_foundobjectnumber;i++)
	{
		m_previousObjects[i] = m_currentObjects[i];
	}

	m_previousobjectnumber= m_foundobjectnumber; 

	//cvtColor(img_whitebalance,dst,CV_RGB2BGR);

/*

	Mat temphsv;
	cvtColor(img_whitebalance,temphsv,CV_BGR2HSV);
	vector<Mat> hsv_planes;
	split(temphsv,hsv_planes);
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
			//imshow("Sat",img_saturation);		}
		if (minV != 0 || maxV != 255)	
		{
			img_luminance = blob.LuminanceFilter(hsv_planes,minV,maxV);
			bitwise_and(img_luminance,img_green,temp,noArray());
			//imshow("Luminance",img_luminance);
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
*/

	return(img);
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
	if (finalGate.area >minArea && finalGate.gatepieces==2)
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
	//printf("WTF?");
	//imshow("ImgProcess",imgprocess);

	//cv::Canny(bw,bw, m_cannylow, m_cannyhigh, 3);
	//imshow("Canny",bw);

	//FindContours(imgprocess);
	//foundLines::parallelLinesPairs final= gate.gateblob(imgprocess,img_whitebalance); //built in redfilter
	//Mat img_red = gate.hedgeblob(img_whitebalance);  //built in green filter

	//imshow("results",img_red);
	//img_gate = gate.rectangle(img_red, img_whitebalance);
	//cvtColor(img_whitebalance,img_whitebalance,CV_BGR2RGB);
	//printf("\n done with gateblob");
/*
	finalGate.area = 0;
	m_found = FALSE;
	if (finalGate.area > m_minArea)
	{
		m_found = TRUE;
		if (m_checkRed == true && finalGate.gatepieces == 2)
			publishFoundEventContour(finalGate,Color::RED);
		else
			publishFoundEventContour(finalGate,Color::GREEN);
	}
	else if (m_found == TRUE)
	{
		m_found = FALSE;
		if (m_checkRed == true && finalGate.gatepieces==2)
			publishLostEventBuoy(Color::RED);
		else
			publishLostEventBuoy(Color::GREEN);
	}
*/
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
	//cvtColor(img_whitebalance,img_whitebalance,CV_BGR2RGB);
	input->setData(img_whitebalance.data,false);
	frame->copyFrom(input);

printf("HATE");
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
	printf("No output");
	
        

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

Mat GateDetector::GetOutline(Mat img)
{
	Mat img_saliency;
	Mat temp_smooth;
	//This function gets the outline of an image
	//uses saliency, thresholding, canny or adaptive thresholding
printf("\n m_smoothparam= %d",m_smoothparam);

	if(m_smoothparam%2 < 1)
	{	
		m_smoothparam=m_smoothparam+1; //must be odd
		printf("\n m_smoothparam= %d",m_smoothparam);
	}
	if (m_smooth == 1)
	{
		bilateralFilter (img, temp_smooth, m_smoothparam, m_smoothparam*2, m_smoothparam/2 );
		img = temp_smooth.clone();
	}
	else if (m_smooth == 2)
	{
		medianBlur (img, temp_smooth, m_smoothparam);
		img = temp_smooth.clone();
	}
imshow( "SmoothInput",img);


	int lowThreshold= m_cannylow; 
	int morph_size = m_morphsize;
	int morph_type = m_morphtype;
	int erode_size = m_erodesize;
	int dilate_size = m_dilatesize;
	int cannyoradaptive= m_cannyoradaptive;
	int adwindow = m_adwindow;
	double threshvalue = m_threshvalue;
	//leave alone
	int ratio = 3;
	int kernel_size = 3;
	Mat element;
	//create blank image of the same size as others and in color

	//perform saliency
	printf("\n saliency on original");
	SaliencyFilter(img);
	
	printf("\n works?");
	if (m_saliency.data)
	{
		printf("VICTORY");
	//	namedWindow( "tempSal", CV_WINDOW_AUTOSIZE );
	//	imshow( "tempSal",m_saliency);

	}
	else
	{
		printf("\n No Saliency Data Line 1103");
	}


	//find edges: convert to gray, canny, and then morph_gradietn Morphology	
	Mat img_gray;
	Mat temp;
	printf(" line 1068");

	if (m_saliency.channels() == 3)
	{
		cvtColor(m_saliency,img_gray,CV_BGR2GRAY);
	}
	else if (img_saliency.channels() == 1)
	{
		printf("\n Expected color, got gray");
		img_gray = m_saliency;
	}
	else
	{
		printf("ERROR: WTF ON LINE 1123");
	}

	//namedWindow( "imgGray", CV_WINDOW_AUTOSIZE );
	//imshow( "imgGray",img_gray);

	printf(" line 1070");
	if (threshvalue > 0)
	{
		threshold(img_gray, img_gray, threshvalue,1,3);
	//	namedWindow( "Threshold", CV_WINDOW_AUTOSIZE );
	//	imshow( "Threshold",img_gray);
	//	temp.copyTo(img_gray);
	}

	printf("line 1136");
	if (cannyoradaptive == 1)
	{

		if (adwindow%2 <1)
			adwindow = adwindow +1;

	
		adaptiveThreshold(img_gray, img_gray, 255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,adwindow, 1 );
	//	namedWindow( "AdaptiveThreshold", CV_WINDOW_AUTOSIZE );
	//	imshow( "AdaptiveThreshold",img_gray);
	}
	else
	{
		printf(" canny on 1146");
		Canny(img_gray,img_gray, lowThreshold, lowThreshold*ratio, kernel_size );
	}

	if (morph_size > 0 && morph_type >=2)
	{	printf("\n morph is greater then 0 and type greater then 2");
		
		if(morph_type == 2 && cannyoradaptive == 0)
		{
			printf("\n Cannot close a canny image");
		}
		else
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
			morphologyEx(img_gray,temp,morph_type, element);
			if (!temp.empty())
			{
				temp.copyTo(img_gray); //perform a deep copy  but only if the image isn't empty
			}
			else
			{
				printf("\n ERROR: Empty image, so not performing morphology");
			}	
		}	
	}
	if (morph_type == 2)
	{
		if (dilate_size > 0)
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilate_size + 1, 2*dilate_size+1 ), Point( dilate_size, dilate_size ) );
			dilate(img_gray, temp, element);
			if (erode_size > 0)	
			{	
				element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erode_size + 1, 2*erode_size+1 ), Point( erode_size, erode_size ) );	
				erode(temp, img_gray, element);
			}
			else
			{
				if (temp.data)
				{
					temp.copyTo(img_gray); //perform a deep copy  but only if the image isn't empty
				}
				else
				{
					printf("\n ERROR: Empty image, so not performing morphology");
				}	
			}
		}
		else if (erode_size > 0 && cannyoradaptive == 0) //cannot erode without dilating! will result in empty image
		{
			printf("\n ERROR: Cannot erode without dilating first, will result in an empty image");
			//element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erode_size + 1, 2*erode_size+1 ), Point( erode_size, erode_size ) );
			//erode(ref_canny, temp, element);
			//temp.copyTo(ref_canny);
		}
		else if (erode_size > 0)
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erode_size + 1, 2*erode_size+1 ), Point( erode_size, erode_size ) );
			erode(img_gray, temp, element);
			temp.copyTo(img_gray);

		}
	}
	if (morph_type == 1 && cannyoradaptive == 1)
	{
		if (erode_size > 0)
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erode_size + 1, 2*erode_size+1 ), Point( erode_size, erode_size ) );	
			erode(img_gray, temp, element);
			if (dilate_size > 0)	
			{	
				element = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilate_size + 1, 2*dilate_size+1 ), Point( dilate_size, dilate_size ) );
				dilate(temp, img_gray, element);
			}
			else
			{
				if (temp.data)
				{
					temp.copyTo(img_gray); //perform a deep copy  but only if the image isn't empty
				}
				else
				{
					printf("\n ERROR: Empty image, so not performing morphology");
				}	
			}
		}
		else if (dilate_size > 0)
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilate_size + 1, 2*dilate_size+1 ), Point( dilate_size, dilate_size ) );
			dilate(img_gray, img_gray, element);
		}
	}

printf("Line 1180 done with getoutline");
return (img_gray);

}


Mat GateDetector::GetReference(int refnumber)
{
	printf("\n getReference");
printf("\n getReference");
	//Load Reference Image for shape matching
	//Input: 
	//	reference object number. So it'll load the correct reference

	//Output: Canny image from reference, also will save to variable m_refcontour
	//	m_refcontour is the counter number in the reference image to be used

	m_gotrefcontour=0;
	unsigned int maxRefArea =(unsigned int)m_maxrefarea;
	unsigned int minRefArea =(unsigned int)m_minrefarea;



	//Other variables: 
	int lowThreshold= m_ref_cannylow; //can be modified but shouldn't need to
	//leave alone
	int ratio = 3;
	int kernel_size = 3;
	int morph_size = m_ref_morphsize;
	int morph_type = m_ref_morphtype;
	int erode_size = m_ref_erodesize;
	int dilate_size = m_ref_dilatesize;
	int cannyoradaptive= m_ref_cannyoradaptive;
	int adwindow = m_ref_adwindow;
	double threshvalue = m_ref_threshvalue;
	Mat element;
	//create blank image of the same size as others and in color
	
	Mat ref_img;

	//get reference image name
	stringstream ss;
	ss << "../GateRef";
	ss << refnumber;
	ss <<".png";
	string str = ss.str();

	cout << "string = " << str ;
	printf("\n Loading Data");
	//Load image
	m_ref_img = imread(str);
	Mat originalImage = imread(str);
	if (!m_ref_img.data)
	{
		printf("Unable to find Reference Image");
		rectangle(m_ref_img, Point(50,100), Point(350,105), Scalar( 255, 255, 255 ),10,10, 0);
	}
	str = "";
	printf("\n loaded iamge?");

	//perform saliency
	printf("\n Saliency on Reference");
	SaliencyFilter(m_ref_img);
	ref_img = m_saliency;

	namedWindow( "ReferenceSaliency", CV_WINDOW_AUTOSIZE );
	imshow( "ReferenceSaliency",ref_img);


	//find edges: convert to gray, canny, and then morph_gradietn Morphology	
	Mat ref_canny;
	Mat temp;


	if (ref_img.channels() == 3)
		cvtColor(ref_img,ref_canny,CV_BGR2GRAY);
	else
	{
		ref_canny = ref_img;
	}

	//	namedWindow( "RefCanny2", CV_WINDOW_AUTOSIZE );
	//	imshow( "RefCanny2",ref_canny);



	if (threshvalue > 0)
	{
		threshold(ref_canny, temp, threshvalue,1,3);
	//	namedWindow( "Threshold", CV_WINDOW_AUTOSIZE );
	//	imshow( "Threshold",temp);
		temp.copyTo(ref_canny);
	}
	namedWindow( "Thresh", CV_WINDOW_AUTOSIZE );
	imshow( "Thresh",ref_canny);
	if (cannyoradaptive == 1)
	{
	
		if (adwindow%2 < 1) //needs to be odd
			adwindow = adwindow +1;
		adaptiveThreshold(ref_canny, ref_canny, 255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,adwindow, 1 );
	//	namedWindow( "AdaptiveThreshold", CV_WINDOW_AUTOSIZE );
	//	imshow( "AdaptiveThreshold",ref_canny);
	}
	else
	{
		Canny(ref_canny,ref_canny, lowThreshold, lowThreshold*ratio, kernel_size );
	}

	printf("\n morph_type = %d, morph_size = %d", morph_type, morph_size);
	if (morph_size > 0 && morph_type >=2)
	{
		
		if(morph_type == 2 && cannyoradaptive == 0)
		{
			printf("\n Cannot close a canny image");
		}
		else
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
			morphologyEx(ref_canny,temp,morph_type, element);
			if (!temp.empty())
			{
				temp.copyTo(ref_canny); //perform a deep copy  but only if the image isn't empty
			}
			else
			{
				printf("\n ERROR: Empty image, so not performing morphology");
			}	
		}	
	}
	if (morph_type == 2)
	{
		if (dilate_size > 0)
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilate_size + 1, 2*dilate_size+1 ), Point( dilate_size, dilate_size ) );
			dilate(ref_canny, temp, element);
			if (erode_size > 0)	
			{	
				element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erode_size + 1, 2*erode_size+1 ), Point( erode_size, erode_size ) );	
				erode(temp, ref_canny, element);
			}
			else
			{
				if (temp.data)
				{
					temp.copyTo(ref_canny); //perform a deep copy  but only if the image isn't empty
				}
				else
				{
					printf("\n ERROR: Empty image, so not performing morphology");
				}	
			}
		}
		else if (erode_size > 0 && cannyoradaptive == 0) //cannot erode without dilating! will result in empty image
		{
			printf("\n ERROR: Cannot erode without dilating first, will result in an empty image");
			//element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erode_size + 1, 2*erode_size+1 ), Point( erode_size, erode_size ) );
			//erode(ref_canny, temp, element);
			//temp.copyTo(ref_canny);
		}
		else if (erode_size > 0)
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erode_size + 1, 2*erode_size+1 ), Point( erode_size, erode_size ) );
			erode(ref_canny, temp, element);
			temp.copyTo(ref_canny);

		}
	}
	if (morph_type == 1 && cannyoradaptive == 1)
	{
		if (erode_size > 0)
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*erode_size + 1, 2*erode_size+1 ), Point( erode_size, erode_size ) );	
			erode(ref_canny, temp, element);
			if (dilate_size > 0)	
			{	
				element = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilate_size + 1, 2*dilate_size+1 ), Point( dilate_size, dilate_size ) );
				dilate(temp, ref_canny, element);
			}
			else
			{
				if (temp.data)
				{
					temp.copyTo(ref_canny); //perform a deep copy  but only if the image isn't empty
				}
				else
				{
					printf("\n ERROR: Empty image, so not performing morphology");
				}	
			}
		}
		else if (dilate_size > 0)
		{
			element = getStructuringElement( MORPH_ELLIPSE, Size( 2*dilate_size + 1, 2*dilate_size+1 ), Point( dilate_size, dilate_size ) );
			dilate(ref_canny, ref_canny, element);
		}
	}

	printf("\n\n done");

	printf("line 1359");
	m_ref_img = ref_canny.clone();
	ref_canny = GetOutline(m_ref_img);

	//ref_canny = m_saliency.clone();

	//find contours for reference image
	//vector<vector<Point> > ref_contours;
	vector<vector<Point> > final_contours;
	//vector<Vec4i> ref_hierarchy;
	findContours(ref_canny, m_ref_contours, m_ref_hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	printf("\n contours!");
	int j;
	int contournumber =0;
	for (unsigned int i = 0; i < m_ref_contours.size(); i++) {
		j = i;
		if (m_ref_contours[i].size() > minRefArea && m_ref_contours[i].size() < maxRefArea)
		{
			drawContours(originalImage, m_ref_contours, i,Scalar(0,255,255), 1, 8, vector<Vec4i>(), 0, Point() );
			contournumber = i;
			m_gotrefcontour = 1;
		}
	}



	namedWindow( "ReferenceCanny", CV_WINDOW_AUTOSIZE );
	imshow( "ReferenceCanny",ref_canny);

	namedWindow( "ReferenceContours", CV_WINDOW_AUTOSIZE );
	imshow( "ReferenceContours",originalImage);

	m_refcontour = contournumber;
	return (ref_canny);
};

void GateDetector::FindShape(Mat erosion_dst, Mat img, Mat ref_canny,Mat hsv_img)
{
	//Find shape based on edges
	//input: Mat erosion_Dst: saliency filtered image
	//	Mat img = original image
	//Output: Output image
	
	//Get variables from GUI/file

	double bestMatch_ref = m_bestMatch;
	double AR_final_ref = m_aspectratio;
	double area_final_ref = m_arearatio;
	double AL_final_ref = m_arclengthratio;
	//int iframe = m_refimage;

	//Load reference image, with int as the image type
	//Mat ref_canny = GetReference(iframe);

	//Get edges
/*s
	int lowThreshold= m_cannylow;
	int ratio = 3;
	int kernel_size = 3;
	int morph_size = m_morphsize;
	Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
	Canny(erosion_dst,erosion_dst, lowThreshold, lowThreshold*ratio, kernel_size );
	morphologyEx(erosion_dst,erosion_dst,MORPH_GRADIENT, element);
*/
	//now find contours
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours( erosion_dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	printf("got contours");
	m_contours = contours;
	//find contours for reference image
		
	//ref_canny = GetReference(iframe);
	vector<Vec4i> ref_hierarchy = m_ref_hierarchy;
	vector<vector<Point> > ref_contours = m_ref_contours;
	//findContours(ref_canny, ref_contours, ref_hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	//printf("got reference contours");

	//create parameters
	double area_ref, area, area_final;
	double rect_area_ref, rect_area;
	double ref_AR, AR, AR_final; //aspect ratio
	double AL, ref_AL, AL_final; //arch length
	double perimeter, perimeter_final, perimeter_ref;
	RotatedRect box, box_ref;
	double bestMatch;
	int j= 0;
	Mat mask;
	//set reference contour to the number found in the reference
	unsigned int i = (unsigned int)m_refcontour;
   vector<vector<Point> >hull( contours.size() );

	int foundobjectnumber = 0;
	foundObjectusingRef currentObjects[10];// = m_currentObjects[10];
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
		if (contours[idx].size() > 50)
		{
			if (ref_contours[i].size()>50)
			{
			//got contours to compare
			vector<Point>& a = ref_contours[i];
			vector<Point>& b = contours[idx];

			//std::cout << "a.size = " << a.size() << ", b.size = " << b.size();
				//get shape information
				bestMatch = matchShapes(a, b, CV_CONTOURS_MATCH_I3, 0); //parameter to compare shapes

				//draw rectangle about contours to get more information
				box = minAreaRect(contours[idx]);
				box_ref = minAreaRect(ref_contours[i]);
				rect_area = double(box.size.width*box.size.height);
				rect_area_ref = double(box_ref.size.width*box_ref.size.height);
				area_ref = contourArea(ref_contours[i]);
				area = contourArea(contours[idx]);

				if (box_ref.size.height < box_ref.size.width)
					ref_AR = double(box_ref.size.height/box_ref.size.width);
				else
					ref_AR = double(box_ref.size.width/box_ref.size.height);
				if (box.size.height < box.size.width)
					AR = double(box.size.height/box.size.width);
				else
					AR = double(box.size.width/box.size.height);
				if (double(area_ref/rect_area_ref) < double(area/rect_area))
					area_final = 1- double(area_ref/rect_area_ref) / double(area/rect_area) ;
				else
					area_final = 1- double(area/rect_area)/ double(area_ref/rect_area_ref) ;

				AL = arcLength(contours[idx],TRUE);
				ref_AL = arcLength(ref_contours[i],TRUE);

		
				if (AR < ref_AR)
					AR_final = 1- AR/ref_AR;
				else
					AR_final = 1- ref_AR/AR;
				if (double(AL/area) < double(ref_AL/area_ref))
					AL_final = 1- double(AL/area)/double(ref_AL/area_ref);
				else
					AL_final = 1- double(ref_AL/area_ref)/double(AL/area);

				if (AL < 2*(box.size.width+box.size.height))
					perimeter = 1-AL/((box.size.width+box.size.height)*2);
				else
					perimeter = 1-((box.size.width+box.size.height)*2)/AL;
				if (ref_AL < 2*(box_ref.size.width+box_ref.size.height))
					perimeter_ref = 1-ref_AL/((box_ref.size.width+box_ref.size.height)*2);
				else
					perimeter_ref = 1-((box_ref.size.width+box_ref.size.height)*2)/ref_AL;
				if ((perimeter) <(perimeter_ref))
					perimeter_final =1-double( double(perimeter) / double(perimeter_ref));
				else
					perimeter_final = 1-double( double(perimeter_ref) / double(perimeter));


				printf("bestmatch = %f Area_final = %f, AR_final = %f, AL_final = %f i = %d \n", bestMatch, area_final, AR_final, AL_final, i);
	
					//double bestMatch_ref, AR_final_ref, area_final_ref;
						j = i;
				Scalar color = Scalar(0,255,0);
	/*
					if (abs(perimeter_final) < .2)
					{		
						Scalar color = Scalar(0,0,255);
						//drawContours( img, contours, idx, color, 1, 8, vector<Vec4i>(), 0, Point() );
						drawContours( img, hull, idx, color, 15, 8, vector<Vec4i>(), 0, Point() );
			  		}
					if (abs(AL_final) < 0.1)
					{		
						Scalar color = Scalar(0,255,0);
						//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
						drawContours( img, contours, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );
						//drawContours( img, hull, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );
			  		}
					if (area_final < 0.2)
					{		
						Scalar color = Scalar(255,0,0);
						//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
						drawContours( img, contours, idx, color, 5, 8, vector<Vec4i>(), 0, Point() );
						//drawContours( img, hull, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );
			  		}
					if (AR_final < 0.2)
					{		
						Scalar color = Scalar(255,255,255);
						//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
						drawContours( img, contours, idx, color, 3, 8, vector<Vec4i>(), 0, Point() );
						//drawContours( img, hull, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );
			  		}
	*/
				if (bestMatch < bestMatch_ref && AR_final< AR_final_ref && area_final<area_final_ref  && AL_final < AL_final_ref)
				{		
					printf("bestmatch = %f, ref = %f, AR = %f, %f, area = %f, %f \n",bestMatch,bestMatch_ref, AR_final,AR_final_ref, area_final, area_final_ref);
					//Scalar color = Scalar(255,0,255);
					//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
					drawContours( img, contours, idx, color, 1, 8, vector<Vec4i>(), 0, Point() );
					//drawContours( img, hull, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );

					if(box.size.width < box.size.height)
						{currentObjects[foundobjectnumber].angle= box.angle+180;}
					else{
						currentObjects[foundobjectnumber].angle= box.angle+90;
					    }
					m_currentObjects[foundobjectnumber].centerx =box.center.x;
					m_currentObjects[foundobjectnumber].arclengthratio = AL_final;
					m_currentObjects[foundobjectnumber].bestmatch = bestMatch;
					m_currentObjects[foundobjectnumber].aspectratio = AR_final;
					m_currentObjects[foundobjectnumber].arearatio = area_final;
					m_currentObjects[foundobjectnumber].contourID = idx;
					//m_currentObjects[foundobjectnumber].contour = b;
					m_currentObjects[foundobjectnumber].rect_area = area;
					m_currentObjects[foundobjectnumber].rect_aspectratio = AR;	
					m_currentObjects[foundobjectnumber].arclength = AL;
	      				convexHull( Mat(contours[idx]), hull[idx], false ); 
	    				//drawContours(img, hull, idx, Scalar(255,0,0), 1, 8, vector<Vec4i>(), 0, Point() );		
					mask = Mat::zeros(erosion_dst.size(),CV_8UC1);
					drawContours(mask, contours, idx, Scalar(255), CV_FILLED, 8, vector<Vec4i>(), 0, Point() );		
					Scalar colorIndex = mean(hsv_img,mask);
					printf("\n colorIndex = %f,%f,%f,%f",colorIndex[0],colorIndex[1],colorIndex[2],colorIndex[3]);
					imshow("Mask",mask);				

					if (foundobjectnumber > 9)
					{
						printf("\n ERROR TOO MANY OBJECTS, CHANGE PARAMTERS");
						foundobjectnumber = 9;
					}					
					else
					{
						foundobjectnumber = foundobjectnumber+1;
					}
			  	}
			}//end if refcontours is large enough
		}; //end if contour is large enough
	};//end for
 	  /// Show in a window
 	  namedWindow( "Hull demo", CV_WINDOW_AUTOSIZE );
 	  imshow( "Hull demo", img);


m_foundobjectnumber = foundobjectnumber;
printf("\n I found %d OBJECTS",foundobjectnumber);
printf("\n aaaaaah");
printf("\n sigh");
printf("\n aaaaaah");
printf("\n sigh");

 	//cvtColor(erosion_dst,img,CV_GRAY2RGB);
	//return img;
}




void GateDetector::SaliencyFilter(Mat img)
{
	//Mat image_output(img.size(),img.type());
	//img.copyTo(image_output);
	if (!img.empty()) 
	{
		printf("141");
		//Mat image_output(img.size(),img.type());
		//Mat image_output;
		Mat img_gray; //(img.size(), CV_8U);
		printf("1639");
		printf("1640");
		cout<< "img.channels() = "<< img.channels();
		cout<< "img.type() = " <<img.type();

		if (img.channels()==  3)
			cvtColor(img,img_gray,CV_BGR2GRAY);
		else
		{
			printf("WTF in SaliencyFilter");
			img_gray = img;
		}

		IplImage* dstImg = cvCreateImage(cvSize(img_gray.cols, img_gray.rows), 8, 1);

		IplImage* srcImg= new IplImage(img_gray);

		//get saliency
		Saliency saliency;
		saliency.calcIntensityChannel(srcImg, dstImg);
		Mat img_salience(dstImg);

		m_saliency=img_salience.clone();
		cvReleaseImage(&dstImg);


	//	namedWindow( "Salience1", CV_WINDOW_AUTOSIZE );
	//	cvShowImage( "Salience1",srcImg);

		//namedWindow( "Salience", CV_WINDOW_AUTOSIZE );
		//imshow( "Salience",m_saliency);
		printf("175");
//		cvReleaseImage(&srcImg);

//		return image_output;
	}
	else
	{
		printf("No IMage!! for Salien");
//		return image_output;
		m_saliency=img.clone();

	}
}


void GateDetector::MultipleFrames(int display, Mat img_whitebalance)
{

	//checks if the found contour has been found before
	int xallowance = m_xallowance;
	int yallowance = m_yallowance;
	double areallowance = m_areaallowance;
	//CG can only move so much, by the xallowance and yallowance
	int minframes = m_minframes;
	//must be in minframes before its selected as a thing
	int distance;
	int xdiff, ydiff;
	double areadiff;
	double areachange;
	int closestmatch;
printf("\n Found Objects = %d, before = %d",m_foundobjectnumber,m_previousobjectnumber);

	printf("\n begin multipframes");
	
	for (int i=0;i<m_foundobjectnumber;i++)
	{
		distance  = 999;
		areachange = 9999;
		closestmatch = 11;
		for(int j = 0;j<m_previousobjectnumber;j++)
		{
			//check to see if its within xallwance and yallowance
			//also check to see if the parameters... haven't changed? or just go with it since they have to meet min criteria anyway
			xdiff = abs(m_currentObjects[i].centerx-m_previousObjects[j].centerx);
			ydiff = abs(m_currentObjects[i].centery-m_previousObjects[j].centery);
			areadiff = double(abs(double(double(m_currentObjects[i].rect_area - m_previousObjects[j].rect_area)/ m_previousObjects[i].rect_area)))*100;
			printf("areadiff = %f, xdiff = %d,ydiff = %d",areadiff,xdiff,ydiff);
			if (xdiff+ydiff < distance && areadiff < areachange)
			{
				distance = xdiff+ydiff;
				areachange = areadiff;
				closestmatch = j;
			}	
		}//end for j
		if (xdiff<xallowance && ydiff<yallowance && areadiff < areallowance && closestmatch < 11)
		{
			//same object as before so increase the frame number
			m_currentObjects[i].framesfound = m_previousObjects[closestmatch].framesfound+1;
		}
		else 
		{
			m_currentObjects[i].framesfound = 0;
		}
		if (display > 0 && m_currentObjects[i].framesfound > minframes)
		{
			//display images
			printf("Have Match, and is plotting");
			Scalar color = Scalar(255,0,255);
		if (m_currentObjects[i].framesfound < 0)
		{
			printf("ERROR WITH FRAMES FOUND");
			m_currentObjects[i].framesfound = 0;
		}
		if (m_currentObjects[i].framesfound > 250)
		{
			printf("\n So many frames found, error, reseting to 1");
			m_currentObjects[i].framesfound = 1;
		}
		
			//drawContours( img_whitebalance, m_contours, m_currentObjects[i].contourID, color,  m_currentObjects[i].framesfound, 8, Point() );
	drawContours( img_whitebalance,m_contours, m_currentObjects[i].contourID, color,3+m_currentObjects[i].framesfound, 8, vector<Vec4i>(), 0, Point() );				
		}
	printf(" Frames = %d",m_currentObjects[i].framesfound);
	}//end for i
	if (display > 0)
	{
		namedWindow( "Final", CV_WINDOW_AUTOSIZE );
		imshow( "Final",img_whitebalance);
	}
printf("\n leaving multiframes");
};//end void MultipleFrames




} // namespace vision
} // namespace ram
