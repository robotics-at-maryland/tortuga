/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/TargetDetector.cpp
 */

/*
Kate update
now uses contours to find target (which I believe is called the window in thhe past...)
so, it returns multiple events because there are multiple colors
*/

// STD Includes
#include <algorithm>
#include <cmath>
#include <sstream>

// Library Includes
#include "highgui.h"
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/TargetDetector.h"
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/include/ColorFilter.h"

#include "core/include/PropertySet.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

namespace ram {
namespace vision {

TargetDetector::TargetDetector(core::ConfigNode config,
                               core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_filter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_image(new OpenCVImage(640, 480)),
    m_found(false),
    m_targetCenterX(0),
    m_targetCenterY(0),
    m_squareNess(0),
    m_range(0),
    m_minGreenPixels(0),
    m_erodeIterations(0),
    m_dilateIterations(0),
    m_maxAspectRatio(0),
    m_topRemovePercentage(0),
    m_bottomRemovePercentage(0)
{
    init(config);
}
    
void TargetDetector::init(core::ConfigNode config)
{
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    // Hack properties to deal noise from the surface of the water
    propSet->addProperty(config, false, "topRemovePercentage",
                         "% of the screen from the top to be blacked out",
                         0.0, &m_topRemovePercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "bottomRemovePercentage",
                         "% of the screen from the bottom to be blacked out",
                         0.0, &m_bottomRemovePercentage, 0.0, 1.0);

    // Standard tuning properties
    propSet->addProperty(config, false, "maxAspectRatio",
                         "How \"skinny\" a blob can be",
                         4.0, &m_maxAspectRatio, 1.0, 10.0);

    propSet->addProperty(config, false, "minAspectRatio",
                         "How \"fat\" a blob can be",
                         0.8, &m_minAspectRatio, 0.0, 1.0);

    
    propSet->addProperty(config, false, "minGreenPixels",
                         "The minimum pixel count of the green target blob",
                         500, &m_minGreenPixels, 0, 50000);

    propSet->addProperty(config, false, "erodeIterations",
                         "How many times to erode the filtered image",
                         0, &m_erodeIterations, 0, 10);

    propSet->addProperty(config, false, "dilateIterations",
                         "How many times to dilate the filtered image",
                         0, &m_dilateIterations, 0, 10);

    // Color filter properties
    m_filter->addPropertiesToSet(propSet, &config,
                                 "Blue", "L*",
                                 "Yellow", "Blue Chrominance",
                                 "Green", "Red Chrominance",
                                 145, 255,  // L defaults
                                 0, 44,  // U defaults (0, 75)
                                 27, 63); // V defaults (137, 181)

//Kate edit: trying to edit the VisionToolV2 gui to allow sliders    
	propSet->addProperty(config, false, "Min H Red Value",
                         "min",
                         9, &m_redminH, 0, 255);
	propSet->addProperty(config, false, "Max H Red Value",
                         "max",
                         127, &m_redmaxH, 0, 255);

    /// TODO: add a found pixel drop off

    // Make sure the configuration is valid
    //propSet->verifyConfig(config, true);
}
    
TargetDetector::~TargetDetector()
{
    delete m_image;
}

bool TargetDetector::found()
{
    return m_found;
}

double TargetDetector::getX()
{
    return m_targetCenterX;
}

double TargetDetector::getY()
{
    return m_targetCenterY;
}

void TargetDetector::setTopRemovePercentage(double percent)
{
    m_topRemovePercentage = percent;
}

void TargetDetector::setBottomRemovePercentage(double percent)
{
    m_bottomRemovePercentage = percent;
}
 
//Kate added July 1 2013

void TargetDetector::processColorImage(Image* input, Image* output)
{

	 // Remove top and bottom chunks if desired
	//this is only really used to keep the visionTest from failing
	    if (m_topRemovePercentage != 0)
	    {
		int linesToRemove = (int)(m_topRemovePercentage * m_image->getHeight());
		size_t bytesToBlack = linesToRemove * m_image->getWidth() * 3;
		memset(m_image->getData(), 0, bytesToBlack);
	    }

	    if (m_bottomRemovePercentage != 0)
	    {
		int linesToRemove = 
		    (int)(m_bottomRemovePercentage * m_image->getHeight());
		size_t bytesToBlack = linesToRemove * m_image->getWidth() * 3;
		int startIdx = 
		    m_image->getWidth() * m_image->getHeight() * 3 - bytesToBlack;
		memset(&(m_image->getData()[startIdx]), 0, bytesToBlack);
	    }

	//Step 1: color filter based on Vision tool
	//want to filter for red, blue, green and yellow
	//for blue we're going to use the saturation channel

	//saturation finds the entire rectangle
	//so, we look for blobs in each filter, and then make sure the red,yellow, and green are within the blue
	//thats our verification, and how we'll get the blue blob
	
	//We need to get the holes, so we do an invert-saturation filter on the area found by the blobs
	//based on the color region, they're in and the relative areas, we should be able to get the big and small holes	
	double givenAspectRatio = 1.0;
	int red_maxH = m_redmaxH;
	int red_minH = m_redminH;
	int green_maxH = m_filter->getChannel3High();
	int green_minH = m_filter->getChannel3Low();
	int yellow_maxH = m_filter->getChannel2High();
	int yellow_minH = m_filter->getChannel2Low();
	double blue_maxS = m_filter->getChannel1High();
	double blue_minS = m_filter->getChannel1Low();

	Mat img = input->asIplImage();
	Mat img_hsv;

	img_whitebalance = WhiteBalance(img);
	cvtColor(img_whitebalance,img_hsv,CV_BGR2HSV);
		
	//use blob detection to find gate
	//find left and right red poles - vertical poles
	vector<Mat> hsv_planes;
	split(img_hsv,hsv_planes);

	//first take any value higher than max and converts it to 0
	//red is a special case because the hue value for red are 0-10 and 170-1980
	//same filter as the other cases followed by an invert
	blobfinder blob;
	Mat img_green =blob.OtherColorFilter(hsv_planes,green_minH,green_maxH);
	Mat img_yellow =blob.OtherColorFilter(hsv_planes,yellow_minH,yellow_maxH);
	Mat img_red =blob.RedFilter(hsv_planes,red_minH,red_maxH);
	Mat img_blue =blob.SaturationFilter(hsv_planes,blue_minS,blue_maxS);
	//imshow("green",img_green);
	//imshow("yellow",img_yellow);
	//imshow("red",img_red);
	//imshow("blue",img_blue);

	//For attempting to use with canny
	int erosion_type = 0; //morph rectangle type of erosion
	int erosion_size = 2;
	Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  	/// Apply the erosion operation
	Mat erosion_dst_red, erosion_dst_green, erosion_dst_blue, erosion_dst_yellow;
  	erode(img_red, erosion_dst_red, element );
	//get Blobs
	targetPanel squareRed = getSquareBlob(erosion_dst_red,img_whitebalance);

  	erode(img_green, erosion_dst_green, element );
  	//get Blobs
	targetPanel squareGreen = getSquareBlob(erosion_dst_green,img_whitebalance);

  	erode(img_yellow, erosion_dst_yellow, element );
  	//imshow( "Yellow", erosion_dst_yellow );
	//get Blobs
	targetPanel squareYellow = getSquareBlob(erosion_dst_yellow,img_whitebalance);

  	erode(img_blue, erosion_dst_blue, element );
  	//imshow( "Blue", erosion_dst );
	//get Blobs
	targetPanel squareBlue = getSquareBlob(erosion_dst_blue,img_whitebalance);

	//check to see if outer box meets aspet ratio
	Point2f vertices[4];
	double aspectRatio;
	//green event
	float foundAspectRatio = squareGreen.outline.size.height/squareGreen.outline.size.width;

	int minTargetSize = 30;
	bool greenFound = FALSE;
	bool redFound = FALSE;
	bool blueFound = FALSE;
	bool yellowFound = FALSE;

	if (abs(foundAspectRatio-givenAspectRatio) < .35)
	{ 
	 	 //valid panel
		greenFound = TRUE;
		squareGreen.outline.points(vertices);
		m_found = true;
		m_color = Color::GREEN;
	    		// Determine the corindates of the target
		Detector::imageToAICoordinates(input, 
		                               (int)squareGreen.outline.center.x,
		                               (int)squareGreen.outline.center.y,
		                               m_targetCenterX,
		                               m_targetCenterY);

		// Determine range
		m_range = 1.0 - (((double)squareGreen.outline.size.width) /
		                 ((double)squareGreen.outline.size.height));

		// Determine the squareness
		aspectRatio = ((double)squareGreen.outline.size.height)/((double)squareGreen.outline.size.width);
		if (aspectRatio < 1)
		    m_squareNess = 1.0;
		else
		    m_squareNess = 1.0/aspectRatio;


		//plot pretty results
		for (int i = 0; i < 4; i++)
	  		line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));
		
	 	if (squareGreen.targetLarge.size.width > minTargetSize && squareGreen.targetLarge.size.height > minTargetSize)
		{
			//valid
			Detector::imageToAICoordinates(input, 
		                              (int) squareGreen.targetLarge.center.x,
		                               (int)squareGreen.targetLarge.center.y,
		                               m_targetLargeCenterX,
		                               m_targetLargeCenterY);

			//plot pretty results
			ellipse(img_whitebalance, squareGreen.targetLarge, Scalar(0,250,0), 4, 8 );
		};
	  	if (squareGreen.targetSmall.size.width > minTargetSize && squareGreen.targetSmall.size.height > minTargetSize)
		{
			//valid
			Detector::imageToAICoordinates(input, 
		                               (int)squareGreen.targetSmall.center.x,
		                               (int)squareGreen.targetSmall.center.y,
		                               m_targetSmallCenterX,
		                               m_targetSmallCenterY);

			ellipse(img_whitebalance, squareGreen.targetSmall, Scalar(0,250,0), 2, 8 );

		};
		// Notify every that we have found the target
		publishFoundEvent();
	}
	else
	{
        	// Just lost the light so issue a lost event
    		publish(EventType::TARGET_LOST, core::EventPtr(new core::Event()));
		m_found = false;
	}


