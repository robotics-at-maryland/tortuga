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
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <iostream>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/BlobDetector.h"

namespace ram {
namespace vision {

BlobDetector::BlobDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub)
{
    init(config);
}

BlobDetector::~BlobDetector()
{
}
    
void BlobDetector::processImage(Image* input, Image* output)
{
    m_blobs.clear();
    histogram(input->asIplImage());

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
    
void BlobDetector::init(core::ConfigNode)
{
}
    
int BlobDetector::histogram(IplImage* img)
{
    int width=img->width;
    int height=img->height;
    unsigned char* data=(unsigned char*)img->imageData;

    joins.resize(1, 255);
    pixelCounts.resize(1, 0);
    totalX.resize(1, 0);
    totalY.resize(1, 0);
    totalMinX.resize(1, 0);
    totalMaxX.resize(1, 0);
    totalMinY.resize(1, 0);
    totalMaxY.resize(1, 0);
        
    int index=1;
    // Black out the top row, front edge so the above and left algos
    // work properly
    int count=0;
    for (int x=0;x<width;x++)
    {
        data[count]=data[count+1]=data[count+2]=0;
        count+=3;
    }
    count=0;
    for (int y=0;y<height;y++)
    {
        data[count]=data[count+1]=data[count+2]=0;
        count+=3*width;
    }
    count=0;

    // Loop over every pixel
    for (int y=0; y<height;y++)
    {
        for (int x=0; x<width;x++)
        {
            if (data[count]>0)
            {
                unsigned char above=data[count-3*width];
                unsigned char left=data[count-3];
                if (above==0 && left==0)
                {
                    //int neededSize = index + 1;
                    //int presentSize = pixelCounts.size();
                    //assert((presentSize + 1) == neededSize);
                
                    pixelCounts.push_back(1);
                    totalX.push_back(x);
                    totalY.push_back(y);
                    totalMinX.push_back(x);
                    totalMaxX.push_back(x);
                    totalMinY.push_back(y);
                    totalMaxY.push_back(y);
                    joins.push_back(index);
                    assert((index + 1) == (int)(totalX.size()));
                    
                    data[count]=(unsigned char)(index++);
                }
                else 
                {
                    //                    cout<<"I'm in part 2"<<endl;
                    unsigned char above2=above;
                    unsigned char left2=left;
                    if (above2==0)
                        above2=255;
                    else
                    {
                        //assert(above2 <= index);
                        //assert((int)joins.size() >= (above2 + 1));
                            
                        while (above2!=joins[above2])
                            above2=joins[above2];
                    }
                    if (left2==0)
                        left2=255;
                    else
                    {
                        //assert(left2 <= index);
                        //assert((int)joins.size() >= (left2 + 1));
                            
                        while (left2!=joins[left2])
                            left2=joins[left2];
                    }

                    // More sanity checks
                    //assert(left <= index);
                    //assert((int)joins.size() >= (left + 1));
                        
                    //assert(above <= index);
                    //assert((int)joins.size() >= (above + 1));
                    
                    data[count]=joins[above]=joins[left]=std::min(left2,above2);

                    // Small sanity checks for refactoring
                    //assert(data[count] <= index);
                    //assert((int)totalX.size() >= (data[count] + 1));
                        
                    totalX[data[count]]+=x;
                    totalY[data[count]]+=y;
                    ++pixelCounts[data[count]];
                                        
                    // Min/Max
                    if (x < totalMinX[data[count]])
                        totalMinX[data[count]] = x;
                    else if (x > totalMaxX[data[count]])
                        totalMaxX[data[count]] = x;
                    if (y < totalMinY[data[count]])
                        totalMinY[data[count]] = y;
                    else if (y > totalMaxY[data[count]])
                        totalMaxY[data[count]] = y;
                }
            }
            count+=3;
        }
    }
    //    cout<<"Made it through image, now creating useful data from arrays"<<endl;
    int maxCount=0;
  
        // Work from the top to bottom, collapsing the pixel clusters together
    for (int i=index-1;i>0;i--)
    {
        int join = joins[i];
        //assert(join <= index);
        if (join!=i)
        {
            // "Unfinished" cluster of pixels, ie part of bigger cluster
            // So add all of its information to the parents information
            totalX[join]+=totalX[i];
            totalY[join]+=totalY[i];

            // Mins
            if (totalMinX[i] < totalMinX[join])
                totalMinX[join] = totalMinX[i];
            if (totalMinY[i] < totalMinY[join])
                totalMinY[join] = totalMinY[i];

            // Maxs
            if (totalMaxX[i] > totalMaxX[join])
                totalMaxX[join] = totalMaxX[i];
            if (totalMaxY[i] > totalMaxY[join])
                totalMaxY[join] = totalMaxY[i];

            pixelCounts[join]+=pixelCounts[i];
            pixelCounts[i]=0;
        }
        else
        {
            maxCount=pixelCounts[i];
            // Found a final cluster
            m_blobs.push_back(
                   BlobDetector::Blob(pixelCounts[i], totalX[i]/maxCount,
                             totalY[i]/maxCount, totalMaxX[i], totalMinX[i],
                             totalMaxY[i], totalMinY[i])
                                    );
        }
    }

    //Deallocate arrays
    //    cout<<"Happily reaching the end of histogram"<<endl;
    return maxCount;
}

} // namespace vision
} // namespace ram
