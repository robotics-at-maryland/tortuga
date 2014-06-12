/*
  (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/SafeDetector.cpp
 */

// STD Includes
#include <iostream>
#include <algorithm>
#include <vector>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include <log4cpp/Category.hh>
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/SafeDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Vision"));

namespace ram {
namespace vision {

SafeDetector::SafeDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_safeBlob(0,0,0,0,0,0,0),
    blobDetector(config,eventHub)
{
    init(config);
}
    
void SafeDetector::init(core::ConfigNode config)
{
    m_found = false;
    m_safeX = 0;
    m_safeY = 0;
    m_working = new OpenCVImage(640,480);
    m_rOverGMin = config["rOverGMin"].asDouble(.5);
    m_rOverGMax = config["rOverGMax"].asDouble(1.8);
    m_bOverRMax = config["bOverRMax"].asDouble(.7);
    m_erodeIterations = config["erodeIterations"].asInt(0);
    m_dilateIterations = config["dilateIterations"].asInt(0);
    m_minTotal = config["minTotal"].asInt(100);
}
    
    SafeDetector::~SafeDetector()
    {
        delete m_working;
    }

    bool SafeDetector::blobsAreClose(BlobDetector::Blob b1, BlobDetector::Blob b2, double growThresh)
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
        
        BlobDetector::Blob b1Bigger((int)(b1.getSize()*growThresh),
                                    b1.getCenterX(),
                                    b1.getCenterY(),
                                    maxX + (int)(b1Width * growThresh),
                                    minX - (int)(b1Width * growThresh),
                                    maxY + (int)(b1Height * growThresh),
                                    minY - (int)(b1Height * growThresh));
        
        BlobDetector::Blob b2Bigger((int)(b2.getSize()*growThresh),
                                    b2.getCenterX(),
                                    b2.getCenterY(),
                                    maxX2 + (int)(b2Width * growThresh),
                                    minX2 - (int)(b2Width * growThresh),
                                    maxY2 + (int)(b2Height * growThresh),
                                    minY2 - (int)(b2Height * growThresh));

        return b1Bigger.boundsIntersect(b2Bigger);
    }
    
void SafeDetector::processImage(Image* input, Image* out)
{
    BlobDetector::Blob empty(0,0,0,0,0,0,0);

    m_working->copyFrom(input);
	
    if (m_erodeIterations > 0)
      {
	cvErode(m_working->asIplImage(),
	    m_working->asIplImage(),
	    0,
	    m_erodeIterations);
      }
    if (m_dilateIterations > 0)
      {
	cvDilate(m_working->asIplImage(), 
		 m_working->asIplImage(),
		 0, 
		 m_dilateIterations);
      }
    safeMask(m_working->asIplImage(), m_rOverGMin, m_rOverGMax, m_bOverRMax, m_minTotal);
    if (out)
        out->copyFrom(m_working);

    blobDetector.setMinimumBlobSize(100);
    blobDetector.processImage(m_working);
    BlobDetector::BlobList blobs = blobDetector.getBlobs();
    
    if (blobs.size() == 0)
    {
        if (m_found)
        {
            m_safeX = 0;
            m_safeY = 0;
            m_safeBlob = empty;
            m_found = false;
            publish(EventType::SAFE_LOST, core::EventPtr(new core::Event()));
        }
        return;
    }
    std::vector<BlobDetector::Blob> orangeBlobs;
    
    int totalPixels = 0;
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        totalPixels += blob.getSize();    
    }
    int avgPixels = totalPixels / blobs.size();
    
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        if (blob.getSize() < avgPixels * 2.5)
            orangeBlobs.push_back(blob);
    }    
    
    if (out)
    {
        BOOST_FOREACH(BlobDetector::Blob blob, orangeBlobs)
        {
//        printf("Center: %d, %d \n", blob.getCenterX(), blob.getCenterY());
            blob.draw(out);
        }
    }
    if (orangeBlobs.size() >= 6)
    {
        //Possibly found treasure.
        int size = orangeBlobs.size();
        int* blobUnionizer = new int[size];
        BlobDetector::Blob* unionBlobs = new BlobDetector::Blob[size];
        int* blobCounts = new int[size];
        for (int i = 0; i < size; i++)
        {
            blobUnionizer[i] = i;
            unionBlobs[i] = orangeBlobs[i];
            blobCounts[i] = 1;
        }
        
        for (int i = 0; i < size; i ++)
        {
            for (int j = i+1; j < size; j++)
            {
                if (blobsAreClose(orangeBlobs[i],orangeBlobs[j], 1.0))
                {
                    int rooti = i;
                    int rootj = j;
                    
                    while (rooti != blobUnionizer[rooti])
                        rooti = blobUnionizer[rooti];

                    while (rootj != blobUnionizer[rootj])
                        rootj = blobUnionizer[rootj];

                    int minRoot = (rooti < rootj)? rooti : rootj;

                    blobUnionizer[i] = blobUnionizer[j] = minRoot;
                }
            }
        }

        int biggestCluster = -1;
        int biggestClusterBlobCount = 0;
        for (int i = size-1; i >=0; i--)
        {
            if (blobUnionizer[i] == i)
            {
                if (blobCounts[i] > biggestClusterBlobCount)
                {
                    biggestClusterBlobCount = blobCounts[i];
                    biggestCluster = i;
                }
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
        
        if (biggestClusterBlobCount >= 5)
        {
            m_safeX = unionBlobs[biggestCluster].getCenterX();
            m_safeY = unionBlobs[biggestCluster].getCenterY();
            m_safeBlob = unionBlobs[biggestCluster];
            m_found = true;
            if (out)
                m_safeBlob.draw(out);
            
            CvPoint safeCenter;
            safeCenter.x = m_safeBlob.getCenterX();
            safeCenter.y = m_safeBlob.getCenterY();
            if (out)
                cvCircle(out->asIplImage(), safeCenter, 10, CV_RGB(255,128,255), 2, CV_AA, 0);

            m_safeX = -1 * ((m_working->getWidth()/2) - m_safeX);
            m_safeY = m_working->getHeight()/2 - m_safeY;

            m_safeX = m_safeX / ((double)(m_working->getWidth()));
            m_safeY = m_safeY / ((double)(m_working->getHeight()));
           
            m_safeX *=2;
            m_safeY *=2;
            
            m_safeX *= (double)((m_working->getWidth())/(double)(m_working->getHeight()));
            SafeEventPtr event(new SafeEvent(m_safeX, m_safeY));
            publish(EventType::SAFE_FOUND, event);
        }
        else if (m_found)
        {
            m_safeX = 0;
            m_safeY = 0;
            m_safeBlob = empty;
            m_found = false;
            publish(EventType::SAFE_LOST, core::EventPtr(new core::Event()));
        }

        delete[] blobUnionizer;
        delete[] unionBlobs;
        delete[] blobCounts;
    }
}

double SafeDetector::getX()
{
    if (m_found)
        return m_safeX;
    return 0;
}

double SafeDetector::getY()
{
    if (m_found)
        return m_safeY;
    return 0;
}

bool SafeDetector::found()
{
    return m_found;
}
    
}//vision
}//ram