	//Red event
	foundAspectRatio = squareRed.outline.size.height/squareRed.outline.size.width;
	if (abs(foundAspectRatio-givenAspectRatio) < .35)
	{ 
	 	 //valid panel
		redFound = TRUE;
		squareRed.outline.points(vertices);
		m_found = true;
		m_color = Color::RED;
	    		// Determine the corindates of the target
		Detector::imageToAICoordinates(input, 
		                               (int)squareRed.outline.center.x,
		                               (int)squareRed.outline.center.y,
		                               m_targetCenterX,
		                               m_targetCenterY);

		// Determine range
		m_range = 1.0 - (((double)squareRed.outline.size.width) /
		                 ((double)squareRed.outline.size.height));

		// Determine the squareness
		aspectRatio = ((double)squareRed.outline.size.height)/((double)squareRed.outline.size.width);
		if (aspectRatio < 1)
		    m_squareNess = 1.0;
		else
		    m_squareNess = 1.0/aspectRatio;


		//plot pretty results
		for (int i = 0; i < 4; i++)
	  		line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,0,255));
		
	 	if (squareRed.targetLarge.size.width > minTargetSize && squareRed.targetLarge.size.height > minTargetSize)
		{
			//valid
			Detector::imageToAICoordinates(input, 
		                              (int) squareRed.targetLarge.center.x,
		                               (int)squareRed.targetLarge.center.y,
		                               m_targetLargeCenterX,
		                               m_targetLargeCenterY);

			//plot pretty results
			ellipse(img_whitebalance, squareRed.targetLarge, Scalar(0,0,255), 4, 8 );
		};
	  	if (squareRed.targetSmall.size.width > minTargetSize && squareRed.targetSmall.size.height > minTargetSize)
		{
			//valid
			Detector::imageToAICoordinates(input, 
		                               (int)squareRed.targetSmall.center.x,
		                               (int)squareRed.targetSmall.center.y,
		                               m_targetSmallCenterX,
		                               m_targetSmallCenterY);

			ellipse(img_whitebalance, squareRed.targetSmall, Scalar(0,0,255), 2, 8 );

		};
		// Notify every that we have found the target
		publishFoundEvent();
	}


	//Yellow event
	foundAspectRatio = squareYellow.outline.size.height/squareYellow.outline.size.width;
	if (abs(foundAspectRatio-givenAspectRatio) < .35)
	{ 
	 	 //valid panel
		yellowFound = TRUE;
		squareYellow.outline.points(vertices);
		m_found = true;
		m_color = Color::YELLOW;
	    		// Determine the corindates of the target
		Detector::imageToAICoordinates(input, 
		                               (int)squareYellow.outline.center.x,
		                               (int)squareYellow.outline.center.y,
		                               m_targetCenterX,
		                               m_targetCenterY);

		// Determine range
		m_range = 1.0 - (((double)squareYellow.outline.size.width) /
		                 ((double)squareYellow.outline.size.height));

		// Determine the squareness
		aspectRatio = ((double)squareYellow.outline.size.height)/((double)squareYellow.outline.size.width);
		if (aspectRatio < 1)
		    m_squareNess = 1.0;
		else
		    m_squareNess = 1.0/aspectRatio;


		//plot pretty results
		for (int i = 0; i < 4; i++)
	  		line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,255,255));
		
	 	if (squareYellow.targetLarge.size.width > minTargetSize && squareYellow.targetLarge.size.height > minTargetSize)
		{
			//valid
			Detector::imageToAICoordinates(input, 
		                              (int) squareYellow.targetLarge.center.x,
		                               (int)squareYellow.targetLarge.center.y,
		                               m_targetLargeCenterX,
		                               m_targetLargeCenterY);

			//plot pretty results
			ellipse(img_whitebalance, squareYellow.targetLarge, Scalar(0,250,255), 4, 8 );
		};
	  	if (squareYellow.targetSmall.size.width > minTargetSize && squareYellow.targetSmall.size.height > minTargetSize)
		{
			//valid
			Detector::imageToAICoordinates(input, 
		                               (int)squareYellow.targetSmall.center.x,
		                               (int)squareYellow.targetSmall.center.y,
		                               m_targetSmallCenterX,
		                               m_targetSmallCenterY);

			ellipse(img_whitebalance, squareYellow.targetSmall, Scalar(0,250,255), 2, 8 );

		};
		// Notify every that we have found the target
		publishFoundEvent();
	}

	//blule event
	foundAspectRatio = squareBlue.outline.size.height/squareBlue.outline.size.width;
	if (abs(foundAspectRatio-givenAspectRatio) < .35)
	{ 
	 	 //valid panel
		blueFound = TRUE;
		squareBlue.outline.points(vertices);
		m_found = true;
		m_color = Color::BLUE;
	    		// Determine the corindates of the target
		Detector::imageToAICoordinates(input, 
		                               (int)squareBlue.outline.center.x,
		                               (int)squareBlue.outline.center.y,
		                               m_targetCenterX,
		                               m_targetCenterY);

		// Determine range
		m_range = 1.0 - (((double)squareBlue.outline.size.width) /
		                 ((double)squareBlue.outline.size.height));

		// Determine the squareness
		aspectRatio = ((double)squareBlue.outline.size.height)/((double)squareBlue.outline.size.width);
		if (aspectRatio < 1)
		    m_squareNess = 1.0;
		else
		    m_squareNess = 1.0/aspectRatio;


		//plot pretty results
		for (int i = 0; i < 4; i++)
	  		line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(255,0,0));
		
	 	if (squareBlue.targetLarge.size.width > minTargetSize && squareBlue.targetLarge.size.height > minTargetSize)
		{
			//valid
			Detector::imageToAICoordinates(input, 
		                              (int) squareBlue.targetLarge.center.x,
		                               (int)squareBlue.targetLarge.center.y,
		                               m_targetLargeCenterX,
		                               m_targetLargeCenterY);

			//plot pretty results
			ellipse(img_whitebalance, squareBlue.targetLarge, Scalar(255,0,0), 4, 8 );
		};
	  	if (squareBlue.targetSmall.size.width > minTargetSize && squareBlue.targetSmall.size.height > minTargetSize)
		{
			//valid
			Detector::imageToAICoordinates(input, 
		                               (int)squareBlue.targetSmall.center.x,
		                               (int)squareBlue.targetSmall.center.y,
		                               m_targetSmallCenterX,
		                               m_targetSmallCenterY);

			ellipse(img_whitebalance, squareBlue.targetSmall, Scalar(255,0,0), 2, 8 );

		};
		// Notify every that we have found the target
		publishFoundEvent();
	}

    int numberofpanels = 0;	
    int tempx = 0;
    int tempy=0;
    int centerX =0;
    int centerY=0;
    //find center of the entire panel
    if (greenFound == TRUE || redFound == TRUE || blueFound ==TRUE || yellowFound == TRUE)
	{
	  m_found = 1;
	  //have found atleast one panel and therefore I can estimate  the center of the panel
	  
		if (greenFound == TRUE)
		{
			//green should be the upper right panel, so the center of the entire panel is the lower left corner
			numberofpanels = 1+numberofpanels;
			tempx = 100000;
			tempy = 0;
			squareGreen.outline.points(vertices);
			
			for (int i = 0; i < 4; i++)
	  		{
				if (vertices[i].x < tempx)
					tempx = vertices[i].x;
				if (vertices[i].y > tempy)
					tempy = vertices[i].y;
			}//end for 
			centerX = centerX+tempx;
			centerY = centerY+tempy;
			circle(img_whitebalance, Point(tempx, tempy),15,Scalar( 0, 255, 0),-1,8 );
		}
		if (redFound == TRUE)
		{
			//red is upper left panel, so I want to lower right vertex for the center of the entire panel
			numberofpanels = 1+numberofpanels;
			tempx = 0;
			tempy = 0;
			squareRed.outline.points(vertices);
			
			for (int i = 0; i < 4; i++)
	  		{
				if (vertices[i].x > tempx)
					tempx = vertices[i].x;
				if (vertices[i].y > tempy)
					tempy = vertices[i].y;
			}//end for 
			centerX = centerX+tempx;
			centerY = centerY+tempy;
			circle(img_whitebalance, Point(tempx, tempy),15,Scalar( 0, 0, 255),-1,8 );
		}
		if (yellowFound == TRUE)
		{
			//yellow is lower right, so I want the upper left
			numberofpanels = 1+numberofpanels;
			tempx = 100000;
			tempy = 100000;
			squareYellow.outline.points(vertices);
			
			for (int i = 0; i < 4; i++)
	  		{
				if (vertices[i].x < tempx)
					tempx = vertices[i].x;
				if (vertices[i].y < tempy)
					tempy = vertices[i].y;
			}//end for 
			centerX = centerX+tempx;
			centerY = centerY+tempy;
			circle(img_whitebalance, Point(tempx, tempy),15,Scalar( 0, 255, 255),-1,8 );
		}
		if (blueFound == TRUE)
		{
			//blue is the lower left, so I want upper right corner
			numberofpanels = 1+numberofpanels;
			tempx = 0;
			tempy = 100000;
			squareBlue.outline.points(vertices);
			
			for (int i = 0; i < 4; i++)
	  		{
				if (vertices[i].x > tempx)
					tempx = vertices[i].x;
				if (vertices[i].y < tempy)
					tempy = vertices[i].y;
			}//end for 
			centerX = centerX+tempx;
			centerY = centerY+tempy;
			circle(img_whitebalance, Point(tempx, tempy),15,Scalar( 255, 0, 0),-1,8 );
		}
		//printf("\n number of panels found = %d",numberofpanels);
		centerX = centerX/double(numberofpanels);
		centerY = centerY/double(numberofpanels);
		circle(img_whitebalance, Point(centerX, centerY),5,Scalar( 255, 255, 0),-1,8 );
		Detector::imageToAICoordinates(input, 
		                               centerX,
		                               centerY,
		                               m_targetCenterX,
		                               m_targetCenterY);
		m_color = Color::UNKNOWN;
		publishFoundEvent();
	} //end have found some of the panel
	
   cvtColor(img_whitebalance,img_whitebalance,CV_BGR2RGB);
   input->setData(img_whitebalance.data,false);
    if (output)
    {
        // Make the output exactly match the input
  	imshow( "Red", erosion_dst_red );
	imshow( "Yellow", erosion_dst_yellow );
	imshow( "Green", erosion_dst_green );
	imshow( "blue", erosion_dst_blue );

        output->copyFrom(input);

	}

	//imshow("final",img_whitebalance);
	
	//now to find the parts inside - which can be done with contours or by blob detection now that I have ROI
	//Note: still need to add in double checking here but now to do that? 
	//option 1: find contours inside each color
	//should be the second and third largest contour in each image, or the first and second largest in each ROI

	//now I have the outer contour and the two largest inner ones
	//can flag if a smallTarget or targetLarge is found based onthe size
	//how about the other panels?
	//if each panel meets aspect ratio, I'll declare it valid


};//end processColorImage


