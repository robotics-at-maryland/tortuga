/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/BinDetector.cpp
 */

// STD Includes
#include <iostream>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
//#include <cassert>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include <log4cpp/Category.hh>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/BinDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"
#include "vision/include/DetectorMaker.h"
#include "vision/include/SymbolDetector.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/BuoyDetectorKate.h"

#include "math/include/Vector2.h"

#include "core/include/Logging.h"
#include "core/include/PropertySet.h"

//static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Vision"));

// extra boarder pixels to include when extracting sub-image of the bin
static const int BIN_EXTRACT_BORDER = 16;

namespace ram {
namespace vision {

static bool binToCenterComparer(BinDetector::Bin b1, BinDetector::Bin b2)
{
    return b1.distanceTo(0,0) < b2.distanceTo(0,0);
}
    
BinDetector::Bin::Bin() :
    TrackedBlob(),
    m_symbol(Symbol::NONEFOUND)
{
}
    
BinDetector::Bin::Bin(BlobDetector::Blob blob, Image* source,
                      math::Degree angle, int id, Symbol::SymbolType symbol) :
    TrackedBlob(blob, source, angle, id),
    m_symbol(symbol)
{
}
    
void BinDetector::Bin::draw(Image* image, Image* redImage)
{
    IplImage* out = image->asIplImage();

    // Draw green rectangle around the blob
    CvPoint tl; // top left
    CvPoint tr; // top right
    CvPoint bl; // bottom left
    CvPoint br; // bottom right
    int minX, maxX, minY, maxY;

    tl.x = bl.x = minX = getMinX();
    tr.x = br.x = maxX = getMaxX();
    tl.y = tr.y = minY = getMinY();
    bl.y = br.y = maxY = getMaxY();

    cvLine(out, tl, tr, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tl, bl, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tr, br, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, bl, br, CV_RGB(0,255,0), 3, CV_AA, 0);

    // Now draw my id
    std::stringstream ssId;
    ssId << getId();
    Image::writeText(image, ssId.str(), minX, maxY);

    // Draw the fill percentage
    if (redImage)
    {
        std::stringstream ssFillPct;
        ssFillPct << "F%: " << BinDetector::getRedFillPercentage(*this, redImage);
        int fillPctXOffset = -30;
        int fillPctYOffset = 0;
        Image::writeText(image, ssFillPct.str(),
                         maxX + fillPctXOffset,
                         minY + fillPctYOffset);
    }

    // Draw the angle
    std::stringstream ssAngle;
    ssAngle << std::setprecision(1) << getAngle().valueDegrees();
    int angleXOffset = -30;
    int angleYOffset = -15;
    Image::writeText(image, ssAngle.str(),
                     maxX + angleXOffset,
                     minY + angleYOffset);

    // Now do the symbol
    Image::writeText(image, Symbol::symbolToText(m_symbol), bl.x, bl.y - 15);
}
        
BinDetector::BinDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_debug(0),
    m_blobDetector(config, eventHub),
    m_symbolDetector(SymbolDetectorPtr()),
    m_found(false),
    m_centered(false),
    m_runSymbolDetector(true),
    m_logSymbolImages(false),
    m_percents(0),
    m_whiteMaskedFrame(0),
    m_blackMaskedFrame(0),
    m_redMaskedFrame(0),
    m_extractBuffer(0),
    m_scratchBuffer1(0),
    m_scratchBuffer2(0),
    m_scratchBuffer3(0),
    m_whiteMaskMinimumPercent(0),
    m_whiteMaskMinimumIntensity(0),
    m_blackMaskMinimumPercent(0),
    m_blackMaskMaxTotalIntensity(0),
    m_redErodeIterations(0),
    m_redDilateIterations(0),
    m_redOpenIterations(0),
    m_redCloseIterations(0),
    m_redMinPercent(0),
    m_redMinRValue(0),
    m_redMaxGValue(0),
    m_redMaxBValue(0),
    m_blobMinBlackPixels(0),
    m_blobMinWhitePixels(0),
    m_blobMinRedPercent(0),
    m_binMaxAspectRatio(0),
    m_binMinFillPercentage(0),
    m_binMaxOverlaps(0),
    m_binSameThreshold(0),
    m_binLostFrames(0),
    m_binHoughPixelRes(0),
    m_binHoughThreshold(0),
    m_binHoughMinLineLength(0),
    m_binHoughMaxLineGap(0),
    m_binID(0),
    m_whiteFilter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_blackFilter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_redFilter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_frame(0)

{
    // Load all config based settings
    init(config);

    // Allocate images at default size
    allocateImages(640, 480);
    
    // By default we turn symbol detection on
    setSymbolDetectionOn(true);
}
    
    
BinDetector::~BinDetector()
{
    deleteImages();
}














void BinDetector::DetectorContours(Image* input)
{
/*
Kate Note:
This function is used to find the contour of the buoys
A color filter, in HSV space, is first used on a whitebalanced image
Then an erode function is used to clean up the results
The contours are found, and the maximum one with the correct aspect ratio is dubbed the buoy

makes use of find getsquareBlob() this is the part that takes the contours and pulls out the
buoy

*/

	cv::Mat img = input->asIplImage();
	img_whitebalance = WhiteBalance(img);
	int ksize = 15;
	imshow("hsv",img_whitebalance);
	medianBlur(img_whitebalance, img_whitebalance, ksize);
	imshow("blur",img_whitebalance);

	//double givenAspectRatio = 1.0;

	//int red_minH= m_redFilter->getChannel3Low();
	//int red_maxH= m_redFilter->getChannel3High();
	//int green_minH= m_greenFilter->getChannel3Low();
	//int green_maxH= m_greenFilter->getChannel3High();
	int minS = m_redFilter->getChannel1Low();
	int maxS = m_redFilter->getChannel1High();

	int minV = m_redFilter->getChannel2Low(); 
	int maxV = m_redFilter->getChannel2High();

	int erosion_size = m_redErodeIterations;
	//int dilate_size = m_redDilateIterations;

	//Initializae things

	blobfinder blob;
	cv::Mat img_hsv(img_whitebalance.size(),img_whitebalance.type());
	cv::Mat img_saturation(img_whitebalance.size(),CV_8UC1);
	//cv::Mat img_red(img_whitebalance.size(),CV_8UC1);
	//cv::Mat img_added(img_whitebalance.size(),CV_8UC1);
 	//cv::Mat erode_dst_red(img_whitebalance.size(),CV_8UC1);
	cv::Mat erode_dst_redL(img_whitebalance.size(),CV_8UC1);
	cv::Mat erode_dst_redS(img_whitebalance.size(),CV_8UC1);
	cv::Mat dilate_dst_red(img_whitebalance.size(),CV_8UC1);
	//cv::Mat img_filter(img_whitebalance.size(),img_whitebalance.type());


	//get image into useable format and apply whitebalance;
	cvtColor(img_whitebalance,img_hsv,CV_BGR2HSV);
		
	//use blob detection to find gate
	//find left and right red poles - vertical poles
	cv::vector<Mat> hsv_planes;
	cv::split(img_hsv,hsv_planes);

	//first take any value higher than max and converts it to 0
	//red is a special case because the hue value for red are 0-10 and 170-1980
	//same filter as the other cases followed by an invert
	Mat img_Luminance = blob.LuminanceFilter(hsv_planes,minV,maxV);
	img_saturation = blob.SaturationFilter(hsv_planes,minS,maxS);
//	img_red =blob.RedFilter(hsv_planes,red_minH,red_maxH);

	//For attempting to use with canny
	int erosion_type = 0; //morph rectangle type of erosion

	cv::Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );


//	cv::Mat dilate_element = getStructuringElement( erosion_type,
//                                       Size( 2*dilate_size + 1, 2*dilate_size+1 ),
//                                       Point( dilate_size, dilate_size ) );

