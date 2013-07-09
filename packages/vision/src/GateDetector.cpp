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
	propSet->addProperty(config, false, "Rmin",
                         "Rmin",
                         9, &m_redminH, 0, 255);
	propSet->addProperty(config, false, "Rmax",
                         "Rmax",
                         127, &m_redmaxH, 0, 255);
	propSet->addProperty(config, false, "Gmin",
                         "Gmin",
                         9, &m_greenminH, 0, 255);
	propSet->addProperty(config, false, "Gmax",
                         "Gmax",
                         76, &m_greenmaxH, 0, 255);
	propSet->addProperty(config, false, "Ymin",
                         "Ymin",
                         9, &m_yellowminH, 0, 255);
	propSet->addProperty(config, false, "Ymax",
                         "Ymax",
                         127, &m_yellowmaxH, 0, 255);
	propSet->addProperty(config, false, "Smin",
                         "Smin",
                         90, &m_minS, 0, 255);
	propSet->addProperty(config, false, "Smax",
                         "Smax",
                         255, &m_maxS, 0, 255);
	propSet->addProperty(config, false, "diff",
                         "diff",
                         100, &m_maxdiff, 0,300);
   	propSet->addProperty(config, false, "thresholdwithred",
                         "thresholdwithred",
                         true, &m_checkRed);

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
	//int yellow_minH = m_yellowminH;
	//int yellow_maxH = m_yellowmaxH;
	//I dont think theres a reason for these to be doubles
	double minS = (double)m_minS;
	double maxS = (double)m_maxS;

	printf("\n saving img from input");
	Mat img = input->asIplImage();
	img_whitebalance = WhiteBalance(img);
	printf("\n processImageColor");
	cv::Mat img_hsv(img_whitebalance.size(),CV_8UC1);

	printf("\n entering whitebalance");
	cvtColor(img_whitebalance,img_hsv,CV_BGR2HSV);
	printf("\n converting img_whitebalance");
	//use blob detection to find gate
	//find left and right red poles - vertical poles
	vector<Mat> hsv_planes;
	split(img_hsv,hsv_planes);

	//first take any value higher than max and converts it to 0
	//red is a special case because the hue value for red are 0-10 and 170-1980
	//same filter as the other cases followed by an invert

	cv::Mat img_red(img_whitebalance.size(),CV_8UC1);
	cv::Mat img_green(img_whitebalance.size(),CV_8UC1);
	cv::Mat img_blue(img_whitebalance.size(),CV_8UC1);

	cv::Mat erosion_dst(img_whitebalance.size(),CV_8UC1);
	Mat erosion_dst_red(img_whitebalance.size(),CV_8UC1);
	Mat erosion_dst_green(img_whitebalance.size(),CV_8UC1);
	Mat erosion_dst_blue(img_whitebalance.size(),CV_8UC1);

	//filter images
	blobfinder blob;
	//set up erode
	int erosion_type = 0; //morph rectangle type of erosion
	int erosion_size = 2;
	Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );
	//saturation filter
	img_blue =blob.SaturationFilter(hsv_planes,minS,maxS); //saturation filter
	printf("\n colorfilter");

  	/// Apply the erosion operation  	
  	erode(img_blue, erosion_dst_blue, element );

	printf("\nerode");
	//lets AND the blue and the green images
	if (m_checkRed == true)
	{
		img_red =blob.RedFilter(hsv_planes,red_minH,red_maxH);
		erode(img_red, erosion_dst_red, element );
		bitwise_and(erosion_dst_blue,erosion_dst_red, erosion_dst,noArray());
	}
	else
	{
		img_green =blob.OtherColorFilter(hsv_planes,green_minH,green_maxH);
		erode(img_green, erosion_dst_green, element );
		bitwise_and(erosion_dst_blue,erosion_dst_green, erosion_dst,noArray());
	}
	//imshow("AND",erosion_dst);

	return(erosion_dst);
}

void GateDetector::processImage(Image* input, Image* output)
{	
//KATE
	Mat imgprocess = processImageColor(input);
	//img_whitebalance = img;
	//Mat img = input->asIplImage();
	//imshow("input image", img);

	//imshow("process",imgprocess);
	//IplImage* tempImage=0;
	//img_whitebalance = WhiteBalance(img);

	foundLines::parallelLinesPairs final= gate.gateblob(imgprocess,img_whitebalance); //built in redfilter
	//Mat img_red = gate.hedgeblob(img_whitebalance);  //built in green filter

	//imshow("results",img_red);
	//img_gate = gate.rectangle(img_red, img_whitebalance);
	//cvtColor(img_whitebalance,img_whitebalance,CV_BGR2RGB);
	printf("\n done with gateblob");

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

	printf("\n outputting!");
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
