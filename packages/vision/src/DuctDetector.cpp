/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Chris Giles
 * All rights reserved.
 *
 * Author: Chris Giles <cgiles17@umd.edu>
 * File:  packages/vision/include/DuctDetector.cpp
 */

// STD Includes
#include <algorithm>
#include <cmath>
#include <iostream>
#include <math.h>
#include <vector>

// Library Includes
//#include "highgui.h"

// Project Includes
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/DuctDetector.h"
#include "vision/include/Events.h"
#include "vision/include/main.h"
#include <boost/foreach.hpp>

#ifndef M_PI
#define M_PI 3.14159
#endif

//#include "vision/include/DuctDetector.h"

namespace ram {
namespace vision {

bool DuctDetector::blobsAreClose(BlobDetector::Blob b1, BlobDetector::Blob b2, double growThreshX, double growThreshY)
{
    int minX = b1.getMinX();
    int maxX = b1.getMaxX();
    int minY = b1.getMinY();
    int maxY = b1.getMaxY();
    
    int minX2 = b2.getMinX();
    int maxX2 = b2.getMaxX();
    int minY2 = b2.getMinY();
    int maxY2 = b2.getMaxY();

    int b1Width = maxX - minX;
    int b2Width = maxX2 - minX2;

    int b1Height = maxY - minY;
    int b2Height = maxY2 - minY2;
    
    BlobDetector::Blob b1Bigger((int)(b1.getSize()),
                                b1.getCenterX(),
                                b1.getCenterY(),
                                maxX + (int)(b1Width * growThreshX),
                                minX - (int)(b1Width * growThreshX),
                                maxY + (int)(b1Height * growThreshY),
                                minY - (int)(b1Height * growThreshY));
    
    BlobDetector::Blob b2Bigger((int)(b2.getSize()),
                                b2.getCenterX(),
                                b2.getCenterY(),
                                maxX2 + (int)(b2Width * growThreshX),
                                minX2 - (int)(b2Width * growThreshX),
                                maxY2 + (int)(b2Height * growThreshY),
                                minY2 - (int)(b2Height * growThreshY));

    return b1Bigger.boundsIntersect(b2Bigger);
}