  	/// Apply the erosion operation 
	//erode(img_red, erode_dst_red, element );
	erode(img_saturation, erode_dst_redS, element );
	erode(img_Luminance, erode_dst_redL, element );
	//bitwise_or(img_saturation,img_red,img_added,noArray());
  	//erode(img_added, erode_dst_red, element );
	//imshow("erode-red",erode_dst_red);
	//imshow("erode-sat",erode_dst_redS);
	//imshow("erode-lum",erode_dst_redL);

	//try dilate
	//dilate(erode_dst_red, dilate_dst_red, dilate_element );
	//imshow("dilate-red",dilate_dst_red);

	//dilate(erode_dst_red, dilate_dst_red, dilate_element );
	//imshow("dilate-red",dilate_dst_red);

	//dilate(erode_dst_redS, dilate_dst_red, dilate_element );
	//imshow("dilate-S",dilate_dst_red);

	//dilate(erode_dst_redL, dilate_dst_red, dilate_element );
	//imshow("dilate-V",dilate_dst_red);


	//merge the dilated V and S	
	bitwise_and(erode_dst_redS,erode_dst_redL, dilate_dst_red,noArray());

	//imshow("erode-L",erode_dst_redL);
	//imshow("Luminance",img_Luminance);
	//imshow("Red",img_red);
	//imshow("Or images",img_added);
	//imshow("sat1",img_saturation);
	//imshow("rederosion1",erode_dst_red);
	imshow("final erode",dilate_dst_red);

	//get Contours
	m_bin = getSquareBlob(dilate_dst_red);
};


BinDetector::bincontours BinDetector::getSquareBlob(Mat src)
{
	//finds the maximum contour that meets aspectratio
	double aspectratio = 1.0;
	double aspectratio_limit = 1.0;
	double aspectratio_diff;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	  /// Find contours
	findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	//find contour with the largest area- by area I mean number of pixels
	RotatedRect temp,maxtemp;
	double area;

	//find larget area where it should be about a square
	//outline of the main box
	//actually want the top 9 outlines
	//largest = all bins
	//next 4 largest - contour between black and white inside bins
	//next 4 largest - contour of the yellow bottom

	int numberoftrackedcontours = 9;
	bincontours bins[numberoftrackedcontours];

	//initialize to zero, just to verify everything is at zero
	for (int k=0;k<numberoftrackedcontours;k++)
	{
		bins[k].contournumber = 0;
		bins[k].area= 0;
		bins[k].found = false;
		bins[k].vertices[0].x=0;
		bins[k].vertices[0].y=0;
		bins[k].vertices[1].x=0;
		bins[k].vertices[1].y=0;
		bins[k].vertices[2].x=0;
		bins[k].vertices[2].y=0;
		bins[k].vertices[3].x=0;
		bins[k].vertices[3].y=0;

	}
	bool used; //used later on to when trying to find the top areas


	double minX,maxX,minY,maxY;
	int imageheight, imagewidth;
	imagewidth = img_whitebalance.cols;
	imageheight = img_whitebalance.rows;

	Point2f vertices[4];
//find the largest rectangular contour
//find the contours inside that one
	for(unsigned int j=0; j<contours.size(); j++)
	{
		if (contours[j].size()>5)
		{
			temp = minAreaRect(contours[j]); //finds the rectangle that will encompass all the points
			area = temp.size.width*temp.size.height;
			aspectratio_diff = abs((float(temp.size.height)/float(temp.size.width))- aspectratio);
			//printf("\n j = %d, countoursize = %d, area = %f, aspectratio_diff =%f, height = %f, width = %f",j,contours[j].size(),area,aspectratio_diff,temp.size.height,temp.size.width);
			drawContours(img_whitebalance, contours, j, Scalar(255,0,0), 2, 8, hierarchy, 0, Point() );
			if (area > bins[0].area && aspectratio_diff < aspectratio_limit)
			{
				//printf(" beating max");
				bins[0].contournumber = j;
				bins[0].area = area;
				bins[0].aspectratio_diff = aspectratio_diff;
				//get the min and max points
				temp.points(vertices);
				minX= 90000;
				maxX = 0;
				minY= 90000;
				maxY=0;	
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
					bins[0].vertices[i]= vertices[i];
				};
				if (maxX > imagewidth)
				{
					maxX = imagewidth;
				}
				if (maxY > imageheight)
				{
					maxY = imageheight;
				}
				if (minX < 0)
				{
					minX = 0;
				}
				if (minY < 0)
				{
					 minY = 0;
				}
				bins[0].maxX = maxX;
				bins[0].minX = minX;
				bins[0].maxY = maxY;
				bins[0].minY = minY;
				bins[0].found=true;
			}
		}//end if size
	};

	//printf("\n FINAL j = %d, countoursize = %d, area = %d, aspectratio_diff =%f",bins[0].contournumber,contours[bins[0].contournumber].size(),bins[0].area,bins[0].aspectratio_diff);

	for (int i = 0; i < 4; i++)
	{
		line(img_whitebalance, bins[0].vertices[i], bins[0].vertices[(i+1)%4], Scalar(255,0,255),8);
	}
	//given the vertices find the min and max X and min and maxY


