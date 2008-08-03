/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/AdaptiveThresher.cpp
 */

#include "map"
#include "cv.h"
#include "highgui.h"
#include "vision/include/Image.h"
#include "vision/include/AdaptiveThresher.h"
#include "core/include/ConfigNode.h"
#include "vision/include/Events.h"
#include "vision/include/Common.h"

//Include me last.
#include "vision/include/Export.h"



//Help Help, I'm a massive hack written starting at 10 pm the day before the second qualifiers, fix me later!
namespace ram {
namespace vision {

AdaptiveThresher::AdaptiveThresher(core::ConfigNode config,
                 core::EventHubPtr eventHub) : Detector(eventHub), m_working(640,480)//, m_lightDetector(config,eventHub)
{
    pixelSearchRange = 2;
    setUnionTable = (int*)malloc(640*480*sizeof(int));
    finalColorTableR = (float*)malloc(640*480*sizeof(float));
    finalColorTableG = (float*)malloc(640*480*sizeof(float));
    finalColorTableB = (float*)malloc(640*480*sizeof(float));
    pixelCounts = (int*)malloc(640*480*sizeof(int));
    curWidth = 640;
    curHeight = 480;
    distThreshSquared = 50;
//    cannied = cvCreateImage(CvSize(640,480), 8, 1);
}

AdaptiveThresher::~AdaptiveThresher()
{
    free(setUnionTable);
}

void AdaptiveThresher::processImage(Image* in, Image* out)
{
    m_working.copyFrom(out);
    segmentImage(in,&m_working);
//    m_lightDetector.processImage(&m_working, out);
    findCircle();
    out->copyFrom(&m_working);
}

void AdaptiveThresher::segmentImage(Image* in, Image* out)
{
//    printf("Beginning Adaptive Thresholding\n");
    if (in == NULL || 
        out == NULL || 
        out->getWidth() != in->getWidth() || 
        out->getHeight() != in->getHeight())
    {
        //printf("segment image MUST have two non null input images of the same size!\n");
        return;
    }
    //printf("Initializing\n");
    int width = in->getWidth();
    int height = in->getHeight();
    unsigned char * data = (unsigned char *) in->getData();

    if (curWidth != width || curHeight != height)
    {
        free(setUnionTable);
        setUnionTable = (int*)malloc(width * height * sizeof(int));
        finalColorTableR = (float*)malloc(width*height*sizeof(float));
        finalColorTableG = (float*)malloc(width*height*sizeof(float));
        finalColorTableB = (float*)malloc(width*height*sizeof(float));
        pixelCounts = (int*)malloc(width*height*sizeof(int));
        curWidth = width;
        curHeight = height;
    }
    
    int index = 0;
    int count = 0;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            setUnionTable[index]=index;
            pixelCounts[index] = 1;
            finalColorTableB[index] = (float)data[count];
            finalColorTableG[index] = (float)data[count+1];
            finalColorTableR[index] = (float)data[count+2];
            ++index;
            count += 3;
        }
    
    //printf("Finished Initialization\n");

    //printf("Starting massive loop\n");
    int indexIntoUnionTable = pixelSearchRange + pixelSearchRange * width;
    count = pixelSearchRange * 3 + pixelSearchRange * width * 3;
    for (int y = pixelSearchRange; y < height; y++)
    {
        //printf("Starting massive loop outer iteration number: %d\n", y);
        for (int x = pixelSearchRange; x < width; x++)
        {
            //printf("Starting massive loop inner iteration number: %d\n", x);
            for (int top = y-pixelSearchRange; top <=y; top++)
            {
                for (int left = x - pixelSearchRange; left <=x; left++)
                {
                    int otherPixelIndexIntoUnionTable = indexIntoUnionTable - ((y - top) * width) - (x - left);
                    int otherPixelIndex = otherPixelIndexIntoUnionTable * 3;
                    int b = data[count];
                    int g = data[count+1];
                    int r = data[count+2];
                    
                    int b2 = data[otherPixelIndex];
                    int g2 = data[otherPixelIndex+1];
                    int r2 = data[otherPixelIndex+2];
                    int distSquared = (b - b2) * (b - b2) + (g - g2) * (g - g2) * (r - r2) * (r - r2);
                    
                    if (distSquared <= distThreshSquared)
                    {
                        //Join these pixels.
                        int parentIndex = indexIntoUnionTable;
                        int parentIndexOther = otherPixelIndexIntoUnionTable;
                        //printf("Finding parents\n");
                        while (parentIndex != setUnionTable[parentIndex])
                        {
                            parentIndex = setUnionTable[parentIndex];
                        }
                        while (parentIndexOther != setUnionTable[parentIndexOther])
                        {
                            parentIndexOther = setUnionTable[parentIndexOther];
                        }
                        //printf("Finding parents Complete\n");
                        int finalIndex = parentIndex < parentIndexOther ? parentIndex : parentIndexOther;
                        
                        parentIndex = indexIntoUnionTable;
                        parentIndexOther = otherPixelIndexIntoUnionTable;
                        

                        //Collapse union set.
                        //printf("Collapsing\n");
                        int temp = 0;
                        while (parentIndex != setUnionTable[parentIndex])
                        {
                            temp = parentIndex;
                            parentIndex = setUnionTable[parentIndex];
                            setUnionTable[temp] = finalIndex;
                        }
                        while (parentIndexOther != setUnionTable[parentIndexOther])
                        {
                            temp = parentIndexOther;
                            parentIndexOther = setUnionTable[parentIndexOther];
                            setUnionTable[temp] = finalIndex;
                        }
                        setUnionTable[parentIndex] = setUnionTable[parentIndexOther] = finalIndex;
                        //printf("Finished collapsing\n");
                    }
                }
            }
            count += 3;
            ++indexIntoUnionTable;
        }
    }