  //minTotalRGB should be like 125
  //minTotalRGB on failure should be like 150
  //maxRedFailureThresh should be like 50.
int DuctDetector::yellow(unsigned char r, unsigned char g, unsigned char b, double minRedOverGreen, double maxRedOverGreen, double minRedOverBlue, double minGreenOverBlueOnFailure, int maxRedFailureThresh, int minTotalRGB, int minTotalRGBOnFailure)
{
    //Yellow
    if ((g * minRedOverGreen <= r) &&
        (g * maxRedOverGreen >= r) &&
        (b * minRedOverBlue <= r) &&
        (r + b + g >= minTotalRGB))
    {
        return 1;
    }
    else if ((r <= maxRedFailureThresh) && (r < b) &&
             (g >= (b * minGreenOverBlueOnFailure)) &&
             (r+b+g >= minTotalRGBOnFailure))
    {//Dark greenish yellowish blackish stuff
        return 1;
    }
    return 0;
}

double min(double a, double b)
{
    if (a < b)
        return a;
    return b;
}

double max(double a, double b)
{
    if (a > b)
        return a;
    return b;
}

DuctDetector::DuctDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_fullDuct(0,0,0,0,0,0,0),
    blobDetector(config,eventHub),
    m_working(new OpenCVImage(640, 480)),
    m_workingPercents(new OpenCVImage(640,480)),
    m_blackMasked(new OpenCVImage(640,480)),
    m_yellowMasked(new OpenCVImage(640,480)),
    m_x(0.0),
    m_y(0.0),
    m_rotation(M_PI/2.0),
    n_x(0.0),
    n_y(0.0),
    m_found(false),
    containsOne(false)
{
    init(config);
}
    
DuctDetector::DuctDetector(core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_fullDuct(0,0,0,0,0,0,0),
    blobDetector(),
    m_working(new OpenCVImage(640, 480)),
    m_workingPercents(new OpenCVImage(640,480)),
    m_blackMasked(new OpenCVImage(640,480)),
    m_yellowMasked(new OpenCVImage(640,480)),
    m_x(0.0),
    m_y(0.0),
    m_rotation(M_PI/2.0),
    n_x(0.0),
    n_y(0.0),
    m_found(false),
    containsOne(false)
{
    init(core::ConfigNode::fromString("{}"));
}

DuctDetector::~DuctDetector()
{
    delete m_working;
    delete m_workingPercents;
    delete m_blackMasked;
    delete m_yellowMasked;
}

void DuctDetector::mergeBlobs(std::vector<BlobDetector::Blob> *allBlobs,
			      std::vector<BlobDetector::Blob> *allBlobsMerged)
{
  //    printf("Starting merge blobs\n");
    BlobDetector::Blob empty(0,0,0,0,0,0,0);
    int size = allBlobs->size();
    //    printf("Size: %d\n",size);
    int* blobUnionizer = new int[size];
    BlobDetector::Blob* unionBlobs = new BlobDetector::Blob[size];
    int* blobCounts = new int[size];
    double growThreshX = m_defaultGrowThreshX;
    double growThreshY = m_defaultGrowThreshY;
    
    for (int i = 0; i < size; i++)
    {
        blobUnionizer[i] = i;
        unionBlobs[i] = (*allBlobs)[i];
        blobCounts[i] = 1;
    }
    
    for (int i = 0; i < size; i ++)
    {
        for (int j = i+1; j < size; j++)
        {
            bool blobsClose = false;
            double XOverY = ((double)((*allBlobs)[i].getMaxX() - (*allBlobs)[i].getMinX()))/(double)(((*allBlobs)[i].getMaxY() - (*allBlobs)[i].getMinY()));
            double XOverY2 = ((double)((*allBlobs)[j].getMaxX() - (*allBlobs)[j].getMinX()))/(double)(((*allBlobs)[j].getMaxY() - (*allBlobs)[j].getMinY()));
            
	    
            if (XOverY >= m_minXOverYToUpGrowThresh ||
		XOverY2 >= m_minXOverYToUpGrowThresh)
	    {
	      growThreshX = m_uppedGrowThreshX;
	      growThreshY = m_uppedGrowThreshY;
	    }
	    
	    blobsClose = blobsAreClose((*allBlobs)[i],(*allBlobs)[j], growThreshX, growThreshY);

            if (blobsClose)
            {
                int rooti = i;
                int rootj = j;
                
                while (rooti != blobUnionizer[rooti])
                    rooti = blobUnionizer[rooti];

                while (rootj != blobUnionizer[rootj])
                    rootj = blobUnionizer[rootj];

                int minRoot = (rooti < rootj)? rooti : rootj;
                blobUnionizer[i] = blobUnionizer[j] = minRoot;
		
		//		printf("%d = %d = %d.\n",i,j,minRoot);
            }
        }
    }

    for (int i = size-1; i >=0; i--)
    {
        if (blobUnionizer[i] == i)
        {
            allBlobsMerged->push_back(unionBlobs[i]);
            continue;
        }

        BlobDetector::Blob curBlob = unionBlobs[i];
        BlobDetector::Blob parentBlob = unionBlobs[blobUnionizer[i]];

        int minX = (curBlob.getMinX() < parentBlob.getMinX())?curBlob.getMinX():parentBlob.getMinX();
        int minY = (curBlob.getMinY() < parentBlob.getMinY())?curBlob.getMinY():parentBlob.getMinY();
        int maxX = (curBlob.getMaxX() > parentBlob.getMaxX())?curBlob.getMaxX():parentBlob.getMaxX();
        int maxY = (curBlob.getMaxY() > parentBlob.getMaxY())?curBlob.getMaxY():parentBlob.getMaxY();
        
        int centerX = (minX + maxX) / 2;
        int centerY = (minY + maxY) / 2;
        
        
        BlobDetector::Blob newBlob(curBlob.getSize() + parentBlob.getSize(),
                     centerX,centerY,maxX,minX,maxY,minY);

        unionBlobs[blobUnionizer[i]] = newBlob;
        blobCounts[blobUnionizer[i]] += blobCounts[i];

        blobCounts[i] = -1;
        unionBlobs[i] = empty;
        blobUnionizer[i] = -1;
    }
    delete[] blobUnionizer;
    delete[] unionBlobs;
    delete[] blobCounts;    
} 

void DuctDetector::init(core::ConfigNode config)
{
    // get the threshold values from the config file
    m_minRedOverGreen = config["minRedOverGreen"].asDouble(0.5);
    m_maxRedOverGreen = config["maxRedOverGreen"].asDouble(1.5);
    m_minRedOverBlue = config["minRedOverBlue"].asDouble(1.0);
    m_minGreenOverBlueOnRedFailureForInsideDuct =
        config["minGreenOverBlueOnRedFailureForInsideDuct"].asDouble(1.1);
    
    m_maxRedFailureThresh = config["maxRedFailureThresh"].asInt(50);
    m_minTotalRGB = config["minTotalRGB"].asInt(125);
    m_minTotalRGBOnFailure = config["minTotalRGBOnFailure"].asInt(150);
    
    m_erodeIterations = config["erodeIterations"].asInt(3);
    m_dilateIterations = config["dilateIterations"].asInt(3);
    m_alignedThreshold = config["alignedThreshold"].asDouble(5);
    m_centerAlignedThreshold = config["centerAlignedThreshold"].asInt(25);
    
    m_minBlackPercent = config["minBlackPercent"].asInt(5);
    m_maxBlackTotal = config["maxBlackTotal"].asInt(300);
    
    m_defaultGrowThreshX = config["defaultGrowThreshX"].asDouble(.05);
    m_defaultGrowThreshY = config["defaultGrowThreshY"].asDouble(.05);
    m_minXOverYToUpGrowThresh = 
      config["minXOverYToUpGrowThresh"].asDouble(1.5);
    m_uppedGrowThreshX = config["uppedGrowThreshX"].asDouble(.5);
    m_uppedGrowThreshY = config["uppedGrowThreshY"].asDouble(.05);
}
    
void DuctDetector::processImage(Image* input, Image* output)
{
  //  printf("\n");
/* TODO:  Merge yellow blobs together before checking whether any contain
          black blobs, turn merging by intersection into a function of blobs
          or blob detector or something, because its really useful.
          
          also... fix all the damn false positives on the duct detection...*/
    BlobDetector::Blob empty(0,0,0,0,0,0,0);
    
    m_fullDuct = empty;
    m_working->copyFrom(input);
    m_workingPercents->copyFrom(m_working);
    m_blackMasked->copyFrom(m_working);
    m_yellowMasked->copyFrom(m_working);
    to_ratios(m_workingPercents->asIplImage());
    m_possiblyAligned = false;
    
    // Grab data pointers
    unsigned char* outputData = 0;
    unsigned char* data = m_working->getData();
    unsigned char* yellowData = m_yellowMasked->getData();
    int width = m_working->getWidth();
    int height = m_working->getHeight();
//    int minX = 1000, minY = 1000, maxX = -10000, maxY = -10000;
    
    black_mask(m_workingPercents->asIplImage(),
	       m_working->asIplImage(), 
	       m_blackMasked->asIplImage(),
               m_minBlackPercent, 
	       m_maxBlackTotal);
    
    int count = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            //if (yellow(data[count+2],data[count+1],data[count]))
            if (yellow(data[count+2],data[count+1],data[count],
		       m_minRedOverGreen, m_maxRedOverGreen, m_minRedOverBlue,
		       m_minGreenOverBlueOnRedFailureForInsideDuct,
		       m_maxRedFailureThresh, m_minTotalRGB, 
		       m_minTotalRGBOnFailure))
            {
                yellowData[count]=yellowData[count+1]=yellowData[count+2] = 255;
            }
            else
            {
                yellowData[count]=yellowData[count+1]=yellowData[count+2] = 0;            
            }
            count+=3;
        }
    }
//    Image::showImage(m_yellowMasked);

// erode the image (get rid of stray white pixels)
    if (m_erodeIterations != 0)
    {
        cvErode(m_blackMasked->asIplImage(), m_blackMasked->asIplImage(), 0,
                m_erodeIterations);
    }
    if (m_dilateIterations != 0)
    {
        cvDilate(m_yellowMasked->asIplImage(), 
		 m_yellowMasked->asIplImage(),
		 0,
		 m_dilateIterations);
    }
    //for debug
    if (output)
    {
        output->copyFrom(m_working);
        outputData = output->getData();
        unsigned char* blackData = m_blackMasked->getData();

        int count = 0;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (yellowData[count] == 255)
                {
                    outputData[count+1]=outputData[count+2]=255;
                    outputData[count]=0;
                }
                else
                {
                    outputData[count]=outputData[count+1]=outputData[count+2]=0;
                }
                count+=3;
            }
        }
        count=0;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (blackData[count] == 255 && outputData[count+1]==0)
                {
                    outputData[count]=outputData[count+1]=outputData[count+2]=255;
                }
                count+=3;
            }
        }
    }
    
    blobDetector.setMinimumBlobSize(500);
    blobDetector.processImage(m_yellowMasked);
    BlobDetector::BlobList yellowBlobs = blobDetector.getBlobs();
        
    blobDetector.setMinimumBlobSize(750);
    blobDetector.processImage(m_blackMasked);
    BlobDetector::BlobList blackBlobs = blobDetector.getBlobs();
    
    if (output)
    {
              BOOST_FOREACH(BlobDetector::Blob blob, blackBlobs)
        {
            blob.draw(output);
        }
        BOOST_FOREACH(BlobDetector::Blob blob, yellowBlobs)
        {
            blob.draw(output);
        }
    }
    

    int minEntranceX = 10000;
    int minEntranceY = 10000;
    int maxEntranceX = -10000;
    int maxEntranceY = -10000;
    containsOne = false;

    std::vector<BlobDetector::Blob> yellowBlobMerger;
    std::vector<BlobDetector::Blob> yellowBlobMergeResults;
    
    BOOST_FOREACH(BlobDetector::Blob blob, yellowBlobs)
    {
        yellowBlobMerger.push_back(blob);
    }
    
    mergeBlobs(&yellowBlobMerger, &yellowBlobMergeResults);

    BOOST_FOREACH(BlobDetector::Blob blackBlob, blackBlobs)
    {
        BOOST_FOREACH(BlobDetector::Blob yellowBlob, yellowBlobMergeResults)
        {
            if (yellowBlob.containsInclusive(blackBlob,3))
            {
                containsOne = true;
                minEntranceX = std::min(minEntranceX, blackBlob.getMinX());
                minEntranceY = std::min(minEntranceY, blackBlob.getMinY());
                maxEntranceX = std::max(maxEntranceX, blackBlob.getMaxX());
                maxEntranceY = std::max(maxEntranceY, blackBlob.getMaxY());
            }
        }
    }
    
    if (output)
    {
        CvPoint tl,tr,bl,br;
        tl.x = bl.x = std::max(minEntranceX,0) - 1;
        tr.x = br.x = std::min(maxEntranceX,width-1) + 1;
        tl.y = tr.y = std::min(minEntranceY,height-1) + 1;
        br.y = bl.y = std::max(maxEntranceY,0) - 1;
        
        IplImage* raw = output->asIplImage();
        cvLine(raw, tl, tr, CV_RGB(255,0,0), 3, CV_AA, 0 );
        cvLine(raw, tl, bl, CV_RGB(255,0,0), 3, CV_AA, 0 );
        cvLine(raw, tr, br, CV_RGB(255,0,0), 3, CV_AA, 0 );
        cvLine(raw, bl, br, CV_RGB(255,0,0), 3, CV_AA, 0 );
    }
    
    if (containsOne || (yellowBlobs.size() >=2 && blackBlobs.size() >= 1))
    {
        m_found = true;
    }
    else
    {
        if (m_found)
        {
            DuctEventPtr event(new DuctEvent(0, 0, 0, 0, false, false));
            publish(EventType::DUCT_LOST, event);
            m_found = false;
        }
    }
    
    std::vector<BlobDetector::Blob> allBlobs;
    BOOST_FOREACH(BlobDetector::Blob blob, blackBlobs)
    {
        allBlobs.push_back(blob);
    }
    BOOST_FOREACH(BlobDetector::Blob blob, yellowBlobs)
    {
        allBlobs.push_back(blob);
    }

    std::vector<BlobDetector::Blob> resultBlobs;
    mergeBlobs(&allBlobs, &resultBlobs);
    
    BOOST_FOREACH(BlobDetector::Blob blob, resultBlobs)
    {
        if (m_fullDuct.getSize() < blob.getSize())
            m_fullDuct = blob;
    }
    
    if (m_found && containsOne)
    {
        //Seeing the entrance must set the rotation to rotate us to the entrance.
        //If we also see the exit, we will override this rotation to aim us through
        //containsOne is true, so we see entrance!
        m_rotation = (m_fullDuct.getCenterX() - ((minEntranceX + maxEntranceX) / 2));
	
	//Draw lines to all edges of the bin, see how far you get before you hit
	//a side (yellow or black)
	//If you can't go x amount in all directions, youre not really aligned.
	//Alternately, if you hit a wall, you're not really aligned.
	int toTop = 0;
	int toBottom = 0;
	int toLeft = 0;
	int toRight = 0;
	
	unsigned char* yellowData = (unsigned char*) m_yellowMasked->getData();
	unsigned char* blackData = (unsigned char*) m_blackMasked->getData();
	int x = m_fullDuct.getCenterX();
	int y = m_fullDuct.getCenterY();
	for (y = m_fullDuct.getCenterY(); y <= m_fullDuct.getMaxY(); y++)
	{
	    int index = 3 * x + 3 * y * width;
	    if (yellowData[index] == 255 || blackData[index] == 255)
	    {
      	        break;
	    }
	    toBottom++;
	}

	if (y == m_fullDuct.getMaxY()+1) //We hit edge... lame.
        {
	    toBottom = 0;
        }
	
	x = m_fullDuct.getCenterX();
	y = m_fullDuct.getCenterY();
	for (y = m_fullDuct.getCenterY(); y >= m_fullDuct.getMinY(); y--)
	{
	    int index = 3 * x + 3 * y * width;
	    if (yellowData[index] == 255 || blackData[index] == 255)
	    {
      	        break;
	    }
	    toTop++;
	}
	
	if (y == m_fullDuct.getMinY()-1) //We hit edge... lame.
        {
	    toTop = 0;
        }

	x = m_fullDuct.getCenterX();
	y = m_fullDuct.getCenterY();
	for (x = m_fullDuct.getCenterX(); x >= m_fullDuct.getMinX(); x--)
	{
	    int index = 3 * x + 3 * y * width;
	    if (yellowData[index] == 255 || blackData[index] == 255)
	    {
      	        break;
	    }
	    toLeft++;
	}

	if (x == m_fullDuct.getMinX()-1) //We hit edge... lame...
        {
            //This probably means the left side has totally disappeared,
	    //Force it to move a little more to the right.
	    toLeft = 0;
	    m_rotation=-10;
	    if (output)
	    {
	        CvPoint top;
	        CvPoint bottom;
		bottom.x = top.x = m_fullDuct.getMinX();
		top.y = 0;
		bottom.y = height-1;

		cvLine(output->asIplImage(),
		       top, 
		       bottom, 
		       CV_RGB(255,0,0), 3, CV_AA, 0 );
	    }
        }

	x = m_fullDuct.getCenterX();
	y = m_fullDuct.getCenterY();
	for (x = m_fullDuct.getCenterX(); x <= m_fullDuct.getMaxX(); x++)
	{
	    int index = 3 * x + 3 * y * width;
	    if (yellowData[index] == 255 || blackData[index] == 255)
	    {
      	        break;
	    }
	    toRight++;
	}

	if (x == m_fullDuct.getMaxX()+1) //We hit edge... lame.
	{   
	    //This probably means right side has totally disappeared.
	    //Force it to move a little more to the left.
            toRight = 0;
	    m_rotation=10;
	    if (output)
	    {
	        CvPoint top;
	        CvPoint bottom;
		bottom.x = top.x = m_fullDuct.getMaxX();
		top.y = 0;
		bottom.y = height-1;

		cvLine(output->asIplImage(),
		       top, 
		       bottom, 
		       CV_RGB(255,0,0), 3, CV_AA, 0 );
	    }
	}

	if (toLeft > 20 && toRight > 20 && toTop > 20 && toBottom > 20)
	{
	    m_possiblyAligned = true;
	    BlobDetector::Blob exitBlob(0, 
				      m_fullDuct.getCenterX() + (toRight - toLeft)/2,
				      m_fullDuct.getCenterY() + (toBottom - toTop)/2,
				      m_fullDuct.getCenterX() + toRight, 
				      m_fullDuct.getCenterX() - toLeft, 
				      m_fullDuct.getCenterY() + toBottom,
				      m_fullDuct.getCenterY() - toTop);

	    //Seeing the exit blob will override the rotation necessary
	    m_rotation = -(m_fullDuct.getCenterX() - exitBlob.getCenterX());
	    
	    if (output)
	    {
     	        exitBlob.draw(output);
		CvPoint exitCenter;
		exitCenter.x = exitBlob.getCenterX();
		exitCenter.y = exitBlob.getCenterY();
		cvCircle(output->asIplImage(), 
			 exitCenter,
			 10, 
			 CV_RGB(128,128,128), 
			 2, 
			 CV_AA, 
			 0);
	    }
        }
	else
	{
	    m_possiblyAligned = false;
	}
    }
    else
        m_rotation = 0;
                
    if (output)
    {
        m_fullDuct.draw(output);
    }
        
    if (m_found && output)
    {
        // Draw center of bin
        CvPoint binCenter;
        binCenter.x = (int)m_fullDuct.getCenterX();
        binCenter.y = (int)m_fullDuct.getCenterY();
        if (getAligned() && getVisible())
            cvCircle(output->asIplImage(), binCenter, 10, CV_RGB(0,255,0), 2, CV_AA, 0);
        else
            cvCircle(output->asIplImage(), binCenter, 10, CV_RGB(255,0,0), 2, CV_AA, 0);

        // Draw rotation indicator
        CvPoint rotationEnd;
        rotationEnd.y = binCenter.y;
        rotationEnd.x = (int)(binCenter.x - m_rotation);
        if (getAligned() && getVisible())
        {
            cvLine(output->asIplImage(), binCenter, rotationEnd, CV_RGB(0,255,0), 3, CV_AA, 0 );
        }
        else
            cvLine(output->asIplImage(), binCenter, rotationEnd, CV_RGB(255,0,0), 3, CV_AA, 0 );
    }

    if (m_found)
    {
        // Shift origin to the center
        //n_x = -1 * ((input->getWidth() / 2) - m_fullDuct.getCenterX());
        //n_y = (input->getHeight() / 2) - m_fullDuct.getCenterY();
        // These temps needed because of some wierd overflow issue
        int temp = (input->getHeight() / 2);
        int temp2 = temp - m_fullDuct.getCenterY();
        n_y = temp2;
        temp = (input->getWidth() / 2);
        temp2 = temp - m_fullDuct.getCenterX();
        n_x = -1 * temp2;

        // Normalize (-1 to 1)
        n_x = n_x / ((double)(input->getWidth())) * 2.0;
        n_y = n_y / ((double)(input->getHeight())) * 2.0;

        // Account for the aspect ratio difference
        // 640/480
        n_x *= (double)input->getWidth() / input->getHeight();

        // Calculate range
        m_range = 1 - ((double)(m_fullDuct.getMaxY() - m_fullDuct.getMinY()) / (double)input->getHeight()); 
        
        DuctEventPtr event(new DuctEvent(n_x, n_y, m_range, m_rotation, 
                                         getAligned(), getVisible()));
        publish(EventType::DUCT_FOUND, event);
    }


    

    
    //    // convert to the crazy coordinate system
    //    n_x = -1 * ((width / 2) - m_x);
    //    n_y = (height / 2) - m_y;
    //    n_x = n_x / ((double)width) * 2.0;
    //    n_y = n_y / ((double)height) * 2.0;
    //    n_x *= (double)width/height;
    //        
    //    // publish found event
    //    DuctEventPtr event(new DuctEvent(n_x, n_y, m_range, m_rotation, 
    //        getAligned(), getVisible()));
    //    publish(EventType::DUCT_FOUND, event);
}
    
double DuctDetector::getX()
{
    return n_x;
}

double DuctDetector::getY()
{
    return n_y;
}

double DuctDetector::getRange()
{
    return m_range;
}
    
double DuctDetector::getRotation()
 {
    return m_rotation;
}

bool DuctDetector::getVisible()
{
    return m_x >= 0 && m_found;
}
    
bool DuctDetector::getAligned()
{
  //    printf("%d, %d, %f, %d, %d, %d\n\n", m_found, m_possiblyAligned, m_rotation, m_fullDuct.getCenterX(), m_fullDuct.getCenterY(), containsOne);
    
    return m_found && m_possiblyAligned && 
        fabsf(m_rotation) < m_alignedThreshold &&
        abs(m_fullDuct.getCenterX() - m_working->getWidth()/2) < m_centerAlignedThreshold &&
        abs(m_fullDuct.getCenterY() - m_working->getHeight()/2) < m_centerAlignedThreshold &&
        containsOne;
}

} // namespace vision
} // namespace ram