	//have the largest one
	//now to find which ones are inside
	double minX2,minY2,maxX2,maxY2;
	int contourcounter =1;
	int k2;
	for(unsigned int j=0; j<contours.size(); j++)
	{
		if ((contours[j].size() >10) && ((int)j!=bins[0].contournumber) && (bins[0].found == true))
		{
			temp = minAreaRect(contours[j]); //finds the rectangle that will encompass all the points
			area = temp.size.width*temp.size.height;
			aspectratio_diff = abs((float(temp.size.height)/float(temp.size.width))- aspectratio);
			temp.points(vertices);
			minX2= 90000;
			maxX2 = 0;
			minY2= 90000;
			maxY2=0;	
			for (int i = 0; i < 4; i++)
			{
				//printf("\n verticle = (%f, %f)",vertices[i].x, vertices[i].y);
				if (vertices[i].x < minX2)
					minX2 = vertices[i].x;
				if (vertices[i].x > maxX2)
					maxX2 = vertices[i].x;
				if (vertices[i].y < minY2)
					minY2 = vertices[i].y;
				if (vertices[i].y > maxY2)
					maxY2 = vertices[i].y;
				
			};
			if (maxX2 > imagewidth)
					{
				maxX2 = imagewidth;
			}
			if (maxY2 > imageheight)
			{
				maxY2 = imageheight;
			}
			if (minX2 < 0)
			{
				minX2 = 0;
			}
			if (minY2 < 0)
			{
				 minY2 = 0;
			}

			if (((maxX2-minX2)>5)&&((maxY2-minY2) > 5)&&(aspectratio_diff < aspectratio_limit) && (minX2 >bins[0].minX) && (maxX2<bins[0].maxX) && (minY2>bins[0].minY) &&(maxY2<bins[0].maxY) && (contourcounter < (numberoftrackedcontours)))
			{
				used = false;
				for (int i =1;i<contourcounter;i++)
				{
				 //put insize, in order from largest to smallest
					if (area > bins[i].area && used == false)
					{
						//take the one that its greater than, i and move it down
						//need to move all ones down first
						k2 = contourcounter-i; //if contourcounter = 3 and i = 1, =>2, I want to take 1=new,2=old1,3=old2,4=old3
						if ((k2 < contourcounter+1) && (k2 >0))
						{
							bins[k2+1].area =bins[k2].area; 
						
						}
					}
				}

				//should really set this to find the largest 8 instead of just recording them
				bins[contourcounter].contournumber = j;
				bins[contourcounter].area = area;
				bins[contourcounter].aspectratio_diff = aspectratio_diff;
				bins[contourcounter].maxX = maxX2;
				bins[contourcounter].minX = minX2;
				bins[contourcounter].maxY = maxY2;
				bins[contourcounter].minY = minY2;
				bins[contourcounter].found = true;

				for (int i = 0; i < 4; i++)
				{
				   bins[contourcounter].vertices[i]= vertices[i];
				}
				contourcounter = contourcounter+1;
			}
		}//end if size
	};


	CvPoint point1,point2;

	for (int k=0;k<numberoftrackedcontours;k++)
	{
		if (bins[k].found==true)
		{
			point1.x = bins[k].maxX;
			point1.y = bins[k].minY;
			point2.x = bins[k].maxX;
			point2.y = bins[k].maxY;
			//line(img_whitebalance,point1,point2, Scalar(0,k*25,250),5);

			point1.x = bins[k].maxX;
			point1.y = bins[k].maxY;
			point2.x = bins[k].minX;
			point2.y = bins[k].maxY;
			//line(img_whitebalance,point1,point2, Scalar(0,k*25,250),5);

			point1.x = bins[k].minX;
			point1.y = bins[k].maxY;
			point2.x = bins[k].minX;
			point2.y = bins[k].minY;

			point1.x = bins[k].minX;
			point1.y = bins[k].minY;
			point2.x = bins[k].maxX;
			point2.y = bins[k].minY;
			//line(img_whitebalance,point1,point2, Scalar(0,k*25,250),5);
			for (int i = 0; i < 4; i++)
			{		
				line(img_whitebalance, bins[k].vertices[i], bins[k].vertices[(i+1)%4], Scalar(255,255,0),5);
			}
		}
	}
	imshow("final",img_whitebalance); 
 
	return(bins[0]);
}



















void BinDetector::processImage(Image* input, Image* out)
{
    m_frame->copyFrom(input);
    DetectorContours(input);
/*
    // Ensure all the images are the proper size
    if ((m_whiteMaskedFrame->getWidth() != m_frame->getWidth()) || 
        (m_whiteMaskedFrame->getHeight() != m_frame->getHeight()))
    {
        // We are the wrong size delete them and recreate
        deleteImages();
        allocateImages(m_frame->getWidth(), m_frame->getHeight());
    }
*/
    // Make debug output look like m_frame (will be marked up later)
    if (out)
        out->copyFrom(m_frame);
    
    // Convert the image to LCh
/*
    m_frame->setPixelFormat(Image::PF_RGB_8);
    m_frame->setPixelFormat(Image::PF_LCHUV_8);
    
    // Filter for white, black, and red
    filterForWhite(m_frame, m_whiteMaskedFrame);
    filterForRed(m_frame, m_redMaskedFrame);
    filterForBlack(m_frame, m_blackMaskedFrame);
    
    // Update debug image with black, white and red color info
    filterDebugOutput(out);
*/
    // Find all the white blobs
    m_blobDetector.setMinimumBlobSize(m_blobMinWhitePixels);
    m_blobDetector.processImage(m_whiteMaskedFrame);
    BlobDetector::BlobList whiteBlobs = m_blobDetector.getBlobs();
    
    // Find all the black blobs
    m_blobDetector.setMinimumBlobSize(m_blobMinBlackPixels);
    m_blobDetector.processImage(m_blackMaskedFrame);
    BlobDetector::BlobList blackBlobs = m_blobDetector.getBlobs();

    // Find bins
    BlobDetector::BlobList binBlobs;
    findBinBlobs(whiteBlobs, blackBlobs, binBlobs, out);
    
    if (out)
    {
        std::stringstream ss;
        ss << "Bin#: " << binBlobs.size();
        Image::writeText(out, ss.str(), out->getWidth() / 2,
                         out->getHeight() - 15); 
    }

    // Process the individual bins if we have any
    if (binBlobs.size() > 0)
    {
        // We found bins
        m_found = true;
        
        // Process bins to determine there angle and symbol
        BinList newBins;

        int binNumber = 0;
        BOOST_FOREACH(BlobDetector::Blob binBlob, binBlobs)
        {
            Bin newBin = processBin(binBlob, m_runSymbolDetector,
                                    binNumber, out);
            newBins.push_back(newBin);
            binNumber++;
        }

        // Sort through our new bins and match them to the old ones
        TrackedBlob::updateIds(&m_bins, &newBins, &m_lostBins,
                               m_binSameThreshold, m_binLostFrames);

        // Anybody left we didn't find this iteration, so its been dropped
        BOOST_FOREACH(Bin bin, m_bins)
        {
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 0,
                                           bin.getSymbol(), bin.getAngle()));
            event->id = bin.getId();
            publish(EventType::BIN_DROPPED, event);
        }

        // Our new bins are now "the bins", and sort then in relation to the
        // center of the image
        m_bins = newBins;
        m_bins.sort(binToCenterComparer);

        // Determine angle of the array of bins and publish the event
        math::Degree arrayAngle;
        if (findArrayAngle(m_bins, arrayAngle, out))
        {
            // It was a valid angle, send it out
            BinEventPtr event(new BinEvent(arrayAngle));
            publish(EventType::MULTI_BIN_ANGLE, event);
        }

        // Now publish the centered events
        math::Vector2 toCenter(getX(), getY());
        if (toCenter.normalise() < m_centeredLimit)
        {
            if(!m_centered)
            {
                m_centered = true;
                BinEventPtr event(new BinEvent(getX(), getY(), 0,
                                               getSymbol(), getAngle()));
                publish(EventType::BIN_CENTERED, event);
            }
        }
        else
        {
            m_centered = false;
        }

        
        // Send bin events and draw debug update 
        BOOST_FOREACH(Bin bin, m_bins)
        {
            // Draw the debug bin output
            if (out)
                bin.draw(out);

            // Send out the bin event
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 0,
                                           bin.getSymbol(), bin.getAngle()));
            event->id = bin.getId();
            publish(EventType::BIN_FOUND, event);
        }

    }
    else if (m_found)
    {
        // Lets update the ids with no new bins
        BinList emptyBins;
        TrackedBlob::updateIds(&m_bins, &emptyBins, &m_lostBins,
                               m_binSameThreshold, m_binLostFrames);

        // Anybody left has run out of lost frames so its been dropped
        BOOST_FOREACH(Bin bin, m_bins)
        {
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 0,
                                           bin.getSymbol(), bin.getAngle()));
            event->id = bin.getId();
            publish(EventType::BIN_DROPPED, event);
        }

        // Our new bins are now "the bins"
        m_bins = emptyBins;

        if (0 == m_lostBins.size())
        {
            // Publish lost event
            m_found = false;
            m_centered = false;
            publish(EventType::BINS_LOST, core::EventPtr(new core::Event()));
        }
    }
}