TargetDetector::targetPanel TargetDetector::getSquareBlob(Mat erosion_dst, Mat img_whitebalance)
{
	//perform blob detection for a square
	//for now only do blob detection based on area


	  vector<vector<Point> > contours;
	  vector<Vec4i> hierarchy;

	  /// Find contours
	  findContours(erosion_dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	  /// Draw contours
	  //Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	  //for( unsigned int i = 0; i< contours.size(); i++ )
	  //   {
	      // Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	  //    drawContours(img_whitebalance, contours, i, Scalar(50*i,0,0), 2, 8, hierarchy, 0, Point() );
	   // }


	//find contour with the largest area- by area I mean number of pixels
	double maxArea = 0;
	unsigned int maxContour;
	RotatedRect temp,maxtemp;
	//targetSmall and targetLarge are within the maxSize contour
	double area;
	for(unsigned int j=0; j<contours.size(); j++)
	{

	     //cout << "# of contour points: " << contours[i].size() << endl ;
		temp = minAreaRect(contours[j]); //finds the rectangle that will encompass all the points
		area = temp.size.width*temp.size.height;
		if (area > maxArea)
		{	maxContour = j;
			maxtemp = temp;
			maxArea = area;
		}
	};

	Point2f vertices[4];
	maxtemp.points(vertices);
	//given the vertices find the min and max X and min and maxY
	double minX= 90000;
	double maxX = 0;
	double minY= 90000;
	double maxY=0;	
	for (int i = 0; i < 4; i++)
	{
		//printf("\n verticle = (%f, %f)",vertices[i].x, vertices[i].y);
		if (vertices[i].x < minX)
			minX = vertices[i].x;
		if (vertices[i].x > maxX)
			maxX = vertices[i].x;
		if (vertices[i].y < minY)
			minY = vertices[i].y;
		if (vertices[i].y > maxY)
			maxY = vertices[i].y;
	};

	
	RotatedRect minEllipse;
        RotatedRect targetLarge;
	RotatedRect targetSmall;
	double targetLargeArea = 0;
	double targetSmallArea = 0;
	for(unsigned int j=0; j<contours.size(); j++)
	{
		if (j != maxContour && contours[j].size() > 4)
		{
	     		//make sure the center of the next contour is within the bouding area
			//have the center, height and width
			minEllipse = fitEllipse( Mat(contours[j])); //finds the rectangle that will encompass all the points
			//printf("\n center = (%f,%f), Xrange = %f,%f, Yrange = %f,%f",minEllipse.center.x,minEllipse.center.y,minX,maxX,minY,maxY);
			if (minEllipse.center.x > minX && minEllipse.center.x < maxX && minEllipse.center.y < maxY && minEllipse.center.y >minY)
			{
			//printf(" MADE IT!");
				//in the middle, therefore save
				//should also be roughly circular - width should be about equal to height
				if (minEllipse.size.height*minEllipse.size.width > targetLargeArea && abs(minEllipse.size.height-minEllipse.size.width) < 20)
				{	
					//before we go over the large one, save the previous large one to the small one
				        targetSmall = targetLarge;
					targetSmallArea = targetLargeArea;
					targetLarge = minEllipse;
					targetLargeArea = minEllipse.size.height*minEllipse.size.width;
				}
				else if (minEllipse.size.height*minEllipse.size.width > targetSmallArea)
				{
					targetSmall = minEllipse;
					targetSmallArea = minEllipse.size.height*minEllipse.size.width;
				}
			//ellipse(img_whitebalance, minEllipse, Scalar(0,0,255), 2, 8 );
			}//end if in the center
		} //end if j!=maxcounter
	};
//drawContours(img_whitebalance, contours, maxContour, Scalar(0,0,255), 2, 8, hierarchy, 0, Point() );

imshow("internal",img_whitebalance);
/*
//Display Purposes
	Point2f vertices[4];
	maxtemp.points(vertices);
	for (int i = 0; i < 4; i++)
	    line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));

	imshow("final",img_whitebalance); 
*/
targetPanel answer;
	answer.outline = maxtemp;
	answer.targetSmall = targetSmall;
	answer.targetLarge = targetLarge;
	return(answer);
};

   
void TargetDetector::processImage(Image* input, Image* output)
{
processColorImage(input,output);
/*
    // Ensure our working image is the same size
    if ((m_image->getWidth() != input->getWidth()) || 
        (m_image->getHeight() != input->getHeight()))
    {
        m_image->copyFrom(input);
    }

    // Copy to our working image and convert to YUV
    cvCvtColor(input->asIplImage(), m_image->asIplImage(), CV_BGR2Luv);

    // Filter for green
    filterForGreen(m_image);

    // Find all the green blobs
    m_blobDetector.setMinimumBlobSize(m_minGreenPixels);
    m_blobDetector.processImage(m_image);
    std::vector<BlobDetector::Blob> greenBlobs = m_blobDetector.getBlobs();

    // Filter the blobs to get only the valid candidate target blobs
    std::vector<BlobDetector::Blob> candidateBlobs;
    processGreenBlobs(greenBlobs, candidateBlobs);

    // Determine if we found the target
    BlobDetector::Blob targetBlob;
    if (candidateBlobs.size() > 0)
    {
        m_found = true;
        targetBlob = candidateBlobs[0];
    }
    else
    {
        // Just lost the light so issue a lost event
        if (m_found)
            publish(EventType::TARGET_LOST, core::EventPtr(new core::Event()));
        m_found = false;
    }


    if (m_found)
    {
        // Determine the corindates of the target
        Detector::imageToAICoordinates(m_image, 
                                       targetBlob.getCenterX(),
                                       targetBlob.getCenterY(),
                                       m_targetCenterX,
                                       m_targetCenterY);

        // Determine range
        m_range = 1.0 - (((double)targetBlob.getHeight()) /
                         ((double)m_image->getHeight()));

        // Determine the squareness
        double aspectRatio = targetBlob.getTrueAspectRatio();
        if (aspectRatio < 1)
            m_squareNess = 1.0;
        else
            m_squareNess = 1.0/aspectRatio;

        // Notify every that we have found the target
        publishFoundEvent();
    }
    

    // Do the debug display
    if (output)
    {
        // Make the output exactly match the input
        output->copyFrom(input);

        // Color all found pixels pink
        unsigned char* inData = m_image->getData();
        unsigned char* outData = output->getData();
        size_t numPixels = input->getHeight() * input->getWidth();
	
        for (size_t i = 0; i < numPixels; ++i)
        {
            if ((*inData))
            {
                *outData = 147; // B
                *(outData + 1) = 20; // G
                *(outData + 2) = 255; // R
            }
	    
            inData += 3;
            outData += 3;
        }

        // Draw all blobs
        BOOST_FOREACH(BlobDetector::Blob blob, candidateBlobs)
        {
            blob.draw(output, false);
            blob.drawStats(output);
        }

        //BOOST_FOREACH(BlobDetector::Blob blob, nullBlobs)
        //  {
        //  blob.draw(output, false, 255, 0, 0);
        //  blob.drawStats(output);
        //  }

        // Draw our target blob if we found it
        if (m_found)
        {
       	    targetBlob.draw(output, true);
            targetBlob.drawStats(output);
            
            std::stringstream ss;
            ss.precision(2);
            ss << "S:" << m_squareNess;
            targetBlob.drawTextUL(output, ss.str());
            
            std::stringstream ss2;
            ss2.precision(2);
            ss2 << "R:" << m_range;
            targetBlob.drawTextUL(output, ss2.str(), 0, 15);
        }
    }
*/
    /// TODO: consider detection stragies for side on blob
    /// TODO: do some blob merging as needed

    // Invert the images and find all the non-green blobs
    /*unsigned char* data = m_image->getData();
      size_t numPixels = m_image->getHeight() * m_image->getWidth();
    
      for (size_t i = 0; i < numPixels; ++i)
      {
      if (*data)
      {
      *data = 0; // B
      *(data + 1) = 0; // G
      *(data + 2) = 0; // R
      }	
      else
      {
      *data = 255; // B
      *(data + 1) = 255; // G
      *(data + 2) = 255; // R
      }
      data += 3;
      }

      m_blobDetector.setMinimumBlobSize(300);
      m_blobDetector.processImage(m_image);
      std::vector<BlobDetector::Blob> nullBlobs = m_blobDetector.getBlobs();*/
}

void TargetDetector::publishFoundEvent()
{
        TargetEventPtr event(new TargetEvent(
                                 m_targetCenterX,
                                 m_targetCenterY,
				 m_targetLargeCenterX,
				m_targetLargeCenterY,
				m_targetSmallCenterX,
				m_targetLargeCenterY,
                                 m_squareNess,
                                 m_range,
				  m_color));
        
        publish(EventType::TARGET_FOUND, event);
}

void TargetDetector::filterForGreen(Image* input)
{
    // Remove top and bottom chunks if desired
    if (m_topRemovePercentage != 0)
    {
        int linesToRemove = (int)(m_topRemovePercentage * m_image->getHeight());
        size_t bytesToBlack = linesToRemove * m_image->getWidth() * 3;
        memset(m_image->getData(), 0, bytesToBlack);
    }

    if (m_bottomRemovePercentage != 0)
    {
        int linesToRemove = 
            (int)(m_bottomRemovePercentage * m_image->getHeight());
        size_t bytesToBlack = linesToRemove * m_image->getWidth() * 3;
        int startIdx = 
            m_image->getWidth() * m_image->getHeight() * 3 - bytesToBlack;
        memset(&(m_image->getData()[startIdx]), 0, bytesToBlack);
    }

    // Filter the image so all green is white, and everything else is black
    m_filter->filterImage(m_image);

    if (m_erodeIterations)
    {
        cvErode(m_image->asIplImage(), m_image->asIplImage(), 0, 
                m_erodeIterations);
    }
    if (m_dilateIterations)
    {
        cvDilate(m_image->asIplImage(), m_image->asIplImage(), 0, 
                 m_dilateIterations);
    }

}
    
bool TargetDetector::processGreenBlobs(const BlobDetector::BlobList& blobs,
                                       BlobDetector::BlobList& outBlobs)
{
    bool foundBlobs = false;
    
    //Assumed sorted biggest to smallest by BlobDetector.
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Determine if we have a valid green blob
        double aspectRatio = blob.getTrueAspectRatio();
        bool aspectRatioSmallEnough = aspectRatio < m_maxAspectRatio;
        bool aspectRatioBigEnough = m_minAspectRatio < aspectRatio;
        if (aspectRatioSmallEnough && aspectRatioBigEnough)
        {
            outBlobs.push_back(blob);
            foundBlobs = true;
        }
    }

    return foundBlobs;
}
    
    
} // namespace vision
} // namespace ram
