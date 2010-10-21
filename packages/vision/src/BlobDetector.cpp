/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/BlobDetector.cpp
 */

#define _CRT_SECURE_NO_WARNINGS

// STD Includes
#include <math.h>
#include <limits.h>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <sstream>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/BlobDetector.h"

namespace ram {
namespace vision {

void BlobDetector::Blob::draw(Image* output, bool centroid,
			      unsigned char R,
			      unsigned char G,
			      unsigned char B)
{
    CvPoint tl,tr,bl,br;
    
    tl.x = bl.x = m_minX;
    tr.x = br.x = m_maxX;
    
    tl.y = tr.y = m_minY;
    bl.y = br.y = m_maxY;
    
    IplImage* raw = output->asIplImage();
    cvLine(raw, tl, tr, CV_RGB(R, G, B), 3, CV_AA, 0 );
    cvLine(raw, tl, bl, CV_RGB(R, G, B), 3, CV_AA, 0 );
    cvLine(raw, tr, br, CV_RGB(R, G, B), 3, CV_AA, 0 );
    cvLine(raw, bl, br, CV_RGB(R, G, B), 3, CV_AA, 0 );

    if (centroid)
    {
        CvPoint c;
	c.x = m_centerX;
	c.y = m_centerY;
	
	cvCircle(raw, c, 5, CV_RGB(0,255,0), 2, CV_AA, 0); 
    }
}

void BlobDetector::Blob::drawStats(Image* output)
{
    CvPoint tl,tr,bl,br;
    
    tl.x = bl.x = m_minX;
    tr.x = br.x = m_maxX;
    
    tl.y = tr.y = m_minY;
    bl.y = br.y = m_maxY;

    // Aspect ratio
    std::stringstream ss;
    ss.precision(2);
    ss << "A:" << getTrueAspectRatio();
    Image::writeText(output, ss.str(), bl.x, bl.y - 15);

    std::stringstream ss2;
    ss2 << "P:" << m_size;
    Image::writeText(output, ss2.str(), bl.x, bl.y - 30);
}


void BlobDetector::Blob::drawTextUL(Image* output, std::string text,
                                    int xOffset, int yOffset)
{
    Image::writeText(output, text, m_minX + xOffset, m_minY + yOffset);
}

void BlobDetector::Blob::drawTextUR(Image* output, std::string text,
                                    int xOffset, int yOffset)
{
    Image::writeText(output, text, m_maxX + xOffset, m_minY + yOffset);
}

    
BlobDetector::BlobDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_minBlobSize(0),
    m_pixelBlobIndex(0),
    m_imageSize(0)
{
    init(config);
}

BlobDetector::BlobDetector(int minimumBlobSize) :
    Detector(core::EventHubPtr()),
    m_minBlobSize(minimumBlobSize),
    m_pixelBlobIndex(0),
    m_imageSize(0)
{
}
    
BlobDetector::~BlobDetector()
{
    free(m_pixelBlobIndex);
}
    
void BlobDetector::processImage(Image* input, Image* output)
{
    m_blobs.clear();
    buildBlobs(input->asIplImage());

    // Do debug stuff soon
    if (0 != output)
    {
        output->copyFrom(input);
        
        CvPoint boundUR;
        CvPoint boundLL;
        
        BOOST_FOREACH(Blob blob, m_blobs)
        {
            // Draw bounds around blob
            boundUR.x = blob.getMaxX();
            boundUR.y = blob.getMaxY();
            boundLL.x = blob.getMinX();
            boundLL.y = blob.getMinY();

            cvRectangle(output->asIplImage(), boundUR, boundLL,
                        CV_RGB(0,255,0), 2, CV_AA, 0);
        }
    }
}
    
bool BlobDetector::found()
{
    return m_blobs.size() > 0;
}

std::vector<BlobDetector::Blob> BlobDetector::getBlobs()
{
    return m_blobs;
}

void BlobDetector::setMinimumBlobSize(int pixels)
{
    m_minBlobSize = pixels;
}

int BlobDetector::getMinimumBlobSize()
{
    return m_minBlobSize;
}
    
void BlobDetector::init(core::ConfigNode config)
{
    // Pre-allocate memory
    m_joinedBlobIndex.reserve(1024);
    m_pixelCounts.reserve(1024);
    m_blobTotalX.reserve(1024);
    m_blobTotalY.reserve(1024);
    m_blobMinX.reserve(1024);
    m_blobMaxX.reserve(1024);
    m_blobMinY.reserve(1024);
    m_blobMaxY.reserve(1024);

    ensureDataSize(640 * 480);

    m_minBlobSize = config["minBlobSize"].asInt(0);
}
    
int BlobDetector::buildBlobs(IplImage* img)
{
    int width=img->width;
    int height=img->height;
    unsigned char* imgData=(unsigned char*)img->imageData;
    // Make sure the data array is large enough
    ensureDataSize(width * height);
    
    m_joinedBlobIndex.resize(1, UINT_MAX);
    m_pixelCounts.resize(1, 0);
    m_blobTotalX.resize(1, 0);
    m_blobTotalY.resize(1, 0);
    m_blobMinX.resize(1, 0);
    m_blobMaxX.resize(1, 0);
    m_blobMinY.resize(1, 0);
    m_blobMaxY.resize(1, 0);
        
    unsigned int initialBlobIndex = 1;
    // Black out the top row, front edge so the above and left algos
    // work properly
    int imgCount=0;
    int count = 0;

    // Top row
    memset(imgData, 0, width * 3);
    memset(m_pixelBlobIndex, 0, sizeof(*m_pixelBlobIndex) * width);

    // Front edge
    for (int y=0;y<height;y++)
    {
        imgData[imgCount]=imgData[imgCount+1]=imgData[imgCount+2]=0;
        m_pixelBlobIndex[count]=0;
        imgCount+=3*width;
        count += width;
    }
    imgCount=0;
    count = 0;


    // Loop over every pixel
    for (int y=0; y<height;y++)
    {
        for (int x=0; x<width;x++)
        {
            // Check for a marked (ie. white) pixel
            if (imgData[imgCount]>0)
            {
                // Found a valid pixel, check the value above and below it
                unsigned int aboveBlobIndex = m_pixelBlobIndex[count-width];
                unsigned int leftBlobIndex = m_pixelBlobIndex[count-1];
                if (aboveBlobIndex == 0 && leftBlobIndex == 0)
                {
                    // Start of a new blob
                    m_pixelCounts.push_back(1);
                    m_blobTotalX.push_back(x);
                    m_blobTotalY.push_back(y);
                    m_blobMinX.push_back(x);
                    m_blobMaxX.push_back(x);
                    m_blobMinY.push_back(y);
                    m_blobMaxY.push_back(y);

                    // add a new blob index and set the current pixel to belong to that blob
                    m_joinedBlobIndex.push_back(initialBlobIndex);
                    m_pixelBlobIndex[count]= (initialBlobIndex++);
                }
                else 
                {
                    // Continuation of an existing blob
                    unsigned int aboveJoinedBlobIndex = aboveBlobIndex;
                    unsigned int leftJoinedBlobIndex = leftBlobIndex;
                    if (aboveJoinedBlobIndex == 0)
                        aboveJoinedBlobIndex = UINT_MAX;
                    else
                    {
                        while (aboveJoinedBlobIndex != m_joinedBlobIndex[aboveJoinedBlobIndex])
                            aboveJoinedBlobIndex = m_joinedBlobIndex[aboveJoinedBlobIndex];
                    }
                    if (leftJoinedBlobIndex == 0)
                        leftJoinedBlobIndex = UINT_MAX;
                    else
                    {
                        while (leftJoinedBlobIndex != m_joinedBlobIndex[leftJoinedBlobIndex])
                            leftJoinedBlobIndex = m_joinedBlobIndex[leftJoinedBlobIndex];
                    }

                    unsigned int finalBlobIndex = 
                        std::min(leftJoinedBlobIndex, aboveJoinedBlobIndex);

                    // modify the joined blob indices to be the lowest index that this pixel
                    // connects to
                    m_joinedBlobIndex[aboveBlobIndex] = finalBlobIndex;
                    m_joinedBlobIndex[leftBlobIndex] = finalBlobIndex;
                    m_pixelBlobIndex[count]= finalBlobIndex;

                    m_blobTotalX[finalBlobIndex]+=x;
                    m_blobTotalY[finalBlobIndex]+=y;
                    ++m_pixelCounts[finalBlobIndex];
                                        
                    // Min/Max
                    if (x < m_blobMinX[finalBlobIndex])
                        m_blobMinX[finalBlobIndex] = x;
                    else if (x > m_blobMaxX[finalBlobIndex])
                        m_blobMaxX[finalBlobIndex] = x;
                    if (y < m_blobMinY[finalBlobIndex])
                        m_blobMinY[finalBlobIndex] = y;
                    else if (y > m_blobMaxY[finalBlobIndex])
                        m_blobMaxY[finalBlobIndex] = y;
                }
            }
            else
            {
                // We need to zero the data as we go
                m_pixelBlobIndex[count] = 0;
            }
            count++;
            imgCount += 3;
        }
    }

    int maxCount=0;

    // Work from the top to bottom, collapsing the pixel clusters together
    for (unsigned int i=initialBlobIndex-1;i>0;i--)
    {
        unsigned int join = m_joinedBlobIndex[i];

        // Catch sentinal to make the first join, point to the last
        if (join == UINT_MAX)
            join = m_joinedBlobIndex.size() - 1;
        
        //assert(join <= initialBlobIndex);
        if (join!=i)
        {
            // "Unfinished" cluster of pixels, ie part of bigger cluster
            // So add all of its information to the parents information
            m_blobTotalX[join]+=m_blobTotalX[i];
            m_blobTotalY[join]+=m_blobTotalY[i];

            // Mins
            if (m_blobMinX[i] < m_blobMinX[join])
                m_blobMinX[join] = m_blobMinX[i];
            if (m_blobMinY[i] < m_blobMinY[join])
                m_blobMinY[join] = m_blobMinY[i];

            // Maxs
            if (m_blobMaxX[i] > m_blobMaxX[join])
                m_blobMaxX[join] = m_blobMaxX[i];
            if (m_blobMaxY[i] > m_blobMaxY[join])
                m_blobMaxY[join] = m_blobMaxY[i];

            m_pixelCounts[join]+=m_pixelCounts[i];
            m_pixelCounts[i]=0;
        }
        else
        {
            maxCount=m_pixelCounts[i];
            if (maxCount >= m_minBlobSize)
            {
                // Found a final cluster
                m_blobs.push_back(
                    BlobDetector::Blob(m_pixelCounts[i], m_blobTotalX[i]/maxCount,
                                       m_blobTotalY[i]/maxCount, m_blobMaxX[i],
                                       m_blobMinX[i], m_blobMaxY[i], m_blobMinY[i])
                                  );
            }
        }
    }

    // Put largest blob first
    if (m_blobs.size() > 0)
    {
        std::sort(m_blobs.begin(), m_blobs.end(),
                  BlobDetector::BlobComparer::compare);
    }

    return maxCount;
}

void BlobDetector::ensureDataSize(int pixels)
{
    size_t bytes = (size_t)pixels * sizeof(*m_pixelBlobIndex);
    if (m_imageSize < (size_t)pixels)
    {
        m_imageSize = (int)pixels;
        if (m_pixelBlobIndex)
            m_pixelBlobIndex = (unsigned int*)realloc(m_pixelBlobIndex, bytes);
        else
            m_pixelBlobIndex = (unsigned int*)malloc(bytes);
    }
}
    
} // namespace vision
} // namespace ram