bool BinDetector::found()
{
    return m_found;
}
    
float BinDetector::getX()
{
    if (m_bins.size() > 0)
        return m_bins.front().getX();
    else
        return 0;
}
float BinDetector::getY()
{
    if (m_bins.size() > 0)
        return m_bins.front().getY();
    else
        return 0;
}
math::Degree BinDetector::getAngle()
{
    if (m_bins.size() > 0)
        return m_bins.front().getAngle();
    else
        return math::Degree(0);
}

Symbol::SymbolType BinDetector::getSymbol()
{
    if (m_bins.size() > 0)
        return m_bins.front().getSymbol();
    else
        return Symbol::NONEFOUND;
}

BinDetector::BinList BinDetector::getBins()
{
    return m_bins;
}

void BinDetector::setSymbolDetectionOn(bool on)
{
    m_runSymbolDetector = on;
}

void BinDetector::setSymbolImageLogging(bool value)
{
    m_logSymbolImages = value;
}

    
void BinDetector::init(core::ConfigNode config)
{
    // Look up type for the symbol detector and validate it
    std::string symbolDetectorType = "BasicGladiatorDetector";
    if (config.exists("symbolDetector"))
        symbolDetectorType = config["symbolDetector"].asString();
    assert(vision::DetectorMaker::isKeyRegistered(symbolDetectorType) &&
           "Invalid symbol detector type");

    // Create the detector
    config.set("type", symbolDetectorType);
    DetectorPtr detector = vision::DetectorMaker::newObject(
        DetectorMakerParamType(config, core::EventHubPtr()));

    // Convert it the proper type
    m_symbolDetector =
        boost::dynamic_pointer_cast<SymbolDetector>(detector);
    assert(m_symbolDetector && "Symbol detector not of SymbolDetector type");

    
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    // Add properties of the symbolDetector
    propSet->addPropertiesFromSet(m_symbolDetector->getPropertySet().get());
    
    // Debug parameter
    propSet->addProperty(config, false, "debug",
                         "Debug parameter", 0, &m_debug, 0, 1);

    // General properties
    propSet->addProperty(config, false, "centeredLimit",
        "Maximum distance for the bin to be considred \"centered\"",
        0.1, &m_centeredLimit, 0.0, 4.0/3.0);
    propSet->addProperty(config, false, "logSymbolImages",
        "Log all the images passed to the symbols detector to disk",
        false, &m_logSymbolImages);
    
    // Black mask properties
    propSet->addProperty(config, false, "blackIsRed",
        "Treat red as black",
        true, &m_blackIsRed);
    propSet->addProperty(config, false, "blackMaskMinimumPercent",
        "% of for the black mask minimum",
        10, &m_blackMaskMinimumPercent, 0, 100);
    propSet->addProperty(config, false, "blackMaskMaxTotalIntensity",
        "Maximum value of RGB pixels added together for black",
        350, &m_blackMaskMaxTotalIntensity, 0, 765);

    // White mask properties
    propSet->addProperty(config, false, "whiteMaskMinimumPercent",
        "% of for the white mask minimum",
        30, &m_whiteMaskMinimumPercent, 0, 100);
    propSet->addProperty(config, false, "whiteMaskMinimumIntensity",
        "Minimum value of RGB pixels added together for white",
        190, &m_whiteMaskMinimumIntensity, 0, 765);

    // Red mask properties
    propSet->addProperty(config, false, "redMinPercent",
        "Minimum percent of the total pixel value for red",
        35, &m_redMinPercent, 0, 255);
    propSet->addProperty(config, false, "redMinRValue",
        "Minimum value of Red pixel value for for red",
        125, &m_redMinRValue, 0, 255);
    propSet->addProperty(config, false, "redMaxGValue",
        "Maximum value of Green pixel value for for red",
        170, &m_redMaxGValue, 0, 255);
    propSet->addProperty(config, false, "redMaxBValue",
        "Maximum value of Blue pixel value for for red",
        170, &m_redMaxBValue, 0, 255);
    propSet->addProperty(config, false, "redErodeIterations",
        "Erosion iterations on the red filtered image",
	0, &m_redErodeIterations, 0, 10);
    propSet->addProperty(config, false, "redDilateIterations",
        "Dilation iterations on the red filtered image",
         0, &m_redDilateIterations, 0, 10);
    propSet->addProperty(config, false, "redOpenIterations",
        "Opening iterations on the red filtered image",
         0, &m_redOpenIterations, 0, 10);
    propSet->addProperty(config, false, "redCloseIterations",
        "Closing iterations on the red filtered image",
         0, &m_redCloseIterations, 0, 10);

    // Blob detector properties
    propSet->addProperty(config, false, "blobMinBlackPixels",
       "Minimum pixel count of a black blob",
        2500, &m_blobMinBlackPixels, 0, 10000);
    propSet->addProperty(config, false, "blobMinWhitePixels",
       "Minimum pixel count of a white blob",
        1500, &m_blobMinWhitePixels, 0, 10000);
    propSet->addProperty(config, false, "blobMinRedPercent",
       "Size of min red blob as a percent of the smallest black bin blob",
        10.0, &m_blobMinRedPercent, 0.0, 100.0);

    // Bin Determination properties 
    propSet->addProperty(config, false, "binMaxAspectRatio",
       "The maximum aspect ratio the black blob of a bin can have",
        3.0, &m_binMaxAspectRatio, 0.0, 10.0);
    propSet->addProperty(config, false, "binMinFillPrecentage",
       "The minimum amount of the black bob that must be filled to be a bin",
        0.0, &m_binMinFillPercentage, 0.0, 1.0);
    propSet->addProperty(config, false, "redMinFillPrecentage",
       "The minimum amount of the black blob that must be red to be a bin",
        0.0, &m_minRedFillPercentage, 0.0, 1.0);
    propSet->addProperty(config, false, "binMaxOverlaps",
       "The minimum amount of the black bob that must be filled to be a bin",
        20, &m_binMaxOverlaps, 0, 20);
    propSet->addProperty(config, false, "binSameThreshold",
       "The max distance between bins on different frames",
        0.2, &m_binSameThreshold, 0.0, 4.0/3.0);
    propSet->addProperty(config, false, "binLostFrames",
       "How many frames a bin must be missing before reporting lost",
        0, &m_binLostFrames, 0, 30);

    propSet->addProperty(config, false, "binHoughPixelRes",
        "Pixel resolution for hough based bin angle detection",
        3, &m_binHoughPixelRes, 0, 100); // 3 in Dans version
    propSet->addProperty(config, false, "binHoughThresdhold",
        "Threshold for hough based bin angle detection",
        110, &m_binHoughThreshold, 0, 300); // 150 in Dans version
    propSet->addProperty(config, false, "binHoughMinLineLength",
        "Minimum length of lines found when detemining bin angle",
        20, &m_binHoughMinLineLength, 0, 100); // 5 in Dans version
    propSet->addProperty(config, false, "binHoughMaxLineGap",
        "Maximum gap between lines for them to be joined",
        50, &m_binHoughMaxLineGap, 0, 300); // 50 in Dans version


    m_whiteFilter->addPropertiesToSet(propSet, &config,
                                      "whiteL", "Luminance",
                                      "whiteC", "Chrominance",
                                      "whiteH", "Hue",
                                      125, 255,  // L defaults // 180,255
                                      0, 255,  // U defaults // 76, 245
                                      0, 255); // V defaults // 200,255

    m_blackFilter->addPropertiesToSet(propSet, &config,
                                      "blackL", "Luminance",
                                      "blackC", "Chrominance",
                                      "blackH", "Hue",
                                      0, 124,  // L defaults // 180,255
                                      0, 255,  // U defaults // 76, 245
                                      0, 255); // V defaults // 200,255

    
    m_redFilter->addPropertiesToSet(propSet, &config,
                                    "redL", "Luminance",
                                    "redC", "Chrominance",
                                    "redH", "Hue",
                                    5, 30,  // L defaults // 180,255
                                    0, 200,  // U defaults // 76, 245
                                    45, 255); // V defaults // 200,255

    m_frame = new OpenCVImage(640, 480, Image::PF_BGR_8);

    // Make sure the configuration is valid
    //propSet->verifyConfig(config, true);
}