    //printf("Finished massive loop\n");
    //printf("Creating maps\n");
    std::map<int, float> colorsToIndicesR;
    std::map<int, float> colorsToIndicesG;
    std::map<int, float> colorsToIndicesB;
    indexIntoUnionTable = height * width-1;

    int biggestColorIndex = 0;

    for (int y = height-1; y >= 0; y--)
    {
        for (int x = width-1; x >= 0; x--)
        {
            if (setUnionTable[indexIntoUnionTable] == indexIntoUnionTable)
            {
//                printf("Found color set\n");
                colorsToIndicesR[indexIntoUnionTable] = finalColorTableR[indexIntoUnionTable] / 
                                                             pixelCounts[indexIntoUnionTable];

                colorsToIndicesG[indexIntoUnionTable] = finalColorTableG[indexIntoUnionTable] / 
                                                             pixelCounts[indexIntoUnionTable];

                colorsToIndicesB[indexIntoUnionTable] = finalColorTableB[indexIntoUnionTable] / 
                                                             pixelCounts[indexIntoUnionTable];
                if (pixelCounts[biggestColorIndex] < pixelCounts[indexIntoUnionTable])
                    biggestColorIndex = indexIntoUnionTable;
            }
            else 
            {
                //create pixel counts
                pixelCounts[setUnionTable[indexIntoUnionTable]] += pixelCounts[indexIntoUnionTable];
                finalColorTableR[setUnionTable[indexIntoUnionTable]] += 
                     finalColorTableR[indexIntoUnionTable];
                finalColorTableG[setUnionTable[indexIntoUnionTable]] += 
                     finalColorTableG[indexIntoUnionTable];
                finalColorTableB[setUnionTable[indexIntoUnionTable]] += 
                     finalColorTableB[indexIntoUnionTable];

                //Collapse union sets
                int finalParent = indexIntoUnionTable;
                int parent = indexIntoUnionTable;
                while (parent != setUnionTable[parent])
                {
                    parent = setUnionTable[parent];
                }
                finalParent = parent;
                
                int temp = 0;
                parent = indexIntoUnionTable;
                while (parent != setUnionTable[parent])
                {
                    temp = parent;
                    parent = setUnionTable[parent];
                    setUnionTable[temp] = finalParent;
                }
            }
            indexIntoUnionTable--;
        }
    }
    //printf("Coloring output image\n");
    unsigned char* data2 = out->getData();
    indexIntoUnionTable = 0;
    count = 0;
//    unsigned char backgroundR = colorsToIndicesR[biggestColorIndex];
//    unsigned char backgroundG = colorsToIndicesG[biggestColorIndex];
//    unsigned char backgroundB = colorsToIndicesB[biggestColorIndex];
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            
            data2[count] = colorsToIndicesB[setUnionTable[indexIntoUnionTable]];
            data2[count+1] = colorsToIndicesG[setUnionTable[indexIntoUnionTable]];
            data2[count+2] = colorsToIndicesR[setUnionTable[indexIntoUnionTable]];
//            if (colorsToIndicesB[setUnionTable[indexIntoUnionTable]] >= backgroundB)
//	        data2[count] = colorsToIndicesB[setUnionTable[indexIntoUnionTable]] - backgroundB;
//            else
//                data2[count] = 0;
//            
//            if (colorsToIndicesG[setUnionTable[indexIntoUnionTable]] >= backgroundG)
//                data2[count+1] = colorsToIndicesG[setUnionTable[indexIntoUnionTable]] - backgroundG;
//            else
//                data2[count+1] = 0;
//
//            if (colorsToIndicesR[setUnionTable[indexIntoUnionTable]] >= backgroundR)
//                data2[count+2] = colorsToIndicesR[setUnionTable[indexIntoUnionTable]] - backgroundR;
//            else
//                data2[count+2] = 0;

//            int total = data2[count] + data2[count+1] + data2[count+2];
//            float bPercent = data2[count]/(total+1.0);
//            float gPercent = data2[count+1]/(total+1.0);
//            float rPercent = data2[count+2]/(total+1.0);
            
//            data2[count] = 255.0 * (bPercent);
//            data2[count+1] = 255.0 * (gPercent);
//            data2[count+2] = 255.0 * (rPercent);
            ++indexIntoUnionTable;
            count += 3;
        }
    }
}

void AdaptiveThresher::findCircle()
{
    IplImage* img = cvCreateImage(cvSize(m_working.getWidth(), m_working.getHeight()), 8, 1);
    CvMemStorage* storage = cvCreateMemStorage(0);
    unsigned char * data = (unsigned char *)m_working.getData();
    unsigned char * data2 = (unsigned char *)img->imageData;
    int len = m_working.getWidth() * m_working.getHeight() * 3;
    
    int count = 0;
    for (int i = 2; i < len ; i+=3)
    {
        data2[count] = data[i];
        count++;
    }
    CvSeq* circles = cvHoughCircles(img, storage, CV_HOUGH_GRADIENT, 2, 75, 200, 100);
    
    for (int i = 0; i < circles->total; i++)
    {
        float* p = (float*) cvGetSeqElem(circles,i);
        cvCircle(m_working, cvPoint(cvRound(p[0]), cvRound(p[1])),p[2], CV_RGB(255,0,0), 3, 8, 0);
    }

    cvReleaseImage(&img);
}

}//vision
}//ram


