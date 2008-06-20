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

//Allow testRecord to be called easily from within a dynamically linked library

//-1 on failure from too many distinct pieces, 0 if nothing at all was found,
//otherwise returns number of pixels in the largest connected white splotch in the image
//and fills centerX and centerY with its center.
//	cout<<"starting histogram, beware."<<endl;
	int width=img->width;
	int height=img->height;
	unsigned char* data=(unsigned char*)img->imageData;
	int* pixelCounts=(int*) calloc(254,sizeof(int));
	//Might need longs for these two if we increase image size above 1024x1024, otherwise a full white image might overflow
	int* totalX=(int*) calloc(254,sizeof(int));
	int* totalY=(int*) calloc(254,sizeof(int));

        // To keep track of mins and maxes of the bounds of blobs
        int* totalMaxX=(int*) calloc(254, sizeof(int));
        int* totalMaxY=(int*) calloc(254, sizeof(int));
        int* totalMinX=(int*) malloc(254 * sizeof(int));
        int* totalMinY=(int*) malloc(254 * sizeof(int));
        
	unsigned char* joins=(unsigned char*) malloc(254*sizeof(unsigned char));
	for (unsigned char i=0; i<254; i++)
        {
		joins[i]=i;
                totalMinX[i] = img->width;
                totalMinY[i] = img->height;
        }
	joins[0]=255;
	
	int index=1;
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
					pixelCounts[index]=1;
					totalX[index]=x;
					totalY[index]=y;
					data[count]=(unsigned char)(index++);
//					cout<<index<<endl;				
					if (index==254)
					{
						free(pixelCounts);
						free(totalX);
						free(totalY);
						free(joins);
						return -1;
					}
				}
				else 
				{
//					cout<<"I'm in part 2"<<endl;
					unsigned char above2=above;
					unsigned char left2=left;
					if (above2==0)
						above2=255;
					else
					{
					while (above2!=joins[above2])
						above2=joins[above2];
					}
					if (left2==0)
						left2=255;
					else
					{
						while (left2!=joins[left2])
							left2=joins[left2];
					}
					
					data[count]=joins[above]=joins[left]=std::min(left2,above2);
					totalX[data[count]]+=x;
					totalY[data[count]]+=y;
					++pixelCounts[data[count]];
//					cout<<"end of part 2"<<endl;
                                        
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
//	cout<<"Made it through image, now creating useful data from arrays"<<endl;
	int maxCount=0;

        // Work from the top to bottom, collapsing the pixel clusters together
	for (int i=index-1;i>0;i--)
	{
                
		if (joins[i]!=i)
		{
                    // "Unfinished" cluster of pixels, ie part of bigger cluster
                    // So add all of its information to the parents information
			totalX[joins[i]]+=totalX[i];
			totalY[joins[i]]+=totalY[i];
			pixelCounts[joins[i]]+=pixelCounts[i];
			pixelCounts[i]=0;
		}
		else
		{
                    // Found a final cluster
                    m_blobs.push_back(
                        BlobDetector::Blob(pixelCounts[i], totalX[i]/maxCount,
                             totalY[i]/maxCount, totalMaxX[i], totalMinX[i],
                             totalMaxY[i], totalMinY[i])
                                    );
		}
	}

	//Deallocate arrays
	free(pixelCounts);
	free(totalX);
	free(totalY);
	free(joins);
        free(totalMinX);
        free(totalMaxX);
        free(totalMinY);
        free(totalMaxY);
//	cout<<"Happily reaching the end of histogram"<<endl;
	return maxCount;
}

} // namespace vision
} // namespace ram