void BinDetector::allocateImages(int width, int height)
{
    m_percents = new OpenCVImage(width, height);
    m_whiteMaskedFrame = new OpenCVImage(width, height, Image::PF_BGR_8);
    m_blackMaskedFrame = new OpenCVImage(width, height, Image::PF_BGR_8);
    m_redMaskedFrame = new OpenCVImage(width, height, Image::PF_BGR_8);
    
    int extra = BIN_EXTRACT_BORDER * 2;
    size_t size = (width + extra) * (height + extra) * 3;
    m_extractBuffer = new unsigned char[size];
    m_scratchBuffer1 = new unsigned char[size];
    m_scratchBuffer2 = new unsigned char[size];
    m_scratchBuffer3 = new unsigned char[size];
}

void BinDetector::deleteImages()
{
    delete m_percents;
    delete m_whiteMaskedFrame;
    delete m_blackMaskedFrame;
    delete m_redMaskedFrame;
    delete [] m_extractBuffer;
    delete [] m_scratchBuffer1;
    delete [] m_scratchBuffer2;
    delete [] m_scratchBuffer3;
}
    
void BinDetector::filterForWhite(Image* input, Image* output)
{
    m_whiteFilter->filterImage(input, output);
}

void BinDetector::filterForBlack(Image* input, Image* output)
{
    m_blackFilter->filterImage(input, output);
  
    // And the red and black filter into the black
    if (m_blackIsRed)
    {
        unsigned char* blackData = m_blackMaskedFrame->getData();
        unsigned char* redData = m_redMaskedFrame->getData();
        int size = input->getWidth() * input->getHeight() * 3;
        for (int i = 0; i < size; ++i)
        {
            if (*redData)
                *blackData = 255;
            redData++;
            blackData++;
        }
    }
}

void BinDetector::filterForRed(Image* input, Image* output)
{
    m_redFilter->filterImage(input, output);

    cvSmooth(output->asIplImage(), output->asIplImage(), CV_MEDIAN, 5);

    if (m_redOpenIterations > 0)
    {
        IplImage *img = output->asIplImage();
        cvErode(img, img, NULL, m_redOpenIterations);
        cvDilate(img, img, NULL, m_redOpenIterations);
    }

    if (m_redCloseIterations > 0)
    {
        IplImage *img = output->asIplImage();
        cvDilate(img, img, NULL, m_redCloseIterations);
        cvErode(img, img, NULL, m_redCloseIterations);
    }

    if (m_redErodeIterations)
    {
      cvErode(output->asIplImage(), output->asIplImage(), 0, 
	      m_redErodeIterations);
    }
    if (m_redDilateIterations)
    {
      cvDilate(output->asIplImage(), output->asIplImage(), 0, 
	       m_redDilateIterations);
    }

}

void BinDetector::filterDebugOutput(Image* out)
{
    if (out)
    {
        int size = out->getWidth() * out->getHeight() * 3;
        unsigned char* outData = out->getData();
        unsigned char* whiteData = m_whiteMaskedFrame->getData();
        unsigned char* blackData = m_blackMaskedFrame->getData();
        unsigned char* redData = m_redMaskedFrame->getData();


        for (int count = 0; count < size; count += 3)
        {
            bool setColor = false;
            unsigned char R = 147;
            unsigned char G = 20;
            unsigned char B = 255;

            if ((whiteData[count] != 0) && (blackData[count] == 0))
            {
                // Make all white black
                R = G = B = 0;
                setColor = true;
            }
            else if ((blackData[count] != 0) && (whiteData[count] == 0))
            {
                // Make all black white
                R = G = B = 255;
                setColor = true;
            }
            else if  ((blackData[count] != 0) || (whiteData[count] != 0))
            {
                // else defaults to pink
                setColor = true;
            }

            if (redData[count] != 0)
            {
                // we have red
                R = 255;
                G = B = 0;
                setColor = true;
            }
            
            if (setColor)
            {
                outData[count] = B;
                outData[count+1] = G;
                outData[count+2] = R;
            }
        }
    }
}

