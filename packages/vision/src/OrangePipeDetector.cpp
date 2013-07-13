/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/OrangePipeDetector.cpp
 */

// STD Includes
#include <iostream>
#include <algorithm>
#include <string>

// Library Includes
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include <boost/foreach.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"
#include "vision/include/ImageFilter.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/TableColorFilter.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/BuoyDetectorKate.h"

#include "math/include/Vector2.h"

#include "core/include/PropertySet.h"

using namespace std;
using namespace cv;

namespace ram {
namespace vision {

//static bool pipeToCenterComparer(PipeDetector::Pipe b1, PipeDetector::Pipe b2)/
//{
//    return b1.distanceTo(0,0) < b2.distanceTo(0,0);/
//}
    
OrangePipeDetector::OrangePipeDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    PipeDetector(config, eventHub),
    m_centered(false),
    m_colorFilterLookupTable(false),
    m_lookupTablePath("")
{
    init(config);
}
    
void OrangePipeDetector::init(core::ConfigNode config)
{
    m_angle = math::Degree(0);
    m_lineX = 0;
    m_lineY = 0;
    m_found = false;
    m_filter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_useLUVFilter = false;
    
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "centeredLimit",
        "Max distance from the center for the pipe to be considered centered",
        0.1, &m_centeredLimit);

    propSet->addProperty(config, false, "minBrightness",
        "Minimum brighness for orange",
        100, &m_minBrightness, 0, 255);

    propSet->addProperty(config, false, "erodeIterations",
        "How many times to erode the filtered image",
        1, &m_erodeIterations);

    propSet->addProperty(config, false, "openIterations",
                         "How many times to perform the open morphological operation",
                         0, &m_openIterations);
                         

    propSet->addProperty(config, false, "rOverGMin",
       "Red/Green minimum ratio", 1.0, &m_rOverGMin, 0.0, 5.0);
    propSet->addProperty(config, false, "rOverGMax",
        "Red/Green maximum ratio", 2.0, &m_rOverGMax, 0.0, 5.0);
    propSet->addProperty(config, false, "bOverRMax",
        "Blue/Red maximum ratio",  0.4, &m_bOverRMax, 0.0, 5.0);

    propSet->addProperty(config, false, "MaxAspectRatio",
        "MaxAspectRatio",  2.0, &m_maxAspectRatio, 0.0, 10.0);


    // Newer Color filter properties
    propSet->addProperty(config, false, "useLUVFilter",
        "Use LUV based color filter",  true, &m_useLUVFilter);
    
    propSet->addProperty(config, false, "ColorFilterLookupTable",
        "True uses color filter lookup table", false,
        boost::bind(&OrangePipeDetector::getLookupTable, this),
        boost::bind(&OrangePipeDetector::setLookupTable, this, _1));

