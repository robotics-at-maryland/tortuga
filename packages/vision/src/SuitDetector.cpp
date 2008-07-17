/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/SuitDetector.cpp
 */

#include <iostream>
//Library Includes
#include "cv.h"
#include "highgui.h"

//Project Includes
#include "vision/include/main.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/SuitDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/SuitHistoArrays.h"


namespace ram {
namespace vision {
//  Traversal Distances for findPointsOnEdges2 --BestNumbers for handheld camera video
    int SuitDetector::CLUBMIN[] = {120, 120, 200, 200};
    int SuitDetector::CLUBMAX[] = {130, 130, 230, 230};
    int SuitDetector::CLUBSIZE = 4;

    int SuitDetector::SPADEMIN[] = {120, 120, 190, 190};
    int SuitDetector::SPADEMAX[] = {130, 130, 200, 200};
    int SuitDetector::SPADESIZE = 4;

    int SuitDetector::HEARTMIN[] = {100, 100, 150, 150};
    int SuitDetector::HEARTMAX[] = {115, 115, 160, 160};
    int SuitDetector::HEARTSIZE = 4;

    int SuitDetector::DIAMONDMIN[] = {110, 110, 110, 110};
    int SuitDetector::DIAMONDMAX[] = {120, 120, 120, 120};
    int SuitDetector::DIAMONDSIZE = 4;
    