void BinDetector::findBinBlobs(const BlobDetector::BlobList& whiteBlobs,
                               const BlobDetector::BlobList& blackBlobs,
                               BlobDetector::BlobList& binBlobs,
                               Image* output)
{
    BlobDetector::BlobList nonBinBlobs;
    BlobDetector::BlobList candidateBins;
    double averageHeight = 0;
    int foundBins = 0;

    // TODO: Add fill percenter, full height percentage, border width
    //       and aspect ratio to the configurable parameters


    // NOTE: all blobs sorted largest to smallest
    BOOST_FOREACH(BlobDetector::Blob blackBlob, blackBlobs)
    {
        bool matchedBlob = false;
        
        BOOST_FOREACH(BlobDetector::Blob whiteBlob, whiteBlobs)
        {
            // Sadly, this totally won't work at the edges of the screen...
            // crap damn.
            if (whiteBlob.containsInclusive(blackBlob, 2) &&
                (blackBlob.getAspectRatio() < m_binMaxAspectRatio) &&
                (blackBlob.getFillPercentage() > m_binMinFillPercentage) &&
		(getRedFillPercentage(blackBlob, m_redMaskedFrame)
		 >= m_minRedFillPercentage))
            {
                // blackBlobs[blackBlobIndex] is the black rectangle of a bin
                candidateBins.push_back(blackBlob);
                matchedBlob = true;
		averageHeight += blackBlob.getHeight();
                // Don't add it once for each white blob containing it,
                // thatd be dumb.
                break;
            }
        }

	if (!matchedBlob)
	    nonBinBlobs.push_back(blackBlob);
    }
    
    // Determine the average height
    foundBins = binBlobs.size();
    averageHeight /= foundBins;

    // Now lets go through the non bin blobs, and see if we can figure
    // out if they are black blobs that have just grown so large they only
    // have white on the sides
/*    BOOST_FOREACH(BlobDetector::Blob blackBlob, nonBinBlobs)
    {
        double blobHeight = blackBlob.getHeight();
        double imageHeight = m_whiteMaskedFrame->getHeight();
        if (((blobHeight / imageHeight) > 0.9) && 
            (blackBlob.getFillPercentage() > 0.8))
        {
            // We are a full height blob, which just might have white blobs on 
            // the side

            bool whiteLeft = false;
            bool whiteRight = false;

            // Attempt to find white blobs on the side
            BOOST_FOREACH(BlobDetector::Blob whiteBlob, whiteBlobs)
            {
                double whiteblobHeight = blackBlob.getHeight();
                if ((whiteblobHeight / imageHeight) > 0.9)
                {
                    // The white blob is full height as well
                    if (blackBlob.boundsIntersect(whiteBlob, 12))
                    {
                        if (whiteBlob.getCenterX() < blackBlob.getCenterX())
                            whiteLeft = true;
                        else
                           whiteRight = true;
                    }
                }
            } // foreach white blob

            // Determine if the black blob is on the edge
            bool onLeftEdge = blackBlob.getMinX() <= 12;
            bool onRightEdge = 
              blackBlob.getMaxX() >= (int)(m_whiteMaskedFrame->getWidth() - 12);
            
            // If we are the edge and have a white on the other side or
            // we have white on the both sides, we are a bin! If both sides
	    // are on the edge, we are not a bin
            if ((onLeftEdge || whiteLeft) && (onRightEdge || whiteRight) && 
                !(onLeftEdge && onRightEdge))
            {
       	        if (whiteLeft && whiteRight)
		{
		    // If both edges are white we need to be a proper aspect
		    // ratio
		    if (blackBlob.getAspectRatio() < 3)
		        candidateBins.push_back(blackBlob);
		}
		else 
		{
		    if (foundBins > 0)
		    {
		        // If we found a "normal" bin make sure this bins
		        // height is similar to that one
		        double heightError = 
			    fabs(blackBlob.getHeight() - averageHeight);
			if ((heightError / averageHeight) < 0.2)
			    candidateBins.push_back(blackBlob);
		    }
		    else
		    {
		        candidateBins.push_back(blackBlob);
		    }
		}
            }
        } // if (blob of full height)
    }
*/

    bool removed = true;
    while (removed)
    {
        removed = false;
        
        // Filter all the candidates and remove ones that overlap
        BlobDetector::BlobList::iterator mainIter = candidateBins.begin();
        BlobDetector::BlobList::iterator iterToRemove =
            candidateBins.end();
        int removeOverlaps = 0;
        while (mainIter != candidateBins.end())
        {
            // Count the number of overlaps
            int binOverlaps = 0;
            BlobDetector::BlobList::const_iterator iter = candidateBins.begin();
            while (iter != candidateBins.end())
            {
                if (mainIter->boundsIntersect(*iter) &&
                    (mainIter->getCenterX() != iter->getCenterX()) &&
                    (mainIter->getCenterY() != iter->getCenterY()))
                {
                    binOverlaps++;
                }
                iter++;
            }

            // Only add if its under the overlap count
            if (binOverlaps > m_binMaxOverlaps && binOverlaps > removeOverlaps)
            {
                removeOverlaps = binOverlaps;
                iterToRemove = mainIter;
                // We found a bad bin remove and start over again
                removed = true;
            }
            
            // Advance through
            mainIter++;
        }

        // Remove the bad bin
        if (removed)
        {
            candidateBins.erase(iterToRemove);
            if (output)
                iterToRemove->draw(output);
        }
    }

    // Anybody left is really a bin
    binBlobs = candidateBins;
}

bool BinDetector::findArrayAngle(const BinList& bins, math::Degree& finalAngle,
                                 Image* output)
{
    // Determine the angle of the bin array
    if (bins.size() > 1 && bins.size() <= 4)
    {
        int curX = -1;
        int curY = -1;
        int prevX = -1;
        int prevY = -1;
        int binsCenterX = 0;
        int binsCenterY = 0;
        //If you change this from a 3, also change the loops below
        double innerAngles[3];
        
        int angleCounter = 0;
        BOOST_FOREACH(Bin bin, bins)
        {
            binsCenterX += bin.getCenterX();
            binsCenterY += bin.getCenterY();
            prevX = curX;
            prevY = curY;
            curX = bin.getCenterX();
            curY = bin.getCenterY();
            
            if (prevX == -1 && prevY == -1)
            {
                // the first one
            }
            else
            {
                CvPoint prev;
                CvPoint cur;
                prev.x = prevX;
                prev.y = prevY;
                cur.x = curX;
                cur.y = curY;
                
                //Swap so we always get answers mod 180.
                if (prev.x > cur.x || (prev.x == cur.x && prev.y > cur.y))
                {
                    CvPoint swap = prev;
                    prev = cur;
                    cur = swap;
                }
                
                double innerAng = atan2(cur.y - prev.y,cur.x - prev.x);
                
                innerAngles[angleCounter] = innerAng;
                angleCounter++;

                // Draw line between bins
                if (output)
                {
                    cvLine(output->asIplImage(), prev, cur, CV_RGB(255,0,0), 5,
                           CV_AA, 0);
                }
            }
        }
        
        double sinTotal = 0;
        double cosTotal = 0;
        for (int i = 0; i < angleCounter && i < 3; i++)
        {
            sinTotal+=sin(innerAngles[i]);
            cosTotal+=cos(innerAngles[i]);
        }
        
        double finalAngleAcrossBins = atan2(sinTotal,cosTotal);

        // Draw line across all the bins
        if (output)
        {
            CvPoint drawStart, drawEnd;
            drawStart.x = binsCenterX/(angleCounter+1);
            drawStart.y = binsCenterY/(angleCounter+1);
            
            drawEnd.x = (int)(drawStart.x + cosTotal / (angleCounter) * 250);
            drawEnd.y = (int)(drawStart.y + sinTotal / (angleCounter) * 250);

            cvLine(output->asIplImage(), drawStart, drawEnd, CV_RGB(255,255,0),
                   5, CV_AA,0); 
        }
        //printf("final angle across bins %f:\n", finalAngleAcrossBins);
        
        math::Radian angleAcrossBins(finalAngleAcrossBins);
        
        finalAngle = math::Degree(-angleAcrossBins.valueDegrees());
        //printf("Final Inner Angle For Joe: %f\n", finalAngle.valueDegrees());
        return true;
    } // angle of bin array

    return false;
}