    m_filter->addPropertiesToSet(propSet, &config,
                                 "L", "L*",
                                 "U", "Blue Chrominance",
                                 "V", "Red Chrominance",
                                 0, 129,  // L defaults
                                 14, 200,  // U defaults
                                 126, 255); // V defaults
    m_framenumber = 0;
    // Make sure the configuration is valid
    //propSet->verifyConfig(config, true);
 m_redFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_redFilter->addPropertiesToSet(propSet, &config,
                                    "RedL", "Red Luminance",
                                    "RedC", "Red Chrominance",
                                    "RedH", "Red Hue",
                                    53, 148, 97, 255,9, 160);


}

bool OrangePipeDetector::getLookupTable()
{
    return m_colorFilterLookupTable;
}

void OrangePipeDetector::setLookupTable(bool lookupTable)
{
    if ( lookupTable ) {
        m_colorFilterLookupTable = true;

        // Initializing ColorFilterTable
        m_lookupTablePath =
            "/home/steven/ImageFilter/LookupTables/doubleRedpipeBlend1.25.serial";
        m_tableColorFilter = new TableColorFilter(m_lookupTablePath);
    } else {
        m_colorFilterLookupTable = false;
    }
}

void OrangePipeDetector::filterForOrangeOld(Image* image)
{
    if (m_found)
    {
        mask_orange(image->asIplImage(),true, m_minBrightness, true,
		    m_rOverGMin, 
		    m_rOverGMax, 
		    m_bOverRMax);
    }
    else
    {
        mask_orange(image->asIplImage(),true, m_minBrightness, false,
		    m_rOverGMin, 
		    m_rOverGMax, 
		    m_bOverRMax);
    }
}

void OrangePipeDetector::filterForOrangeNew(Image* image)
{
    // Filter the image so all orange is white, and everything else is black
    image->setPixelFormat(Image::PF_LUV_8);
    if ( m_colorFilterLookupTable )
        m_tableColorFilter->filterImage(image);
    else
        m_filter->filterImage(image);
}
    
bool OrangePipeDetector::found()
{
    return m_found;
}

double OrangePipeDetector::getX()
{
    return m_lineX;
}

double OrangePipeDetector::getY()
{
    return m_lineY;
}

math::Degree OrangePipeDetector::getAngle()
{
    return m_angle;
}

void OrangePipeDetector::setUseLUVFilter(bool value)
{
    m_useLUVFilter = value;
}   

OrangePipeDetector::~OrangePipeDetector()
{
    delete m_filter;

    if ( m_colorFilterLookupTable )
        delete m_tableColorFilter;
}



































void OrangePipeDetector::DetectorContours(Image* input)
{
/*
Kate Note:
This function is used to find the contour of the pipes
A color filter, in HSV space, is first used on a whitebalanced image
Then an erode function is used to clean up the results
The contours are found, and the maximum one with the correct aspect ratio is dubbed the pipe

makes use of find getsquareBlob() this is the part that takes the contours and pulls out the
pipe

*/

	//double givenAspectRatio = 1.0;
	m_framenumber = m_framenumber+1;
	int red_minH =m_redFilter->getChannel3Low();
	int red_maxH =m_redFilter->getChannel3High();

	int red_maxL =m_redFilter->getChannel2High();
	int red_minL =m_redFilter->getChannel2Low();

	int red_maxS =m_redFilter->getChannel1High();
	int red_minS =m_redFilter->getChannel1Low();

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

	//green blends in really well so we want to use a saturation filter as well
	Mat temp_red; //temperoary Mat used for merging channels

	Mat img_saturation(img_whitebalance.size(),CV_8UC1);
	Mat img_Luminance(img_whitebalance.size(),CV_8UC1);
	//For attempting to use with canny
	int erosion_type = 0; //morph rectangle type of erosion
	int erosion_size = m_erodeIterations;
	Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

	//green blends in really well so we want to use a saturation filter as well
	//green
	Mat  erode_dst_red;

	//red
	Mat img_red =blob.OtherColorFilter(hsv_planes,red_minH,red_maxH);	
	if (red_minS != 0 || red_maxS != 255)	
	{
		img_saturation = blob.SaturationFilter(hsv_planes,red_minS,red_maxS);
		bitwise_and(img_saturation,img_red,temp_red,noArray());
		img_red = temp_red;
		//imshow("Sat ReD",img_saturation);
	}
	if (red_minL != 0 || red_maxL != 255)	
	{
		Mat img_Luminance_red = blob.LuminanceFilter(hsv_planes,red_minL,red_maxL);
		bitwise_and(img_Luminance_red,img_red,temp_red,noArray());
		img_red = temp_red;
		//imshow("Luminance Red",img_Luminance);
	}	
  	erode(img_red, erode_dst_red, element);

  	//imshow("green",erode_dst_green);
	//imshow("yellow",erode_dst_yellow);
	imshow("red",erode_dst_red);

	//get Blobs 
	//needs to be able to handle multiple pipes
	foundpipe finalpipe;
	finalpipe= getSquareBlob(erode_dst_red);

	if (finalpipe.found == true)
	{	
		m_foundpipe1 = true;
		publishFoundEvent(finalpipe,1);
	}
	else if (m_foundpipe1 == true)
	{
		//lost event
		m_foundpipe1= false;
	//	publishLostEvent(1);
	}
	if (finalpipe.found2 == true)
	{	
		m_foundpipe2 = true;
		publishFoundEvent(finalpipe,2);
	}
	else if (m_foundpipe2== true)
	{
		//lost event
		m_foundpipe2 = false;
	//	publishLostEvent(2);
	}	

}


OrangePipeDetector::foundpipe OrangePipeDetector::getSquareBlob(Mat erosion_dst)
{
	//finds the maximum contour that meets aspectratio

	double aspectdifflimit = m_maxAspectRatio;
	double foundaspectdiff;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	foundpipe finalpipe;

	  /// Find contours
	findContours(erosion_dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	//find contour with the largest area- by area I mean number of pixels
	double maxArea = 0;
	double maxArea2 = 0;
	
	unsigned int maxContour, maxContour2;
	RotatedRect temp,maxtemp,maxtemp2;
	//targetSmall and targetLarge are within the maxSize contour
	double area;
	double desiredAspectRatio = 5.0;
	for(unsigned int j=0; j<contours.size(); j++)
	{

	        //cout << "# of contour points: " << contours[j].size() << endl ;
		temp = minAreaRect(contours[j]); //finds the rectangle that will encompass all the points
		area = temp.size.width*temp.size.height;
		foundaspectdiff = abs(temp.size.height/temp.size.width- desiredAspectRatio);
		//printf("\n foundaspectdiff = %f",foundaspectdiff);
		if (area > maxArea && foundaspectdiff < aspectdifflimit)
		{	
			maxArea2 =maxArea;
			maxContour2 = maxContour;
			maxtemp2 = maxtemp;
			maxContour = j;
			maxtemp = temp;
			maxArea = area;
		}
		else if (area > maxArea2)
		{
			maxContour2 = j;
			maxArea2 = area;
			maxtemp2 = temp;
		}
	};

	if (maxArea  > 5)
	{
		Point2f vertices[4];
		maxtemp.points(vertices);
	
		//allocate data
		finalpipe.found = true;
		finalpipe.id = 1;
		finalpipe.centerx = maxtemp.center.x;
		finalpipe.centery = maxtemp.center.y;
		finalpipe.range = maxtemp.size.width;
		finalpipe.angle = maxtemp.angle;

		//Display Purposes
		for (int i = 0; i < 4; i++)
		    line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,255,0),5);
		//imshow("final",img_whitebalance);
		drawContours(img_whitebalance, contours, maxContour, Scalar(255,0,0), 2, 8, hierarchy, 0, Point() ); 

	}
	else	
	{
		//printf("\n unable to find pipe");
		finalpipe.found = false;
		finalpipe.centerx =0;
		finalpipe.centery =0;
		finalpipe.range = 0;
		finalpipe.angle = 0;
	}