    int SuitDetector::SPLITMIN[] = {110, 110, -2, -2};
    int SuitDetector::SPLITMAX[] = {130, 130, 0, 0};
    int SuitDetector::SPLITSIZE = 4;

//    Traversal Distances for findPointsOnEdges2 --Best Numbers For Tests
//    int SuitDetector::HEARTMIN[] = {145, 145, 90, 90};
//    int SuitDetector::HEARTMAX[] = {155, 155, 110, 110};
//    int SuitDetector::HEARTSIZE = 4;
//
//    int SuitDetector::SPADEMIN[] = {120, 120, 235, 235};
//    int SuitDetector::SPADEMAX[] = {130, 130, 261, 261};
//    int SuitDetector::SPADESIZE = 4;
//
//    int SuitDetector::CLUBMIN[] = {110, 110, 209, 209};
//    int SuitDetector::CLUBMAX[] = {125, 125, 230, 230};
//    int SuitDetector::CLUBSIZE = 4;
//
//    int SuitDetector::DIAMONDMIN[] = {123, 123, 123, 123};
//    int SuitDetector::DIAMONDMAX[] = {133, 133, 133, 133};
//    int SuitDetector::DIAMONDSIZE = 4;

//    Traversal Distances for findPointsOnEdges
//    int SuitDetector::HEARTMIN[] = {160, 150, 45, 95, 55};
//    int SuitDetector::HEARTMAX[] = {177, 165, 60, 112, 75};
//    int SuitDetector::HEARTSIZE = 5;
//
//    int SuitDetector::SPADEMIN[] = {185, 275, 130, 120};
//    int SuitDetector::SPADEMAX[] = {220, 300, 150, 140};
//    int SuitDetector::SPADESIZE = 4;
//
//    int SuitDetector::CLUBMIN[] = {125, 127, 185, 250};
//    int SuitDetector::CLUBMAX[] = {155, 150, 210, 275};
//    int SuitDetector::CLUBSIZE = 4;
//
//    int SuitDetector::DIAMONDMIN[] = {120, 120, 120, 120};
//    int SuitDetector::DIAMONDMAX[] = {131, 131, 131, 134};
//    int SuitDetector::DIAMONDSIZE = 4;

SuitDetector::SuitDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    blobDetector(100)
{
    init(config);
}
    
SuitDetector::SuitDetector(Camera* camera) :
    cam(camera), blobDetector(100)
{
    init(core::ConfigNode::fromString("{}"));
}

void SuitDetector::init(core::ConfigNode)
{
    suit = Suit::UNKNOWN;
	analyzedImage = cvCreateImage(cvSize(640,480), 8,3);
	ratioImage = cvCreateImage(cvSize(640,480),8,3);
	tempHoughImage = cvCreateImage(cvSize(640,480),8,3);
}
	
SuitDetector::~SuitDetector()
{
	cvReleaseImage(&analyzedImage);
	cvReleaseImage(&ratioImage);
	cvReleaseImage(&tempHoughImage);
}

int SuitDetector::findPointsOnEdges2(IplImage* img, int xPositions[], int yPositions[])
{
    int width = img->width;
    int height = img->height;
    int hits = 0;
    unsigned char *data = (unsigned char*) img->imageData;
    int x = 0;
    int y = 0;
    
    x = width-1;
    y = height/2;
    while (x >= 0)
    {
        unsigned char val = data[x * 3 + y * width * 3];
        if (val>0)
        {
            xPositions[0] = x;
            yPositions[0] = y;
            hits++;
            break;
        }
        x--;
    }

    x = width/2;
    y = 0;
    while (y < height)
    {
        unsigned char val = data[x * 3 + y * width * 3];
        if (val>0)
        {
            xPositions[1] = x;
            yPositions[1] = y;
            hits++;
            break;
        }
        y++;
    }
    
    x = 0;
    y = height/2;
    while (x < width)
    {
        unsigned char val = data[x * 3 + y * width * 3];
        if (val>0)
        {
            xPositions[2] = x;
            yPositions[2] = y;
            hits++;
            break;
        }
        x++;
    }
    
    x = width/2;
    y = height-1;
    while (y >= 0)
    {
        unsigned char val = data[x * 3 + y * width * 3];
        if (val>0)
        {
            xPositions[3] = x;
            yPositions[3] = y;
            hits++;
            break;
        }
        y--;
    }
    
    //Must hit 4 sides, or its a failure.
    if (hits != 4)
        return -1;
    else
        return 4;
}


//Returns -1 for failure, or the number of segments along the edges of the image.
//If the number of segments is less than 4, this will return -1  
//If one side of the image has no segments on it, this will also return -1.
//xPositions and yPositions will be filled with the start points of each segment,
//pass in arrays of at least size ten.
//First point is on right side of image.
int SuitDetector::findPointsOnEdges(IplImage* img, int xPositions[], int yPositions[])
{
    int width = img->width;
    int height = img->height;
    unsigned char *data = (unsigned char*) img->imageData;
    
    int totalSegs = 0;
    int x = 0;
    int y = 0;
    int numSegsOnSide = 0;
    int maxSkipCountBeforeBreakingSegs = 2;
    bool onSegment = false;


    //NOTE, DONT DO CORNERS, OTHERWISE WE CAN ADD DUPLICATES OF THE SAME POINT
    //THIS WOULD BE BADNESS.  
    //Right side, counter clockwise.
    x = width-1;
    y = height-2;
    onSegment = false;
    numSegsOnSide = 0;
    maxSkipCountBeforeBreakingSegs = 2;
    for (;y >= 1; y--)
    {
        unsigned char val = data[x * 3 + y * width * 3];
        if (onSegment && val > 0)
        {
            //Continuing along segment, do nothing
            maxSkipCountBeforeBreakingSegs = 2;
        }
        else if (onSegment && val == 0)
        {
            //Segment end, turn off onSegment
            maxSkipCountBeforeBreakingSegs--;
            if (maxSkipCountBeforeBreakingSegs == 0)
            {
                onSegment = false;
                maxSkipCountBeforeBreakingSegs = 2;
            }
        }
        else if (!onSegment && val > 0)
        {
            //Found new segment.
            onSegment = true;
            xPositions[totalSegs + numSegsOnSide] = x;
            yPositions[totalSegs + numSegsOnSide] = y;
            numSegsOnSide++;
        }
        else if (!onSegment && val == 0)
        {
            //Not on segment, and no segment to be seen, do nothing
        }
        
        if (numSegsOnSide >= 3)
        {
            //Failure, too many segs on this side.
            return -1;
        }
    }
    if (numSegsOnSide == 0)
    {
        //Failure, no segs on this side.
        return -1;
    }
    totalSegs += numSegsOnSide;
    
    //NOTE, DONT DO CORNERS, OTHERWISE WE CAN ADD DUPLICATES OF THE SAME POINT
    //THIS WOULD BE BADNESS.  
    //Top side, counter clockwise.
    x = width-2;
    y = 0;
    onSegment = false;
    numSegsOnSide = 0;
    maxSkipCountBeforeBreakingSegs = 2;
    for (;x >= 1; x--)
    {
        unsigned char val = data[x * 3 + y * width * 3];
        if (onSegment && val > 0)
        {
            //Continuing along segment, do nothing
            maxSkipCountBeforeBreakingSegs = 2;
        }
        else if (onSegment && val == 0)
        {
            //Segment end, turn off onSegment
            maxSkipCountBeforeBreakingSegs--;
            if (maxSkipCountBeforeBreakingSegs == 0)
            {
                onSegment = false;
                maxSkipCountBeforeBreakingSegs = 2;
            }
        }
        else if (!onSegment && val > 0)
        {
            //Found new segment.
            onSegment = true;
            xPositions[totalSegs + numSegsOnSide] = x;
            yPositions[totalSegs + numSegsOnSide] = y;
            numSegsOnSide++;
        }
        else if (!onSegment && val == 0)
        {
            //Not on segment, and no segment to be seen, do nothing
        }

        if (numSegsOnSide >= 3)
        {
            //Failure, too many segs on this side.
            return -1;
        }
    }
    if (numSegsOnSide == 0)
    {
        //Failure, no segs on this side.
        return -1;
    }

    totalSegs += numSegsOnSide;


    //NOTE, DONT DO CORNERS, OTHERWISE WE CAN ADD DUPLICATES OF THE SAME POINT
    //THIS WOULD BE BADNESS.  
    //Left side, counter clockwise.
    x = 0;
    y = 1;
    onSegment = false;
    numSegsOnSide = 0;
    maxSkipCountBeforeBreakingSegs = 2;
    for (;y < height - 1; y++)
    {
        unsigned char val = data[x * 3 + y * width * 3];
        if (onSegment && val > 0)
        {
            //Continuing along segment, do nothing
            maxSkipCountBeforeBreakingSegs = 2;
        }
        else if (onSegment && val == 0)
        {
            //Segment end, turn off onSegment
            maxSkipCountBeforeBreakingSegs--;
            if (maxSkipCountBeforeBreakingSegs == 0)
            {
                onSegment = false;
                maxSkipCountBeforeBreakingSegs = 2;
            }
        }
        else if (!onSegment && val > 0)
        {
            //Found new segment.
            onSegment = true;
            xPositions[totalSegs + numSegsOnSide] = x;
            yPositions[totalSegs + numSegsOnSide] = y;
            numSegsOnSide++;
        }
        else if (!onSegment && val == 0)
        {
            //Not on segment, and no segment to be seen, do nothing
        }
        
        if (numSegsOnSide >= 3)
        {
            //Failure, too many segs on this side.
            return -1;
        }
    }
    if (numSegsOnSide == 0)
    {
        //Failure, no segs on this side.
        return -1;
    }

    totalSegs += numSegsOnSide;


    //NOTE, DONT DO CORNERS, OTHERWISE WE CAN ADD DUPLICATES OF THE SAME POINT
    //THIS WOULD BE BADNESS.  
    //Bottom side, counter clockwise.
    x = 1;
    y = height-1;
    onSegment = false;
    numSegsOnSide = 0;
    maxSkipCountBeforeBreakingSegs = 2;
    for (;x < width - 1; x++)
    {
        unsigned char val = data[x * 3 + y * width * 3];
        if (onSegment && val > 0)
        {
            //Continuing along segment, do nothing
            maxSkipCountBeforeBreakingSegs = 2;
        }
        else if (onSegment && val == 0)
        {
            //Segment end, turn off onSegment
            maxSkipCountBeforeBreakingSegs--;
            if (maxSkipCountBeforeBreakingSegs == 0)
            {
                onSegment = false;
                maxSkipCountBeforeBreakingSegs = 2;
            }
        }
        else if (!onSegment && val > 0)
        {
            //Found new segment.
            onSegment = true;
            xPositions[totalSegs + numSegsOnSide] = x;
            yPositions[totalSegs + numSegsOnSide] = y;
            numSegsOnSide++;
        }
        else if (!onSegment && val == 0)
        {
            //Not on segment, and no segment to be seen, do nothing
        }
    
        if (numSegsOnSide >= 3)
        {
            //Failure, too many segs on this side.
            return -1;
        }
    }
    if (numSegsOnSide == 0)
    {
        //Failure, no segs on this side.
        return -1;
    }
    totalSegs += numSegsOnSide;
    
    return totalSegs;
}



/*Return the traversal distance from startX, startY to endX, endY.  
Both of these points should have been returned in the array from find points
on edges.  Returns -1 if endX endY is never reached, so 
call this function ONLY if there is a SINGLE blob in the image

startX startY must be a point along the outer rim of the image!*/
int SuitDetector::edgeRun(int startX, int startY, int endX, int endY, IplImage* img, int dir, int* numBackups)
{
	int width = img->width;
	int height = img->height;
	unsigned char *data=(unsigned char *)img->imageData;
//    int backups = 0;
    int traversalDist = 0;
	int x = startX;
	int y = startY;
	 
	int RIGHT = 3;
	int UP=-width*3;
	int DOWN=width*3;
	int LEFT=-3;
	
    if (dir == 0)
        dir = RIGHT;
    else if (dir == 1)
        dir = UP;
    else if (dir == 2)
        dir = LEFT;
    else if (dir == 3)
        dir = DOWN;
    
	//3 = right 
	//-width*3 = up 
	//-3 = left 
	//width*3 = down
//	int dir = RIGHT;
    
    if (numBackups != NULL)
    {
        *numBackups = 0;
    }
	
	int count = 3*x+3*width*y;
	if (data[count] == 0)
	{
		printf("%s\n", "Invalid start location on edgeRun, must start in a white square");
		return -1;
	}
	
//	while(true)
//	{
//		if (x+1 < width && data[count+3]!=0)
//		{
//			count+=3;
//			data[count+1]=data[count+2]=0;
//			x+=1;
//		}
//		else
//		{
//			//Catch a corner if there is one here, then break out.
//			bool noBottom = false;
//			bool noTop = false;
//			if (y+1 >= height || data[count+3*width] == 0)
//			{
//				noTop = true;
//			}
//			if (y-1 < 0 || data[count-3*width] == 0)
//			{
//				noBottom = true;
//			}
//			if (noBottom && noTop)
//			{
//				backups++;
//			}
//			break;
//		}
//	}
//	 
//	startX = x;
//	startY = y; //Should now have traversed to an edge of the shape.
	 
//    if (startX == width -1 && startY != 0)
//        dir = UP;
//    else if (startY == 0 && startX !=0)
//        dir = LEFT;
//    else if (startX == 0 && startY != height-1)
//        dir = DOWN;
//    else if (startY == height-1 && startX != width-1)
//        dir = RIGHT;
        
	int nextX;
	int nextY;
//	printf("Starting Outline Finding\n");
	while (true)
	{
		int startDir = dir;
		for (int attempt=0; attempt<4; attempt++)
		{
			//First attempt try turn right
			//second attempt try straight
			//third attempt try turn left
			//fourth attempt turn around
			
			if (attempt==0)
			{
				//Turn Right
				if (startDir == UP)
				{
					dir = RIGHT;
				}
				if (startDir == LEFT)
				{
					dir = UP;
				}
				if (startDir == DOWN)
				{
					dir = LEFT;
				}
				if (startDir ==RIGHT)
				{
					dir = DOWN;
				}
			}
			else if (attempt==1)
			{
				//Go straight
				dir = startDir;
			}
			else if (attempt==2)
			{
				if (startDir == UP)
				{
					dir = LEFT;
				}
				if (startDir == LEFT)
				{
					dir = DOWN;
				}
				if (startDir == DOWN)
				{
					dir = RIGHT;
				}
				if (startDir == RIGHT)
				{
					dir = UP;
				}
			}
			else
			{
				//Turn around
				dir = -startDir;
			}
			
			//Now figure out where we'd be heading
			nextX = x;
			nextY = y;
			if (dir == 3)
				nextX++;
			else if (dir == -3)
				nextX--;
			else if (dir > 0)
				nextY++;
			else if (dir < 0)
				nextY--;
			//if we made it, move and break out of the attempt loop
			if (nextX < width && nextX >= 0 && nextY < height && nextY >= 0 && data[count+dir]!=0)
			{
				count+=dir;
				data[count+1]=data[count+2]=0;
				x=nextX;
				y=nextY;
                traversalDist++;

                
//				printf("%d ", attempt);		
				if (attempt == 3)
				{
                    if (numBackups != NULL)
                    {
                        *numBackups = *numBackups + 1;
                    }  
				}
				break;
			}
			else
			{
				//Otherwise, just go on to the next attempt.
			}
		}
        if (x == endX && y == endY)
        {
//            printf("Success!  Reached end\n");
            return traversalDist;
        }
		else if (x == startX && y == startY)
		{
			//Done!
//			printf("Failure, could not reach start from end!\n");
			return -1;
		}
        
	}	
}




//finds the bounds on the suit, assuming it broke into a maximum of two pieces.
//bool SuitDetector::makeSuitHistogram(IplImage* rotatedRedSuit)
//{
//    //make sure to zero out the histogram array, regardless of whether or not we find anything.
//    for (int i = 0; i < HISTOARRSIZE; i++)
//    {
//        histoArr[i] = 0;
//    }
//
//    int minSuitX = 999999;
//    int minSuitY = 999999;
//    int maxSuitX = 0;
//    int maxSuitY = 0;
//    //            int redCX, redCY;
////    cvDilate(rotatedRedSuit,rotatedRedSuit,NULL, 1);
//    OpenCVImage mySuit(rotatedRedSuit,false);
//    blobDetector.setMinimumBlobSize(25);
//    blobDetector.processImage(&mySuit);
//    if (!blobDetector.found())
//    {
//        return false;//no suit found, don't make a histogram
//        //                printf("Oops, we fucked up, no suit found :(\n");
//    }
//    else
//    {
//        //find biggest two blobs (hopefully should be just one, but if spade or club split..)
//        std::vector<ram::vision::BlobDetector::Blob> blobs = blobDetector.getBlobs();
//        ram::vision::BlobDetector::Blob biggest(-1,0,0,0,0,0,0);
//        ram::vision::BlobDetector::Blob secondBiggest(0,0,0,0,0,0,0);
//        ram::vision::BlobDetector::Blob swapper(-1,0,0,0,0,0,0);
//        for (unsigned int blobIndex = 0; blobIndex < blobs.size(); blobIndex++)
//        {
//            if (blobs[blobIndex].getSize() > secondBiggest.getSize())
//            {
//                secondBiggest = blobs[blobIndex];
//                if (secondBiggest.getSize() > biggest.getSize())
//                {
//                    swapper = secondBiggest;
//                    secondBiggest = biggest;
//                    biggest = swapper;
//                }
//            }
//        }
//        minSuitX = biggest.getMinX();
//        minSuitY = biggest.getMinY();
//        maxSuitX = biggest.getMaxX();
//        maxSuitY = biggest.getMaxY();
//        
//        if (blobs.size() > 1)
//        {
//            if (minSuitX > secondBiggest.getMinX())
//                minSuitX = secondBiggest.getMinX();
//            if (minSuitY > secondBiggest.getMinY())
//                minSuitY = secondBiggest.getMinY();
//            if (maxSuitX < secondBiggest.getMaxX())
//                maxSuitX = secondBiggest.getMaxX();
//            if (maxSuitY < secondBiggest.getMaxY())
//                maxSuitY = secondBiggest.getMaxY();
//        }
//    }
//
//    int onlyRedSuitRows = (maxSuitX - minSuitX + 1);// / 4 * 4;
//    int onlyRedSuitCols = (maxSuitY - minSuitY + 1);// / 4 * 4;
//
//    if (onlyRedSuitRows == 0 || onlyRedSuitCols == 0)
//    {
//        return false;
//    }
//    IplImage* onlyRedSuit = cvCreateImage(
//        cvSize(onlyRedSuitRows,
//               onlyRedSuitCols),
//        IPL_DEPTH_8U,
//        3);
//    
//    cvGetRectSubPix(rotatedRedSuit,
//                    onlyRedSuit,
//                    cvPoint2D32f((maxSuitX+minSuitX)/2,
//                                 (maxSuitY+minSuitY)/2));
//    
//    cvResize(onlyRedSuit, scaledRedSuit, CV_INTER_LINEAR);
//    
////    OpenCVImage showTheSuit(scaledRedSuit, false);
////    Image::showImage(&showTheSuit, "The Suit Stands Alone");
//    
//    int scaledRedIndex = 0;
//    unsigned char* scaledRedData=(unsigned char*)scaledRedSuit->imageData;
//    for (int y = 0; y < scaledRedSuit->height; y++)
//    {
//        for (int x = 0; x < scaledRedSuit->width; x++)
//        {
//            if (scaledRedData[scaledRedIndex]!=0)
//            {
//                histoArr[y]++;
//                histoArr[x+scaledRedSuit->height]++;
//            }
//            scaledRedIndex+=3;
//        }
//        if (scaledRedIndex%4 == 0)
//        {}
//        else
//        { 
//            scaledRedIndex += 4 - scaledRedIndex%4;
//        }
//    }
//    
//    
//    cvReleaseImage(&onlyRedSuit);
//    return true;//we made a histogram   
//}


void SuitDetector::processImage(Image* input, Image* output)
{
    IplImage* scaledRedSuit = (IplImage*)(*input);
    //This is all done by bin detector now.
//    IplImage* percentsRotatedRed = cvCreateImage(cvGetSize(rotatedRedSuit),IPL_DEPTH_8U, 3);
//    cvCopyImage(rotatedRedSuit,percentsRotatedRed);
//    to_ratios(percentsRotatedRed);
//    suitMask(percentsRotatedRed, rotatedRedSuit);
//    OpenCVImage mySuit(rotatedRedSuit,false);
//    Image::showImage(&mySuit);
//    if (cropImage(rotatedRedSuit)) //This places a cropped suit into scaledRedSuit.
//    {
//        doEdgeRunning(scaledRedSuit);
//    }
//    else
//    {
//        printf("Failure to crop image properly, too small a suit?\n");
//    }
//    cvReleaseImage(&percentsRotatedRed);

    doEdgeRunning(scaledRedSuit);
}


void SuitDetector::doEdgeRunning(IplImage* image)
{
//    OpenCVImage wrapScaled(scaledRedSuit, false);
//    Image::showImage(&wrapScaled, "The Red Suit, Cropped And Masked");
//    printf("Starting edge running\n");
    int xPos[20];
    int yPos[20];
    int traverseDists[20];
    int numBackups = 0;
    int numSegs = findPointsOnEdges2(image,xPos,yPos);
    
    if (numSegs == -1)
    {
//        printf("Edge running failure!\n");
//        vision::OpenCVImage showMe(image,false);
//        vision::Image::showImage(&showMe);
//        printf("Failure on findPointsOnEdges\n");
        suit = Suit::UNKNOWN;
    }
    else 
    {
        int j = numSegs-1;
        //printf("Traversal Distances: \n");
        for (int i = 0; i < numSegs; j=i++)
        {
            int startX = xPos[j];
            int startY = yPos[j];
            int endX = xPos[i];
            int endY = yPos[i];

            traverseDists[i] = edgeRun(startX,startY,endX,endY, image, i, &numBackups);
        //    printf("(%d,%d) to (%d,%d) : %d   Backups: %d \n", startX, startY, endX, endY, traverseDists[i], numBackups);
        }
        //printf("End of Traversal Dists \n");
    }
    
    suit = Suit::UNKNOWN;
    bool heartLike = false;
    bool spadeLike = false;
    bool clubLike = false;
    bool diamondLike = false;
    bool splitSuit = false;
    
    if (numSegs == SuitDetector::SPLITSIZE)
    {
        splitSuit = cyclicCompare(traverseDists, SuitDetector::SPLITMIN, SuitDetector::SPLITMAX, SuitDetector::SPLITSIZE);
    }
    
    if (numSegs == SuitDetector::HEARTSIZE)
    {
        heartLike = cyclicCompare(traverseDists, SuitDetector::HEARTMIN, SuitDetector::HEARTMAX, SuitDetector::HEARTSIZE);
//        if (heartLike)
//            printf("Heart!");
    }
    if (numSegs == SuitDetector::SPADESIZE)
    {
        spadeLike = cyclicCompare(traverseDists, SuitDetector::SPADEMIN, SuitDetector::SPADEMAX, SuitDetector::SPADESIZE);
//        if (spadeLike)
//            printf("Spade!");
    }
    if (numSegs == SuitDetector::CLUBSIZE)
    {
        clubLike = cyclicCompare(traverseDists, SuitDetector::CLUBMIN, SuitDetector::CLUBMAX, SuitDetector::CLUBSIZE);
//        if (clubLike)
//            printf("Club!");
    }
    if (numSegs == SuitDetector::DIAMONDSIZE)
    {    
        diamondLike = cyclicCompare(traverseDists, SuitDetector::DIAMONDMIN, SuitDetector::DIAMONDMAX, SuitDetector::DIAMONDSIZE);
//        if (diamondLike)
//            printf("Diamond!");
    }
    
    if (heartLike && !spadeLike && !clubLike && !diamondLike)
    {
        suit = Suit::HEART;
    }
    if (spadeLike && !heartLike && !clubLike && !diamondLike)
    {
        suit = Suit::SPADE;
    }
    if (clubLike && !heartLike && !spadeLike && !diamondLike)
    {
        suit = Suit::CLUB;
    }
    if (diamondLike && !heartLike && !spadeLike && !clubLike)
    {
        suit = Suit::DIAMOND;
    }
    
    if (splitSuit || (clubLike && spadeLike))
    {
        int distBot = 127-yPos[3];
        int distTop = yPos[1];
        int distRight = 127 - xPos[0];
        int distLeft = xPos[2];
        
        //printf("Dists: %d %d %d %d\n", distBot, distRight, distTop, distLeft);
        
        if (distBot >= 8 && distTop >= 8 && distLeft <= 2 && distRight <= 2)
        {
            suit = Suit::SPADE;
        }
        else if (distBot >= 2 && distTop >= 2 && distLeft <= 8 && distRight <= 8)
        {
            suit = Suit::SPADE;
        }
        else if (distBot <2 && distTop < 2 && distLeft < 2 && distRight < 2)
        {
            suit = Suit::CLUB;
        }
//        if (suit == Suit::SPADE)
//            printf("SPADE BY SPLIT!\n");
//        else if (suit == Suit::CLUB)
//            printf("CLUB BY SPLIT!\n");
    }
    
//    printf("\n");
//    switch(suit)
//    {
//        case Suit::HEART: break;//printf("Heart!\n");break;
//        case Suit::DIAMOND: break;//printf("Diamond!\n");break;
//        case Suit::SPADE: break;//printf("Spade!\n");break;
//        case Suit::CLUB: break;//printf("Club!\n");break;
//            default: printf("Unknown :( ??\n");
//    }
//    if (suit == Suit::SPADE)
//        printf("What the hell...\n");
}

bool SuitDetector::cyclicCompare(int traverseDists[], int min[], int max[], int size)
{
    
    int shift = 0;
    bool failure = false;
    
    for (shift = 0; shift < size; shift++)
    {
//        for (int i = 0; i < size; i++)
//        {
//            printf("%d <= %d <= %d\n", min[(shift + i) % size], traverseDists[i], max[(shift + i) % size]);
//        }

        for (int check = 0; check < size; check++)
        {
            if (traverseDists[check] < min[(shift + check) % size] ||
                traverseDists[check] > max[(shift + check) % size]   )
                {
                    failure = true;
                    break;
                }
        }
        if (failure)
        {
            failure = false;
        }
        else
        {
            return true; //Match
        }
    }
    return false; //No match
}

//Input is expected to be a black rectangle that touches the four edges of the image (or fills it completely if the bin was properly rotated)
//void SuitDetector::processImage(Image* input, Image* output)
//{
//    IplImage* redSuit = (IplImage*)(*input);
//    IplImage* redSuitGrayScale = cvCreateImage(cvGetSize(redSuit),IPL_DEPTH_8U,1);
//
//    cvCvtColor(redSuit,redSuitGrayScale,CV_BGR2GRAY);
//    IplImage* cannied = cvCreateImage(cvGetSize(redSuitGrayScale), 8, 1 );
//
//    cvCanny( redSuitGrayScale, cannied, 50, 200, 3 );
//    CvMemStorage* storage = cvCreateMemStorage(0);
//    CvSeq* lines = 0;
//        
//    lines = cvHoughLines2( cannied, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 10, 70, 30 );
//        
//    float longestLineLength = -1;
//    float slope = 0;
//    for(int i = 0; i < lines->total; i++ )
//    {
//        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
//        float lineX = line[1].x - line[0].x;
//        float lineY = line[1].y - line[0].y;
//        if (longestLineLength < (lineX * lineX + lineY * lineY))
//        {
//            slope = atan2(lineY,lineX);
//            longestLineLength = lineX * lineX + lineY * lineY;
//        }
//    }
//    
//    IplImage* rotatedRedSuit = cvCreateImage(cvGetSize(redSuit), IPL_DEPTH_8U, 3);
//    
//    float m[6];
//    CvMat M = cvMat( 2, 3, CV_32F, m );
//
//    double factor = -slope;
//    m[0] = (float)(cos(factor));
//    m[1] = (float)(sin(factor));
//    m[2] = redSuitGrayScale->width * 0.5f;
//    m[3] = -m[1];
//    m[4] = m[0];
//    m[5] = redSuitGrayScale->height * 0.5f;
//    
//    cvGetQuadrangleSubPix(redSuit, rotatedRedSuit,&M);
//    
//    IplImage* percentsRotatedRed = cvCreateImage(cvGetSize(rotatedRedSuit),IPL_DEPTH_8U, 3);
//    
//    cvCopyImage(rotatedRedSuit,percentsRotatedRed);
//    to_ratios(percentsRotatedRed);
//    //            OpenCVImage mySuit(rotatedRedSuit,false);
//    //            Image::showImage(&mySuit);
//    
//    suitMask(percentsRotatedRed, rotatedRedSuit);
//    
//    //rotatedRedSuit is now properly rotated and masked for red, pass it on to the histogrammer
////    OpenCVImage showThis(rotatedRedSuit,false);
////    Image::showImage(&showThis);
//    
//    if (makeSuitHistogram(rotatedRedSuit))
//    {
//        //Compare float array
//        int best = 8000;//Unless one of the scores is below this... I don't think it should be counted
//                        //threshold must change if histoArr changes size.
//        suit = Suit::UNKNOWN;
//        int diff;
//        
//        //Hearts
//        //std::cout<<"Hearts:\n";
//
//        diff = suitDifference(histoArr, heartCountsR0, HISTOARRSIZE);        
//        //std::cout<<"R0: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::HEART;
//        }
//
//        diff = suitDifference(histoArr, heartCountsR90, HISTOARRSIZE);
//        //std::cout<<"R90: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::HEARTR90;
//        }
//
//        diff = suitDifference(histoArr, heartCountsR180, HISTOARRSIZE);
//        //std::cout<<"R180: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::HEARTR180;
//        }
//
//        diff = suitDifference(histoArr, heartCountsR270, HISTOARRSIZE);
//        //std::cout<<"R270: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::HEARTR270;
//        }
//
//        
//        //Spades
//        //std::cout<<"Spades:\n";
//        diff = suitDifference(histoArr, spadeCountsR0, HISTOARRSIZE);
//        //std::cout<<"R0: "<<diff<<"\n";
//
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::SPADE;
//        }
//
//        diff = suitDifference(histoArr, spadeCountsR90, HISTOARRSIZE);
//        //std::cout<<"R90: "<<diff<<"\n";
//
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::SPADER90;
//        }
//
//        diff = suitDifference(histoArr, spadeCountsR180, HISTOARRSIZE);
//        //std::cout<<"R180: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::SPADER180;
//        }
//
//        diff = suitDifference(histoArr, spadeCountsR270, HISTOARRSIZE);
//        //std::cout<<"R270: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::SPADER270;
//        }
//        
//        
//        //diamond
//        //std::cout<<"Diamonds:\n";
//        diff = suitDifference(histoArr, diamondCountsR0, HISTOARRSIZE);
//        //std::cout<<"R0: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::DIAMOND;
//        }
//
//        diff = suitDifference(histoArr, diamondCountsR90, HISTOARRSIZE);
//        //std::cout<<"R90: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::DIAMONDR90;
//        }
//
//        diff = suitDifference(histoArr, diamondCountsR180, HISTOARRSIZE);
//        //std::cout<<"R180: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::DIAMONDR180;
//        }
//
//        diff = suitDifference(histoArr, diamondCountsR270, HISTOARRSIZE);
//        //std::cout<<"R270: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::DIAMONDR270;
//        }
//
//        
//        //Club
//        //std::cout<<"Clubs:\n";
//        diff = suitDifference(histoArr, clubCountsR0, HISTOARRSIZE);
//        //std::cout<<"R0: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::CLUB;
//        }
//
//        diff = suitDifference(histoArr, clubCountsR90, HISTOARRSIZE);
//        //std::cout<<"R90: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::CLUBR90;
//        }
//
//        diff = suitDifference(histoArr, clubCountsR180, HISTOARRSIZE);
//        //std::cout<<"R180: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::CLUBR180;
//        }
//
//        diff = suitDifference(histoArr, clubCountsR270, HISTOARRSIZE);
//        //std::cout<<"R270: "<<diff<<"\n";
//        if (diff < best)
//        {
//            best = diff;
//            suit = Suit::CLUBR270;
//        }
//    }
//    else
//    {
//        suit = Suit::NONEFOUND;
//    }
//    
//    cvReleaseImage(&redSuitGrayScale);
//    cvReleaseImage(&cannied);
//    cvReleaseMemStorage(&storage);
//    cvReleaseImage(&rotatedRedSuit);
//    cvReleaseImage(&percentsRotatedRed);
//}

//void SuitDetector::processImage(Image* input, Image* output)
//{
//	/*First argument to white_detect is a ratios frame, then a regular one*/
//	IplImage* image = (IplImage*)(*input);
//	cvCopyImage(image,ratioImage);//Now both are rotated 90 degrees
//	IplImage* gray = cvCreateImage(cvGetSize(image), 8, 1);
//    CvMemStorage* storage = cvCreateMemStorage(0);
//    
//	to_ratios(ratioImage);
//	redMask(ratioImage, image, 50, 100);
//	
//	unsigned int r = (unsigned int)(rand())%image->width;
//	unsigned int r2 = (unsigned int)(rand())%image->height;
//	int corners = edgeRun(r,r2,image);
//	
//	if (corners == 0)
//		printf("Club\n");
//	else if (corners == 1)
//		printf("Heart\n");
//	else if (corners == 2)
//		printf("Spade\n");
//	else if (corners == 3)
//		printf("Diamond\n");
//	
//	cvCvtColor(image, gray, CV_BGR2GRAY);
//    cvSmooth(gray, gray, CV_GAUSSIAN, 9, 9); 
//	
//    CvSeq* circles = cvHoughCircles(gray, storage, 
//        CV_HOUGH_GRADIENT, 2, gray->height/4, 200, 40);
//    int i;
//	
//    for (i = 0; i < circles->total; i++) 
//    {
//         float* p = (float*)cvGetSeqElem( circles, i );
//         cvCircle( image, cvPoint(cvRound(p[0]),cvRound(p[1])), 
//             3, CV_RGB(0,255,0), -1, 8, 0 );
//         cvCircle( image, cvPoint(cvRound(p[0]),cvRound(p[1])), 
//             cvRound(p[2]), CV_RGB(255,0,0), 3, 8, 0 );
//    }
//
//	if (output)
//	{
//		OpenCVImage temp(image, false);
//		output->copyFrom(&temp);
//	}
//	
//	cvReleaseMemStorage(&storage);
//	cvReleaseImage(&gray);	
//}

Suit::SuitType SuitDetector::getSuit()
{
	return suit;
}

IplImage* SuitDetector::getAnalyzedImage()
{
	return analyzedImage;
}

} // namespace vision
} // namespace ram
