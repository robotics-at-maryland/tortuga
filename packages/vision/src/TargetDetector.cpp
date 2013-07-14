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
#include <log4cpp/Category.hh>

// Project Includes
#include "vision/include/TargetDetector.h"
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/Utility.h"

#include "core/include/PropertySet.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

namespace ram {
namespace vision {

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("TargetDetector"));

    
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
                         1.0, &m_maxAspectRatio, 1.0, 10.0);

    propSet->addProperty(config, false, "minAspectRatio",
                         "How \"fat\" a blob can be",
                         0.3, &m_minAspectRatio, 0.0, 1.0);

    
    propSet->addProperty(config, false, "minGreenPixels",
                         "The minimum pixel count of the green target blob",
                         500, &m_minGreenPixels, 0, 50000);

    propSet->addProperty(config, false, "erodeIterations",
                         "How many times to erode the filtered image",
                         2, &m_erodeIterations, 0, 10);

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
	m_greenFound = FALSE;
	m_redFound = FALSE;
	m_blueFound = FALSE;
	m_yellowFound = FALSE;


    propSet->addProperty(config, false, "MinSize",
                         "MinSize",
                         6, &m_minSize, 6, 500); //must be atleast 5

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
/*
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
*/
	//Step 1: color filter based on Vision tool
	//want to filter for red, blue, green and yellow
	//for blue we're going to use the saturation channel

	//saturation finds the entire rectangle
	//so, we look for blobs in each filter, and then make sure the red,yellow, and green are within the blue
	//thats our verification, and how we'll get the blue blob
	
	//get min and max values for the color filters	
	int red_maxH = m_redmaxH;
	int red_minH = m_redminH;
	int green_maxH = m_filter->getChannel3High();
	int green_minH = m_filter->getChannel3Low();
	int yellow_maxH = m_filter->getChannel2High();
	int yellow_minH = m_filter->getChannel2Low();
	double blue_maxS = m_filter->getChannel1High();
	double blue_minS = m_filter->getChannel1Low();
	int erosion_size = m_erodeIterations;

	Mat img = input->asIplImage();
	Mat img_hsv;

	img_whitebalance = WhiteBalance(img);
	LOGGER.infoStream() << "\n Whitebalance complete row=: "<<img_whitebalance.rows<<" cols= "<<img_whitebalance.cols;

	cvtColor(img_whitebalance,img_hsv,CV_BGR2HSV);
		
	vector<Mat> hsv_planes;
	split(img_hsv,hsv_planes);

	///apply color filters, which are private members of the blobfind class
	blobfinder blob;
	Mat img_green =blob.OtherColorFilter(hsv_planes,green_minH,green_maxH);
	Mat img_yellow =blob.OtherColorFilter(hsv_planes,yellow_minH,yellow_maxH);
	Mat img_red =blob.RedFilter(hsv_planes,red_minH,red_maxH);
	Mat img_blue =blob.SaturationFilter(hsv_planes,blue_minS,blue_maxS);
	LOGGER.infoStream() << "\n ColorFiltering Complete";

	//For the erode function, which really helps clean things up
	int erosion_type = 0; //morph rectangle type of erosion
	Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  	/// Apply the erosion operation and then find the contours
	Mat erosion_dst_red, erosion_dst_green, erosion_dst_blue, erosion_dst_yellow;
  	erode(img_red, erosion_dst_red, element );

	LOGGER.infoStream() << "\n Finding Red";
	targetPanel squareRed = getSquareBlob(erosion_dst_red,img_whitebalance);

	LOGGER.infoStream() << "\n Finding Green";
  	erode(img_green, erosion_dst_green, element );
	targetPanel squareGreen = getSquareBlob(erosion_dst_green,img_whitebalance);

	LOGGER.infoStream() << "\n Finding Yellow";
  	erode(img_yellow, erosion_dst_yellow, element );
	targetPanel squareYellow = getSquareBlob(erosion_dst_yellow,img_whitebalance);

	LOGGER.infoStream() << "\n Finding Blue";
  	erode(img_blue, erosion_dst_blue, element );
	targetPanel squareBlue = getSquareBlob(erosion_dst_blue,img_whitebalance);

	//NOw that I have the contours, check to see if outer box meets desired aspect ratio
	Point2f vertices[4];
	float foundAspectRatio;
	//green event
	foundAspectRatio = squareGreen.outline.size.height/squareGreen.outline.size.width;
	if (foundAspectRatio< m_maxAspectRatio && foundAspectRatio > m_minAspectRatio)
	{ 
	 	 //valid panel
	        LOGGER.infoStream() << "\n Green Target Found";
		m_greenFound = TRUE;
		m_found = true;
		m_color = Color::GREEN;
		m_range = squareGreen.outline.size.width;
		setPublishData(squareGreen,input);
		publishFoundEvent();
		// Notify every that we have found the target
	}
	else if (m_greenFound == true)
	{
        	// Just lost the light so issue a lost event
	        LOGGER.infoStream() << "\n Green Target Lost";
    		publish(EventType::TARGET_LOST, core::EventPtr(new core::Event()));
		m_greenFound = false;
	}

	//Red event
	foundAspectRatio = squareRed.outline.size.height/squareRed.outline.size.width;
	if  (foundAspectRatio< m_maxAspectRatio && foundAspectRatio > m_minAspectRatio)
	{ 
	 	 //valid panel
	        LOGGER.infoStream() << "\n Red Target Found";
		m_redFound = TRUE;
		m_found = true;
		m_color = Color::RED;
		m_range = squareRed.outline.size.width;
		setPublishData(squareRed,input);
		publishFoundEvent();
		// Notify every that we have found the target
	}
	else if (m_redFound == true)
	{
        	// Just lost the light so issue a lost event
	        LOGGER.infoStream() << "\n Red Target Found";
    		publish(EventType::TARGET_LOST, core::EventPtr(new core::Event()));
		m_redFound = false;
	}
	//yellow event
	foundAspectRatio = squareYellow.outline.size.height/squareYellow.outline.size.width;
	if  (foundAspectRatio< m_maxAspectRatio && foundAspectRatio > m_minAspectRatio)
	{ 
	 	 //valid panel
	        LOGGER.infoStream() << "\n Yellow Target Found";
		m_yellowFound = TRUE;
		m_found = true;
		m_color = Color::YELLOW;
		setPublishData(squareYellow,input);
		m_range = squareYellow.outline.size.width;
		publishFoundEvent();
		// Notify every that we have found the target
	}
	else if (m_yellowFound == true)
	{
	        LOGGER.infoStream() << "\n Yellow Target Found";
        	// Just lost the light so issue a lost event
    		publish(EventType::TARGET_LOST, core::EventPtr(new core::Event()));
		m_yellowFound = false;
	}
	//blue event
	foundAspectRatio = squareBlue.outline.size.height/squareBlue.outline.size.width;
	if  (foundAspectRatio< m_maxAspectRatio && foundAspectRatio > m_minAspectRatio)
	{ 
	 	 //valid panel
	        LOGGER.infoStream() << "\n BLUE Target Found";
		m_blueFound = TRUE;
		m_found = true;
		m_color = Color::BLUE;
		m_range = squareBlue.outline.size.width;
		setPublishData(squareBlue,input);
		publishFoundEvent();
		// Notify every that we have found the target
	}
	else if (m_blueFound == true)
	{
	        LOGGER.infoStream() << "\n Blue Target Found";
        	// Just lost the light so issue a lost event
    		publish(EventType::TARGET_LOST, core::EventPtr(new core::Event()));
		m_blueFound = false;
	}

	
    int numberofpanels = 0;	
    int tempx = 0;
    int tempy=0;
    int centerX =0;
    int centerY=0;
   double totalangle= 0;
    int mainpanelrange = 0;
    //find center of the entire panel
    if (m_greenFound == TRUE || m_redFound == TRUE || m_blueFound ==TRUE || m_yellowFound == TRUE)
	{
	        LOGGER.infoStream() << "\n\n Main Target Found";
	  m_found = 1;
	  mainpanelrange = 0;
	  //have found atleast one panel and therefore I can estimate  the center of the panel
	  
		if (m_greenFound == TRUE)
		{	
	        LOGGER.infoStream() << "\n Green Target Found";
			//green should be the upper right panel, so the center of the entire panel is the lower left corner
			numberofpanels = 1+numberofpanels;
			tempx = 100000;
			tempy = 0;
			squareGreen.outline.points(vertices);
			mainpanelrange = mainpanelrange+ squareGreen.outline.size.width;
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
			totalangle = totalangle+squareGreen.outline.angle;
		}
		if (m_redFound == TRUE)
		{

		        LOGGER.infoStream() << "\n Red Target Found";
			//red is upper left panel, so I want to lower right vertex for the center of the entire panel
			numberofpanels = 1+numberofpanels;
			tempx = 0;
			tempy = 0;
			squareRed.outline.points(vertices);
			mainpanelrange = mainpanelrange + squareRed.outline.size.width;

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
			totalangle = totalangle+squareRed.outline.angle;
		}
		if (m_yellowFound == TRUE)
		{

	      		  LOGGER.infoStream() << "\n Yellow Target Found";
			//yellow is lower right, so I want the upper left
			numberofpanels = 1+numberofpanels;
			tempx = 100000;
			tempy = 100000;
			squareYellow.outline.points(vertices);
			mainpanelrange = mainpanelrange + squareYellow.outline.size.width;
			
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
			totalangle = totalangle+squareYellow.outline.angle;
		}
		if (m_blueFound == TRUE)
		{

	               LOGGER.infoStream() << "\n Blue Target Found";
			//blue is the lower left, so I want upper right corner
			numberofpanels = 1+numberofpanels;
			tempx = 0;
			tempy = 100000;
			squareBlue.outline.points(vertices);
			mainpanelrange = mainpanelrange + squareBlue.outline.size.width;
			
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
			totalangle = totalangle+squareBlue.outline.angle;
		}
		//printf("\n number of panels found = %d",numberofpanels);


	        LOGGER.infoStream() << "\nTotal Panels Found : " <<numberofpanels;
		centerX = centerX/double(numberofpanels);
		centerY = centerY/double(numberofpanels);
		circle(img_whitebalance, Point(centerX, centerY),5,Scalar( 255, 255, 0),-1,8 );

	        LOGGER.infoStream() << "\n center ("<<centerX<< " , " <<centerY;
		Detector::imageToAICoordinates(input, 
		                               centerX,
		                               centerY,
		                               m_targetCenterX,
		                               m_targetCenterY);

	        LOGGER.infoStream() << " Transformed to ("<<m_targetCenterX<<" , "<<m_targetCenterY;

		m_color = Color::UNKNOWN;
		mainpanelrange = ((mainpanelrange)/(numberofpanels))*2;
		m_range = mainpanelrange;
		m_smallflag = false;
		m_targetSmallCenterX= 0;
                m_targetSmallCenterY=0;
		m_rangesmall =0;
		m_largeflag = false;
		m_targetLargeCenterX= 0;
                m_targetLargeCenterY=0;
		m_rangelarge =0;
		m_angle = (totalangle/(double)numberofpanels);
		

		publishFoundEvent();
	} //end have found some of the panel
	
   cvtColor(img_whitebalance,img_whitebalance,CV_BGR2RGB);
   input->setData(img_whitebalance.data,false);
    if (output)
    {
        // Make the output exactly match the input
  	//imshow( "Red", erosion_dst_red );
	//imshow( "Yellow", erosion_dst_yellow );
	//imshow( "Green", erosion_dst_green );
	//imshow( "blue", erosion_dst_blue )

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


	  LOGGER.infoStream() << "\n Finding Contours, Entered getSquareBlob";
	
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

	LOGGER.infoStream() << "\n Contours FOund with maxSize = "<<maxContour <<" with area : "<<maxArea;
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
		if (j != maxContour && (int(contours[j].size()) > m_minSize))
		{
	     		//make sure the center of the next contour is within the bouding area
			//have the center, height and width
			minEllipse = fitEllipse( Mat(contours[j])); //finds the rectangle that will encompass all the points
			//printf("\n center = (%f,%f), Xrange = %f,%f, Yrange = %f,%f",minEllipse.center.x,minEllipse.center.y,minX,maxX,minY,maxY);
			if (minEllipse.center.x > minX && minEllipse.center.x < maxX && minEllipse.center.y < maxY && minEllipse.center.y >minY)
			{
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


	        LOGGER.infoStream() << "\n TargetLareArea = " <<targetLargeArea <<" Small Area "<<targetSmallArea;
//imshow("internal",img_whitebalance);
/*
//Display Purposes
	Point2f vertices[4];
	maxtemp.points(vertices);
	for (int i = 0; i < 4; i++)
	    line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));

	imshow("final",img_whitebalance); 
*/
	targetPanel answer;
	if (targetLargeArea > 0)
		answer.foundLarge = true;
	else
		answer.foundLarge = false;

	if (targetSmallArea > 0)
		answer.foundSmall = true;
	else
		answer.foundSmall = false;

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
LOGGER.infoStream() << "\n Publish Event: Center "<<m_targetCenterX <<", "<<m_targetCenterY<< 
				"\n large center: "<<m_targetLargeCenterX <<", "<<m_targetLargeCenterY<<
				"\n small center: "<<m_targetSmallCenterX <<", "<<m_targetSmallCenterY<<
				"\n Squareness: "<<m_squareNess <<", range: "<<m_range<<
				"\n Flags: Large: "<<m_largeflag <<", small: "<<m_smallflag<<
				"\n ranges: "<<m_rangelarge<<", small: "<<m_rangesmall<<
				"\n angle: "<<m_angle <<", Color"<<m_color<< " ";

        TargetEventPtr event(new TargetEvent(
                                 m_targetCenterX,
                                 m_targetCenterY,
				 m_targetLargeCenterX,
				m_targetLargeCenterY,
				m_targetSmallCenterX,
				m_targetSmallCenterY,
                                 m_squareNess,
                                 m_range,
				m_largeflag,
				m_smallflag,
				m_rangelarge,
				m_rangesmall,
				m_angle,
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
 

void TargetDetector::setPublishData(targetPanel square, Image* input)
{

LOGGER.infoStream() << "\n Setting Data to be Publish";

	Point2f vertices[4];
	int minTargetSize = 25;
	square.outline.points(vertices);
	// Determine the corindates of the target
LOGGER.infoStream() << "\n Center of main panel ("<<(int)square.outline.center.x<<" , "<<(int)square.outline.center.y <<") ";

	Detector::imageToAICoordinates(input, 
	              (int)square.outline.center.x,
	              (int)square.outline.center.y,
	              m_targetCenterX,
                      m_targetCenterY);

LOGGER.infoStream() << "Center converted to ("<<m_targetCenterX <<" , "<<m_targetCenterY <<") ";

	// Determine range
	//m_range = 1.0 - (((double)square.outline.size.width) /
	//                 ((double)square.outline.size.height));
	
	//m_range = square.outline.size.width;
	//m_range is assigned before now
	// Determine the squareness
	
	double aspectRatio = ((double)square.outline.size.height)/((double)square.outline.size.width);
	if (aspectRatio < 1)
	    m_squareNess = 1.0;
	else
	    m_squareNess = 1.0/aspectRatio;
	m_angle = square.outline.angle;
	//plot pretty results
	for (int i = 0; i < 4; i++)
  		line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));
	
 	if (square.targetLarge.size.width > minTargetSize && square.targetLarge.size.height > minTargetSize && square.foundLarge == true)
	{
		m_largeflag = true;
		//valid
		Detector::imageToAICoordinates(input, 
                         (int)square.targetLarge.center.x,
                         (int)square.targetLarge.center.y,
                          m_targetLargeCenterX,
                          m_targetLargeCenterY);
		m_rangelarge = square.targetLarge.size.width; 

		//plot pretty results
		ellipse(img_whitebalance, square.targetLarge, Scalar(0,250,0), 4, 8 );
	}
	else
	{
		m_largeflag = false;
		m_targetLargeCenterX= 0;
                m_targetLargeCenterY=0;
		m_rangelarge =0;
	}
  	if (square.targetSmall.size.width > minTargetSize && square.targetSmall.size.height > minTargetSize &&  square.foundSmall == true)
	{
		m_smallflag = true;
		//valid
		Detector::imageToAICoordinates(input, 
                         (int)square.targetSmall.center.x,
                         (int)square.targetSmall.center.y,
                         m_targetSmallCenterX,
                         m_targetSmallCenterY);
		
		m_rangesmall = square.targetSmall.size.width; 
		ellipse(img_whitebalance, square.targetSmall, Scalar(0,250,0), 2, 8 );
	}
	else
	{
		m_smallflag = false;
		m_targetSmallCenterX= 0;
                m_targetSmallCenterY=0;
		m_rangesmall =0;
	}
}   
    
} // namespace vision
} // namespace ram
