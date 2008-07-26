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

bool DuctDetector::blobsAreClose(BlobDetector::Blob b1, BlobDetector::Blob b2, double growThresh)
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


int DuctDetector::yellow(unsigned char r, unsigned char g, unsigned char b)
{
    //Yellow
    if ((g * m_minRedOverGreen <= r) &&
        (g * m_maxRedOverGreen >= r) &&
        (b * m_minRedOverBlue <= r) &&
        (r + b + g > 125))
    {
        return 1;
    }
    else if ((r <= 50) && (r < b) &&
             (g >= (b * m_minGreenOverBlueOnRedFailureForInsideDuct)) &&
             (r+b+g > 150))
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
    fullDuct(0,0,0,0,0,0,0),
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
    fullDuct(0,0,0,0,0,0,0),
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

void DuctDetector::init(core::ConfigNode config)
{
    // get the threshold values from the config file
    m_minRedOverGreen = config["minRedOverGreen"].asDouble(0.5);
    m_maxRedOverGreen = config["maxRedOverGreen"].asDouble(1.5);
    m_minRedOverBlue = config["minRedOverBlue"].asDouble(1.0);
    m_minGreenOverBlueOnRedFailureForInsideDuct =
        config["minGreenOverBlueOnRedFailureForInsideDuct"].asDouble(1.1);
    
    m_erodeIterations = config["erodeIterations"].asInt(3);
    m_alignedThreshold = config["alignedThreshold"].asDouble(5);
    m_centerAlignedThreshold = config["centerAlignedThreshold"].asInt(25);
}
    
void DuctDetector::processImage(Image* input, Image* output)
{
/* TODO:  Merge yellow blobs together before checking whether any contain
          black blobs, turn merging by intersection into a function of blobs
          or blob detector or something, because its really useful.
          
          also... fix all the damn false positives on the duct detection...*/
    BlobDetector::Blob empty(0,0,0,0,0,0,0);
    m_working->copyFrom(input);
    m_workingPercents->copyFrom(m_working);
    m_blackMasked->copyFrom(m_working);
    m_yellowMasked->copyFrom(m_working);
    to_ratios(m_workingPercents->asIplImage());
    
    
    // Grab data pointers
    unsigned char* outputData = 0;
    unsigned char* data = m_working->getData();
    unsigned char* yellowData = m_yellowMasked->getData();
    int width = m_working->getWidth();
    int height = m_working->getHeight();
        
//    int minX = 1000, minY = 1000, maxX = -10000, maxY = -10000;
    
    black_mask(m_workingPercents->asIplImage(),m_working->asIplImage(), m_blackMasked->asIplImage(),
               5, 300);
    
    int count = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            //if (yellow(data[count+2],data[count+1],data[count]))
            if (yellow(data[count+2],data[count+1],data[count]))
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
    cvDilate(m_yellowMasked->asIplImage(), m_yellowMasked->asIplImage(), 0, 3);

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
    BOOST_FOREACH(BlobDetector::Blob blackBlob, blackBlobs)
    {
        BOOST_FOREACH(BlobDetector::Blob yellowBlob, yellowBlobs)
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

    ///START OF COPY PASTE
    //////////////////////
    
    int size = allBlobs.size();
    int* blobUnionizer = new int[size];
    BlobDetector::Blob* unionBlobs = new BlobDetector::Blob[size];
    int* blobCounts = new int[size];
    for (int i = 0; i < size; i++)
    {
        blobUnionizer[i] = i;
        unionBlobs[i] = allBlobs[i];
        blobCounts[i] = 1;
    }
    
    for (int i = 0; i < size; i ++)
    {
        for (int j = i+1; j < size; j++)
        {
            if (blobsAreClose(allBlobs[i],allBlobs[j], .025))
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
        
    BlobDetector::Blob fullDuct = unionBlobs[biggestCluster];
    
    if (m_found && containsOne)
        m_rotation = (fullDuct.getCenterX() - ((minEntranceX + maxEntranceX) / 2));
    else
        m_rotation = 0;
                
    if (output)
    {
        fullDuct.draw(output);
    }
    
    delete[] blobUnionizer;
    delete[] unionBlobs;
    delete[] blobCounts;    
    
    //////////////////////
    ///END OF COPY PASTE
    
    if (m_found && output)
    {
        // Draw center of bin
        CvPoint binCenter;
        binCenter.x = (int)fullDuct.getCenterX();
        binCenter.y = (int)fullDuct.getCenterY();
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
        //n_x = -1 * ((input->getWidth() / 2) - fullDuct.getCenterX());
        //n_y = (input->getHeight() / 2) - fullDuct.getCenterY();
        // These temps needed because of some wierd overflow issue
        int temp = (input->getHeight() / 2);
        int temp2 = temp - fullDuct.getCenterY();
        n_y = temp2;
        temp = (input->getWidth() / 2);
        temp2 = temp - fullDuct.getCenterX();
        n_x = -1 * temp2;

        // Normalize (-1 to 1)
        n_x = n_x / ((double)(input->getWidth())) * 2.0;
        n_y = n_y / ((double)(input->getHeight())) * 2.0;

        // Account for the aspect ratio difference
        // 640/480
        n_x *= (double)input->getWidth() / input->getHeight();

        // Calculate range
        m_range = 1 - ((double)(fullDuct.getMaxY() - fullDuct.getMinY()) / (double)input->getHeight());
        
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
    return m_found &&
        fabsf(m_rotation) < m_alignedThreshold &&
        abs(fullDuct.getCenterX() - m_working->getWidth()/2) < m_centerAlignedThreshold &&
        abs(fullDuct.getCenterY() - m_working->getHeight()/2) < m_centerAlignedThreshold &&
        containsOne;
}

} // namespace vision
} // namespace ram