BinDetector::Bin BinDetector::processBin(BlobDetector::Blob bin,
                                         bool detectSymbol,
                                         int binNum, Image* output)
{
    // Get corners of area to extract (must be multiple of 4)
    int width = bin.getWidth()/4 * 4;
    int height = bin.getHeight()/4 * 4;
    
    int upperLeftX = bin.getCenterX() - width/2;
    int upperLeftY = bin.getCenterY() - height/2;
    int lowerRightX = bin.getCenterX() + width/2;
    int lowerRightY = bin.getCenterY() + height/2;

    // Make sure we are not outside the image
    upperLeftX = std::max(0, upperLeftX);
    upperLeftX = std::min((int)m_blackMaskedFrame->getWidth() - 1, upperLeftX);

    upperLeftY = std::max(0, upperLeftY);
    upperLeftY = std::min((int)m_blackMaskedFrame->getHeight() - 1, upperLeftY);

    lowerRightX = std::max(0, lowerRightX);
    lowerRightX = std::min((int)m_blackMaskedFrame->getWidth() - 1,
                           lowerRightX);

    lowerRightY = std::max(0, lowerRightY);
    lowerRightY = std::min((int)m_blackMaskedFrame->getHeight() - 1,
                           lowerRightY);
    
    Image* binImage = Image::extractSubImage(
        m_blackMaskedFrame, m_extractBuffer, upperLeftX, upperLeftY,
        lowerRightX, lowerRightY);

    // Determine angle of the bin
    math::Degree binAngle(0);
    calculateAngleOfBin(bin, binImage, binAngle, output);
    delete binImage; // m_extractBuffer free to use
    
    // Determine bin symbol if desired
    Symbol::SymbolType symbol = Symbol::NONEFOUND;
         
    if (detectSymbol)
    {
        // Extract red masked image
        Image* redBinImage = Image::extractSubImage(
            m_redMaskedFrame, m_extractBuffer, upperLeftX, upperLeftY,
            lowerRightX, lowerRightY);
        
        // Rotate to upright
        Image* rotatedBinImage =
            vision::Image::loadFromBuffer(m_scratchBuffer1,
                                          redBinImage->getWidth(),
                                          redBinImage->getHeight(),
                                          false,
                                          m_redMaskedFrame->getPixelFormat());
        
        vision::Image::transform(redBinImage, rotatedBinImage, binAngle);
        
        // Crop down Image to square around bin symbol
        Image* cropped = cropBinImage(rotatedBinImage, m_scratchBuffer3);

        if (cropped)
        {
            symbol = determineSymbol(redBinImage, m_scratchBuffer1, output);

            if (output && (binNum < 4))
            {
                // Scale the image to 128x128
                Image* scaledBin =
                    Image::loadFromBuffer(m_scratchBuffer1, 128, 128, false);
                cvResize(cropped->asIplImage(), scaledBin->asIplImage(),
                         CV_INTER_LINEAR);
                Image::drawImage(scaledBin, binNum * 128, 0, output, output);
                
                delete scaledBin; // m_scratchBuffer1 free to use
            }

            // Log the images if desired
            if (m_logSymbolImages)
                logSymbolImage(redBinImage, symbol);
            
            delete cropped;// m_scratchBuffer2 free to use
        }
        delete rotatedBinImage; // m_scratchBuffer1 free to use
        delete redBinImage;
    }
    
    // Report our results
    return Bin(bin, m_percents, binAngle, m_binID++, symbol);
}

double BinDetector::getRedFillPercentage(BlobDetector::Blob bin,
					 Image* redImage)
{
    // Get corners of area to extract (must be multiple of 4)
    int width = bin.getWidth()/4 * 4;
    int height = bin.getHeight()/4 * 4;
    
    int upperLeftX = bin.getCenterX() - width/2 - BIN_EXTRACT_BORDER;
    int upperLeftY = bin.getCenterY() - height/2 - BIN_EXTRACT_BORDER;
    int lowerRightX = bin.getCenterX() + width/2 + BIN_EXTRACT_BORDER;
    int lowerRightY = bin.getCenterY() + height/2 + BIN_EXTRACT_BORDER;

    // Make sure we are not outside the image
    upperLeftX = std::max(0, upperLeftX);
    upperLeftX = std::min((int)redImage->getWidth() - 1, upperLeftX);

    upperLeftY = std::max(0, upperLeftY);
    upperLeftY = std::min((int)redImage->getHeight() - 1, upperLeftY);

    lowerRightX = std::max(0, lowerRightX);
    lowerRightX = std::min((int)redImage->getWidth() - 1,
                           lowerRightX);

    lowerRightY = std::max(0, lowerRightY);
    lowerRightY = std::min((int)redImage->getHeight() - 1,
                           lowerRightY);


    // Get the red pixels
    int redPixels = Image::countWhitePixels(redImage,
					    upperLeftX, upperLeftY,
					    lowerRightX, lowerRightY);

    return ((double)redPixels) / ((double)(bin.getWidth() * bin.getHeight()));
					      //  return 0;
}

bool BinDetector::calculateAngleOfBin(BlobDetector::Blob bin, Image* input,
                                      math::Degree& foundAngle, Image* output)
{
    // Grab a gray scale version of the input image
    CvSize size = cvGetSize(input->asIplImage());
    IplImage* grayScale = cvCreateImageHeader(size, IPL_DEPTH_8U, 1);
    cvSetData(grayScale, m_scratchBuffer1, input->getWidth());
    cvCvtColor(input->asIplImage(), grayScale, CV_BGR2GRAY);

    // Grab a cannied version of our image
    IplImage* cannied = cvCreateImageHeader(size, IPL_DEPTH_8U, 1);
    cvSetData(cannied, m_scratchBuffer2, input->getWidth());
    cvCanny(grayScale, cannied, 50, 200, 3 );

    // Run the hough transform on the cannied image
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    
    lines = cvHoughLines2( cannied, storage, CV_HOUGH_PROBABILISTIC,
                           m_binHoughPixelRes,
                           CV_PI/180, m_binHoughThreshold,
                           m_binHoughMinLineLength, m_binHoughMaxLineGap);

    // Determine angle from hough transform
    float longestLineLength = -1;
    float angle = 0;
    for(int i = 0; i < lines->total; i++ )
    {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
        float lineX = line[1].x - line[0].x; 
        float lineY = line[1].y - line[0].y;

        cvLine(input->asIplImage(), line[0], line[1], CV_RGB(255,255,0),
               5, CV_AA, 0);
        
        if (output && m_debug == 1)
        {
            line[0].x += bin.getCenterX() - input->getWidth() / 2;
            line[0].y += bin.getCenterY() - input->getHeight() / 2;
            line[1].x += bin.getCenterX() - input->getWidth() / 2;
            line[1].y += bin.getCenterY() - input->getHeight() / 2;
            
            cvLine(output->asIplImage(), line[0], line[1], CV_RGB(255,255,0),
                   5, CV_AA, 0);
        }
	//printf("Line dimensions: %f, %f\n", lineX, lineY);

        if (longestLineLength < (lineX * lineX + lineY * lineY))
        {
            angle = atan2(lineY,lineX);
            longestLineLength = lineX * lineX + lineY * lineY;
        }
    }

    bool success = false;
    if (lines->total != 0)
    {
        // We can only computer and angle if we found the lines
        double angInDegrees = math::Radian(angle).valueDegrees();
        double angleToReturn = 90-angInDegrees;
        
        if (angleToReturn >= 90)
            angleToReturn -= 180;
        else if (angleToReturn < -90)
            angleToReturn += 180;
        
        math::Degree finalJoeAngle(angleToReturn);
        foundAngle = finalJoeAngle;
        success = true;
    }
    
    cvReleaseImageHeader(&cannied);
    cvReleaseMemStorage(&storage);
    cvReleaseImageHeader(&grayScale);
    
    return success;
}