	if (maxArea2  > 5)
	{
		Point2f vertices[4];
		maxtemp2.points(vertices);
	
		//allocate data
		finalpipe.found2 = true;
		finalpipe.id2 = 2;
		finalpipe.centerx2 = maxtemp.center.x;
		finalpipe.centery2 = maxtemp.center.y;
		finalpipe.range2 = maxtemp.size.width;
		finalpipe.angle2 = maxtemp.angle;

		//Display Purposes
		for (int i = 0; i < 4; i++)
		    line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(255,255,0),5);
		drawContours(img_whitebalance, contours, maxContour2, Scalar(255,255,0), 2, 8, hierarchy, 0, Point() );
	}
	else	
	{
		//printf("\n unable to find pipe");
		finalpipe.found2 = false;
		finalpipe.id2 = 2;
		finalpipe.centerx2 =0;
		finalpipe.centery2 =0;
		finalpipe.range2 = 0;
		finalpipe.angle2 = 0;
	}
	 imshow("final",img_whitebalance); 
	return(finalpipe);
}

void OrangePipeDetector::publishFoundEvent(foundpipe pipe, int id)
{
    PipeEventPtr event(new PipeEvent()); 
    double centerX = 0, centerY = 0;
	 
	if (id == 1)
	{
	    Detector::imageToAICoordinates(frame, pipe.centerx, pipe.centery,
		                           centerX, centerY);
	    event->x = centerX;
	    event->y = centerY;
	    event->range = pipe.range;
	    event->angle = pipe.angle;
	}
	else
	{
	    Detector::imageToAICoordinates(frame, pipe.centerx2, pipe.centery2,
		                           centerX, centerY);
	    event->x = centerX;
	    event->y = centerY;
	    event->range = pipe.range2;
	    event->angle = pipe.angle2;
	}
    publish(EventType::PIPE_FOUND, event);
}

void OrangePipeDetector::publishLostEvent(int number)
{
    PipeEventPtr event(new PipeEvent());
    event->id = number;
    publish(EventType::PIPE_LOST, event);
}


