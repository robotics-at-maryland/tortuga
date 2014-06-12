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
}

AdaptiveThresher::~AdaptiveThresher()
{
    free(setUnionTable);
    free(finalColorTableR);
    free(finalColorTableG);
    free(finalColorTableB);
    free(pixelCounts);
}

void AdaptiveThresher::processImage(Image* in, Image* out)
{
    m_working.copyFrom(out);
    segmentImage(in,&m_working);
    findCircle();
    out->copyFrom(&m_working);
}

void AdaptiveThresher::segmentImage(Image* in, Image* out)
{
    if (in == NULL || out == NULL || 
        out->getWidth() != in->getWidth() || 
        out->getHeight() != in->getHeight())
    {
        //printf("segment image MUST have two non null input images of the same size!\n");
        return;
    }

    int width = in->getWidth();
    int height = in->getHeight();
    unsigned char * data = (unsigned char *) in->getData();

    if (curWidth != width || curHeight != height)
    {
        setUnionTable = (int*)realloc(setUnionTable, width * height * sizeof(int));
        finalColorTableR = (float*)realloc(finalColorTableR, width*height*sizeof(float));
        finalColorTableG = (float*)realloc(finalColorTableG, width*height*sizeof(float));
        finalColorTableB = (float*)realloc(finalColorTableB, width*height*sizeof(float));
        pixelCounts = (int*)realloc(pixelCounts, width*height*sizeof(int));
        curWidth = width;
        curHeight = height;
    }
    

    int tableIndex = 0;
    int inputImageIndex = 0;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            // give each pixel a unique index to start
            setUnionTable[tableIndex]=tableIndex;

            // keep track of the number of pixels with each index
            // this is initially 1 since all pixels have a unique index
            pixelCounts[tableIndex] = 1;

            // these initially hold the individual channel values
            // of each pixel
            finalColorTableB[tableIndex] = (float)data[inputImageIndex];
            finalColorTableG[tableIndex] = (float)data[inputImageIndex+1];
            finalColorTableR[tableIndex] = (float)data[inputImageIndex+2];

            // go to a new table index
            ++tableIndex;

            // go to the next pixel
            inputImageIndex += 3;
        }
    
    int indexIntoUnionTable = pixelSearchRange + pixelSearchRange * width;
    inputImageIndex = pixelSearchRange * 3 + pixelSearchRange * width * 3;

    // iterate over the entire image searching an area of pixels defined
    // by the pixelSearchRange x pixelSearchRange
    for (int y = pixelSearchRange; y < height; y++)
    {
        for (int x = pixelSearchRange; x < width; x++)
        {
            for (int relativeYPosition = -pixelSearchRange; relativeYPosition < 0; relativeYPosition++)
            {
                for (int relativeXPosition = -pixelSearchRange; relativeXPosition < 0; relativeXPosition++)
                {
                    // to find the index into the table of the other pixel, we need to correct
                    // for the number of pixels due to wanting to change our y position and 
                    // number of pixels to change our x position
                    int yChangePixels = relativeYPosition * width;
                    int xChangePixels = relativeXPosition;
                    int otherPixelIndexIntoUnionTable = indexIntoUnionTable + yChangePixels + xChangePixels;

                    // remember there are 3 image channels
                    int otherPixelIndex = otherPixelIndexIntoUnionTable * 3;

                    // get the channel values of the central pixel
                    int b = data[inputImageIndex];
                    int g = data[inputImageIndex+1];
                    int r = data[inputImageIndex+2];
                    
                    // get the channel values of the other pixel
                    int b2 = data[otherPixelIndex];
                    int g2 = data[otherPixelIndex+1];
                    int r2 = data[otherPixelIndex+2];

                    // we calculate the square distance between colors in the euclidian sense
                    // there might actually be a better distance metric to use
                    int distSquared = 
                        (b - b2) * (b - b2) + 
                        (g - g2) * (g - g2) + 
                        (r - r2) * (r - r2);
                    
                    // decide if the colors are similar enough
                    if (distSquared <= distThreshSquared)
                    {
                        // Join these pixels.
                        int parentIndex = indexIntoUnionTable;
                        int parentIndexOther = otherPixelIndexIntoUnionTable;

                        if(parentIndex != setUnionTable[parentIndex])
                            parentIndex = setUnionTable[parentIndex];

                        if(parentIndexOther != setUnionTable[parentIndexOther])
                            parentIndexOther = setUnionTable[parentIndexOther];

                        int finalIndex = parentIndex < parentIndexOther ? parentIndex : parentIndexOther;
                        
                        parentIndex = indexIntoUnionTable;
                        parentIndexOther = otherPixelIndexIntoUnionTable;
                        

                        // Collapse union set.
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
                    }
                }
            }
            inputImageIndex += 3;
            ++indexIntoUnionTable;
        }
    }

    std::map<int, float> colorsToIndicesR;
    std::map<int, float> colorsToIndicesG;
    std::map<int, float> colorsToIndicesB;
    indexIntoUnionTable = height * width - 1;

    int biggestColorIndex = 0;

    for (int y = height-1; y >= 0; y--)
    {
        for (int x = width-1; x >= 0; x--)
        {
            if (setUnionTable[indexIntoUnionTable] == indexIntoUnionTable)
            {
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
    inputImageIndex = 0;
//    unsigned char backgroundR = colorsToIndicesR[biggestColorIndex];
//    unsigned char backgroundG = colorsToIndicesG[biggestColorIndex];
//    unsigned char backgroundB = colorsToIndicesB[biggestColorIndex];
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            
            data2[inputImageIndex] = (unsigned char)colorsToIndicesB[setUnionTable[indexIntoUnionTable]];
            data2[inputImageIndex+1] = (unsigned char)colorsToIndicesG[setUnionTable[indexIntoUnionTable]];
            data2[inputImageIndex+2] = (unsigned char)colorsToIndicesR[setUnionTable[indexIntoUnionTable]];
//            if (colorsToIndicesB[setUnionTable[indexIntoUnionTable]] >= backgroundB)
//	        data2[inputImageIndex] = colorsToIndicesB[setUnionTable[indexIntoUnionTable]] - backgroundB;
//            else
//                data2[inputImageIndex] = 0;
//            
//            if (colorsToIndicesG[setUnionTable[indexIntoUnionTable]] >= backgroundG)
//                data2[inputImageIndex+1] = colorsToIndicesG[setUnionTable[indexIntoUnionTable]] - backgroundG;
//            else
//                data2[inputImageIndex+1] = 0;
//
//            if (colorsToIndicesR[setUnionTable[indexIntoUnionTable]] >= backgroundR)
//                data2[inputImageIndex+2] = colorsToIndicesR[setUnionTable[indexIntoUnionTable]] - backgroundR;
//            else
//                data2[inputImageIndex+2] = 0;

//            int total = data2[inputImageIndex] + data2[inputImageIndex+1] + data2[inputImageIndex+2];
//            float bPercent = data2[inputImageIndex]/(total+1.0);
//            float gPercent = data2[inputImageIndex+1]/(total+1.0);
//            float rPercent = data2[inputImageIndex+2]/(total+1.0);
            
//            data2[inputImageIndex] = 255.0 * (bPercent);
//            data2[inputImageIndex+1] = 255.0 * (gPercent);
//            data2[inputImageIndex+2] = 255.0 * (rPercent);
            ++indexIntoUnionTable;
            inputImageIndex += 3;
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
        cvCircle(m_working, cvPoint(cvRound(p[0]), cvRound(p[1])),(int)p[2],
                 CV_RGB(255,0,0), 3, 8, 0);
    }

    cvReleaseImage(&img);
}

}//vision
}//ram