//Returns false on failure, puts symbol into scaledRedSymbol.
Image* BinDetector::cropBinImage(Image* redBinImage,
                                 unsigned char* storageBuffer)
{   
    int minSymbolX = redBinImage->getWidth() + 1;
    int minSymbolY = redBinImage->getHeight() + 1;
    int maxSymbolX = 0;
    int maxSymbolY = 0;
    
    // TODO: consider dilation of red image
    //cvDilate(redBinImage->asIplImage(), redBinImage->asIplImage(), NULL, 1);

    //   int size = 0;
    m_blobDetector.setMinimumBlobSize(100);
    m_blobDetector.processImage(redBinImage);
    if (!m_blobDetector.found())
    {
        // No symbol found, don't make a histogram
        return 0;
    }
    else
    {
        // TODO: General blob combine and apply it here to join broken up
        //       symbols
        
        //find biggest two blobs (hopefully should be just one, but if spade or club split..)
        std::vector<ram::vision::BlobDetector::Blob> blobs =
            m_blobDetector.getBlobs();
        ram::vision::BlobDetector::Blob biggest(-1,0,0,0,0,0,0);
        ram::vision::BlobDetector::Blob secondBiggest(0,0,0,0,0,0,0);
        ram::vision::BlobDetector::Blob swapper(-1,0,0,0,0,0,0);
        for (unsigned int blobIndex = 0; blobIndex < blobs.size(); blobIndex++)
        {
            if (blobs[blobIndex].getSize() > secondBiggest.getSize())
            {
                secondBiggest = blobs[blobIndex];
                if (secondBiggest.getSize() > biggest.getSize())
                {
                    swapper = secondBiggest;
                    secondBiggest = biggest;
                    biggest = swapper;
                }
            }
        }
        minSymbolX = biggest.getMinX();
        minSymbolY = biggest.getMinY();
        maxSymbolX = biggest.getMaxX();
        maxSymbolY = biggest.getMaxY();

/*        if (!m_incrediblyWashedOutImages)//A fancy way to say that at transdec, the symbols don't get split.
        {
        if (blobs.size() > 1)
        {
            if (minSymbolX > secondBiggest.getMinX())
                minSymbolX = secondBiggest.getMinX();
            if (minSymbolY > secondBiggest.getMinY())
                minSymbolY = secondBiggest.getMinY();
            if (maxSymbolX < secondBiggest.getMaxX())
                maxSymbolX = secondBiggest.getMaxX();
            if (maxSymbolY < secondBiggest.getMaxY())
                maxSymbolY = secondBiggest.getMaxY();

        }
        }*/
    }

    // Find the rows/cols of the croped image
    int onlyRedSymbolRows = (maxSymbolX - minSymbolX + 1);// / 4 * 4;
    int onlyRedSymbolCols = (maxSymbolY - minSymbolY + 1);// / 4 * 4;
    if (onlyRedSymbolRows == 0 || onlyRedSymbolCols == 0)
    {
        return 0;
    }

    // Make the image sqaure on its biggest dimension
    if (m_symbolDetector->needSquareCropped())
    {
        onlyRedSymbolRows = onlyRedSymbolCols = (onlyRedSymbolRows > onlyRedSymbolCols ? onlyRedSymbolRows : onlyRedSymbolCols);

    }

    // Sanity check on the sizes
    if (onlyRedSymbolRows >= (int)redBinImage->getWidth() ||
        onlyRedSymbolCols >= (int)redBinImage->getHeight())
    {
        return 0;
    }

    // Extract the symbol porition of the image
    int centerX = (maxSymbolX+minSymbolX)/2;
    int centerY = (maxSymbolY+minSymbolY)/2;
    int upperLeftX = centerX - onlyRedSymbolRows/2;
    int upperLeftY = centerY - onlyRedSymbolCols/2;
    int lowerRightX = centerX + onlyRedSymbolRows/2;
    int lowerRightY = centerY + onlyRedSymbolCols/2;

    // Make sure we are not outside the image
//    upperLeftX = std::max(0, upperLeftX);
//    upperLeftY = std::max(0, upperLeftY);
//    lowerRightX = std::min((int)redBinImage->getWidth() - 1, lowerRightX);
//    lowerRightY = std::min((int)redBinImage->getWidth() - 1, lowerRightY);

    Image* croppedImage = Image::extractSubImage(
        redBinImage, storageBuffer, upperLeftX, upperLeftY,
        lowerRightX, lowerRightY);

    return croppedImage;
}

Symbol::SymbolType BinDetector::determineSymbol(Image* input,
                                                unsigned char* scratchBuffer,
                                                Image* output)
{
    m_symbolDetector->processImage(input, output);
    // Filter symbol type
    Symbol::SymbolType symbolFound = m_symbolDetector->getSymbol(); 
    return symbolFound;
}

void BinDetector::logSymbolImage(Image* image, Symbol::SymbolType symbol)
{
    static int saveCount = 1;
    
    if (saveCount == 1)
    {
        // First run, make sure all the directories are created
        boost::filesystem::path base = core::Logging::getLogDir();

        BOOST_FOREACH(std::string name, Symbol::getSymbolNames())
        {
            boost::filesystem::path symbolDir = base / name;
            if (!boost::filesystem::exists(symbolDir))
                boost::filesystem::create_directories(symbolDir);
        }
    }

    // Determine the directory to place the image based on symbol
    boost::filesystem::path base = core::Logging::getLogDir();
    boost::filesystem::path baseDir = base / Symbol::symbolToText(symbol);

    // Determine the final path
    std::stringstream ss;
    ss << saveCount << ".png";
    boost::filesystem::path fullPath =  baseDir / ss.str();
    
    // Save the image and increment our counter
    Image::saveToFile(image, fullPath.string());
    saveCount++;
}

} // namespace vision
} // namespace ram