void OrangePipeDetector::processImage(Image* input, Image* output)
{
	printf("Step 1");
	printf("ready?");
	DetectorContours(input);

	 if(output)
	    {
		cvtColor(img_whitebalance,img_whitebalance,CV_RGB2BGR);
		//imshow("final",img_whitebalance);
		//imshow("greenAND",erode_dst_green);
		//imshow("sat",img_saturation);
		//imshow("yellowerosion",erode_dst_yellow);
		//imshow("rederosion",erode_dst_red);

	       input->setData(img_whitebalance.data,false);
	      // frame->copyFrom(input);
	       //output->copyFrom(frame);

	   }



/*
    //Plan is:  Search out orange with a strict orange filter, as soon as we
    // see a good deal of orange use a less strict filter, and reduce the
    // amount we need to see. In theory this makes us follow the pipeline as
    // long as possible, but be reluctant to follow some arbitrary blob until
    // we know its of a large size and the right shade of orange.  If the
    // pipeline is found, the angle found by hough is reported.  
    
    // Mask orange takes frame, then alter image, then strictness (true=more

    input->setPixelFormat(Image::PF_BGR_8);
    
    // Filter the image for the proper color
    if (m_useLUVFilter)
        filterForOrangeNew(input);
    else
        filterForOrangeOld(input);

    // 3 x 3 default erosion element, default 3 iterations.
    cvErode(input->asIplImage(), input->asIplImage(), 0, m_erodeIterations);

    if(m_openIterations > 0)
    {
        cvErode(input->asIplImage(), input->asIplImage(), 0, m_openIterations);
        cvDilate(input->asIplImage(), input->asIplImage(), 0, m_openIterations);
    }

    // Debug display
    if (output)
        output->copyFrom(input);

    // Find all of our pipes
    PipeDetector::processImage(input, output);
    PipeDetector::PipeList pipes = getPipes();

    // Determine if we found any pipes
    bool found = pipes.size() > 0;
    
    // Determine which pipes (by id) were present last time, but aren't present
    // now.  Also build the set of current pipe IDs

    // Get the set of the Ids of the newest pipes
    std::set<int> newIds;
    BOOST_FOREACH(PipeDetector::Pipe pipe, pipes)
        newIds.insert(pipe.getId());

    // Get the Set Ids that were in the last frame but not the current
    std::vector<int> lostIds(m_lastPipeIds.size()); 
    std::vector<int>::iterator lostIdsEnd =
        std::set_difference(m_lastPipeIds.begin(), m_lastPipeIds.end(),
                            newIds.begin(), newIds.end(), lostIds.begin());
    lostIds.resize(lostIdsEnd -lostIds.begin());
    
    // Send out lost events for all the pipes we lost
    BOOST_FOREACH(int id, lostIds)
    {
        PipeEventPtr event(new PipeEvent(0, 0, 0, 0));
        event->id = id;
        publish(EventType::PIPE_DROPPED, event);
    }
    
    // Send out found events for all the pipes we currently see
    BOOST_FOREACH(PipeDetector::Pipe pipe, pipes)
    {
        PipeEventPtr event(new PipeEvent(0, 0, 0, 0));
        event->id = pipe.getId();
        event->x = pipe.getX();
        event->y = pipe.getY();
        event->angle = pipe.getAngle();
        publish(EventType::PIPE_FOUND, event);
    }

    // Record the current set of Ids
    m_lastPipeIds = newIds;

    // Send the lost event if we lost all the pipes
    if (!found && m_found)
    {
        // We have lost the pipe
        publish(EventType::PIPE_LOST, core::EventPtr(new core::Event()));
    }
    m_found = found;
    
    if (m_found)
    {
        // Record the center pipe information
        m_lineX = pipes[0].getX();
        m_lineY = pipes[0].getY();
        m_angle = pipes[0].getAngle();
        
        // Send out the centerted event
        std::sort(pipes.begin(), pipes.end(), pipeToCenterComparer);

        // Determine Centered
        math::Vector2 toCenter(pipes[0].getX(), pipes[0].getY());
        if (toCenter.normalise() < m_centeredLimit)
        {
            if(!m_centered)
            {
                PipeEventPtr event(new PipeEvent(0, 0, 0, 0));
                event->x = pipes[0].getX();
                event->y = pipes[0].getY();
                event->angle = pipes[0].getAngle();
                event->id = pipes[0].getId();

                m_centered = true;
                publish(EventType::PIPE_CENTERED, event);
            }
        }
        else
        {
            m_centered = false;
        }
    }
    
    //    if (output)
    //    {
    //        CvPoint center;
    //	center.x = linex;
    //	center.y = liney;
    //	cvCircle(output->asIplImage(), center, 5, CV_RGB(0, 0, 255), -1);
    //    }
*/
}

} // namespace vision
} // namespace ram
