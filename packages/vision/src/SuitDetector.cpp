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
    scaledRedSuit = cvCreateImage(cvSize(64,64),IPL_DEPTH_8U, 3);
}
	
SuitDetector::~SuitDetector()
{
	cvReleaseImage(&analyzedImage);
	cvReleaseImage(&ratioImage);
	cvReleaseImage(&tempHoughImage);
    cvReleaseImage(&scaledRedSuit);
}

/* New plans:  Run the four edges of the image traveling counter clockwise, mark the
sections of pixels touching the edges.  These are the places where the suit hits the 
walls of the image.  Mark the first of each of these sections.  

Now, run the edge of the suit, counting traversal distances with edgeRun.

This will create both a count of pixels along the edges, sections along the edges, 
and traversal distances between sections along the edges.*/
int SuitDetector::edgeRun(int startX, int startY, IplImage* img)
{
	int width = img->width;
	int height = img->height;
	unsigned char *data=(unsigned char *)img->imageData;
	int backups = 0;

	int x = startX;
	int y = startY;
	 
	int RIGHT = 3;
	int UP=-width*3;
	int DOWN=width*3;
	int LEFT=-3;
	
	//3 = right 
	//-width*3 = up 
	//-3 = left 
	//width*3 = down
	int dir = RIGHT;
	
	int count = 3*x+3*width*y;
	if (data[count] == 0)
	{
		printf("%s\n", "Invalid start location on edgeRun, must start in a white square");
		return -1;
	}
	
	while(true)
	{
		if (x+1 < width && data[count+3]!=0)
		{
			count+=3;
			data[count+1]=data[count+2]=0;
			x+=1;
		}
		else
		{
			//Catch a corner if there is one here, then break out.
			bool noBottom = false;
			bool noTop = false;
			if (y+1 >= height || data[count+3*width] == 0)
			{
				noTop = true;
			}
			if (y-1 < 0 || data[count-3*width] == 0)
			{
				noBottom = true;
			}
			if (noBottom && noTop)
			{
				backups++;
			}
			break;
		}
	}
	 
	startX = x;
	startY = y; //Should now have traversed to an edge of the shape.
	 
	dir = UP;
	int nextX;
	int nextY;
	printf("Starting Outline Finding\n");
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
			
//				printf("%d ", attempt);				
				if (attempt == 3)
				{
//					data[count]=255;
//					data[count+1]=0;
//					data[count+2]=255;
					backups++;
				}
				break;
			}
			else
			{
				//Otherwise, just go on to the next attempt.
			}
		}
		if (x == startX && y == startY)
		{
			//Done!
			printf("backups: %d\n",backups);
			return backups;
		}
	}	
}

//finds the bounds on the suit, assuming it broke into a maximum of two pieces.
bool SuitDetector::makeSuitHistogram(IplImage* rotatedRedSuit)
{
    //make sure to zero out the histogram array, regardless of whether or not we find anything.
    for (int i = 0; i < HISTOARRSIZE; i++)
    {
        histoArr[i] = 0;
    }

    int minSuitX = 999999;
    int minSuitY = 999999;
    int maxSuitX = 0;
    int maxSuitY = 0;
    //            int redCX, redCY;
//    cvDilate(rotatedRedSuit,rotatedRedSuit,NULL, 1);
    OpenCVImage mySuit(rotatedRedSuit,false);
    blobDetector.setMinimumBlobSize(25);
    blobDetector.processImage(&mySuit);
    if (!blobDetector.found())
    {
        return false;//no suit found, don't make a histogram
        //                printf("Oops, we fucked up, no suit found :(\n");
    }
    else
    {
        //find biggest two blobs (hopefully should be just one, but if spade or club split..)
        std::vector<ram::vision::BlobDetector::Blob> blobs = blobDetector.getBlobs();
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
        minSuitX = biggest.getMinX();
        minSuitY = biggest.getMinY();
        maxSuitX = biggest.getMaxX();
        maxSuitY = biggest.getMaxY();
        
        if (blobs.size() > 1)
        {
            if (minSuitX > secondBiggest.getMinX())
                minSuitX = secondBiggest.getMinX();
            if (minSuitY > secondBiggest.getMinY())
                minSuitY = secondBiggest.getMinY();
            if (maxSuitX < secondBiggest.getMaxX())
                maxSuitX = secondBiggest.getMaxX();
            if (maxSuitY < secondBiggest.getMaxY())
                maxSuitY = secondBiggest.getMaxY();
        }
    }

    int onlyRedSuitRows = (maxSuitX - minSuitX + 1);// / 4 * 4;
    int onlyRedSuitCols = (maxSuitY - minSuitY + 1);// / 4 * 4;

    if (onlyRedSuitRows == 0 || onlyRedSuitCols == 0)
    {
        return false;
    }
    IplImage* onlyRedSuit = cvCreateImage(
        cvSize(onlyRedSuitRows,
               onlyRedSuitCols),
        IPL_DEPTH_8U,
        3);
    
    cvGetRectSubPix(rotatedRedSuit,
                    onlyRedSuit,
                    cvPoint2D32f((maxSuitX+minSuitX)/2,
                                 (maxSuitY+minSuitY)/2));
    
    cvResize(onlyRedSuit, scaledRedSuit, CV_INTER_LINEAR);
    
//    OpenCVImage showTheSuit(scaledRedSuit, false);
//    Image::showImage(&showTheSuit, "The Suit Stands Alone");
    
    int scaledRedIndex = 0;
    unsigned char* scaledRedData=(unsigned char*)scaledRedSuit->imageData;
    for (int y = 0; y < scaledRedSuit->height; y++)
    {
        for (int x = 0; x < scaledRedSuit->width; x++)
        {
            if (scaledRedData[scaledRedIndex]!=0)
            {
                histoArr[y]++;
                histoArr[x+scaledRedSuit->height]++;
            }
            scaledRedIndex+=3;
        }
        if (scaledRedIndex%4 == 0)
        {}
        else
        { 
            scaledRedIndex += 4 - scaledRedIndex%4;
        }
    }
    
    
    cvReleaseImage(&onlyRedSuit);
    return true;//we made a histogram   
}

//Input is expected to be a black rectangle that touches the four edges of the image (or fills it completely if the bin was properly rotated)
void SuitDetector::processImage(Image* input, Image* output)
{
    IplImage* redSuit = (IplImage*)(*input);
    IplImage* redSuitGrayScale = cvCreateImage(cvGetSize(redSuit),IPL_DEPTH_8U,1);

    cvCvtColor(redSuit,redSuitGrayScale,CV_BGR2GRAY);
    IplImage* cannied = cvCreateImage(cvGetSize(redSuitGrayScale), 8, 1 );

    cvCanny( redSuitGrayScale, cannied, 50, 200, 3 );
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
        
    lines = cvHoughLines2( cannied, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 10, 70, 30 );
        
    float longestLineLength = -1;
    float slope = 0;
    for(int i = 0; i < lines->total; i++ )
    {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
        float lineX = line[1].x - line[0].x;
        float lineY = line[1].y - line[0].y;
        if (longestLineLength < (lineX * lineX + lineY * lineY))
        {
            slope = atan2(lineY,lineX);
            longestLineLength = lineX * lineX + lineY * lineY;
        }
    }
    
    IplImage* rotatedRedSuit = cvCreateImage(cvGetSize(redSuit), IPL_DEPTH_8U, 3);
    
    float m[6];
    CvMat M = cvMat( 2, 3, CV_32F, m );

    double factor = -slope;
    m[0] = (float)(cos(factor));
    m[1] = (float)(sin(factor));
    m[2] = redSuitGrayScale->width * 0.5f;
    m[3] = -m[1];
    m[4] = m[0];
    m[5] = redSuitGrayScale->height * 0.5f;
    
    cvGetQuadrangleSubPix(redSuit, rotatedRedSuit,&M);
    
    IplImage* percentsRotatedRed = cvCreateImage(cvGetSize(rotatedRedSuit),IPL_DEPTH_8U, 3);
    
    cvCopyImage(rotatedRedSuit,percentsRotatedRed);
    to_ratios(percentsRotatedRed);
    //            OpenCVImage mySuit(rotatedRedSuit,false);
    //            Image::showImage(&mySuit);
    
    suitMask(percentsRotatedRed, rotatedRedSuit);
    
    //rotatedRedSuit is now properly rotated and masked for red, pass it on to the histogrammer
//    OpenCVImage showThis(rotatedRedSuit,false);
//    Image::showImage(&showThis);
    
    if (makeSuitHistogram(rotatedRedSuit))
    {
        //Compare float array
        int best = 8000;//Unless one of the scores is below this... I don't think it should be counted
                        //threshold must change if histoArr changes size.
        suit = Suit::UNKNOWN;
        int diff;
        
        //Hearts
        //std::cout<<"Hearts:\n";

        diff = suitDifference(histoArr, heartCountsR0, HISTOARRSIZE);        
        //std::cout<<"R0: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::HEART;
        }

        diff = suitDifference(histoArr, heartCountsR90, HISTOARRSIZE);
        //std::cout<<"R90: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::HEARTR90;
        }

        diff = suitDifference(histoArr, heartCountsR180, HISTOARRSIZE);
        //std::cout<<"R180: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::HEARTR180;
        }

        diff = suitDifference(histoArr, heartCountsR270, HISTOARRSIZE);
        //std::cout<<"R270: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::HEARTR270;
        }

        
        //Spades
        //std::cout<<"Spades:\n";
        diff = suitDifference(histoArr, spadeCountsR0, HISTOARRSIZE);
        //std::cout<<"R0: "<<diff<<"\n";

        if (diff < best)
        {
            best = diff;
            suit = Suit::SPADE;
        }

        diff = suitDifference(histoArr, spadeCountsR90, HISTOARRSIZE);
        //std::cout<<"R90: "<<diff<<"\n";

        if (diff < best)
        {
            best = diff;
            suit = Suit::SPADER90;
        }

        diff = suitDifference(histoArr, spadeCountsR180, HISTOARRSIZE);
        //std::cout<<"R180: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::SPADER180;
        }

        diff = suitDifference(histoArr, spadeCountsR270, HISTOARRSIZE);
        //std::cout<<"R270: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::SPADER270;
        }
        
        
        //diamond
        //std::cout<<"Diamonds:\n";
        diff = suitDifference(histoArr, diamondCountsR0, HISTOARRSIZE);
        //std::cout<<"R0: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::DIAMOND;
        }

        diff = suitDifference(histoArr, diamondCountsR90, HISTOARRSIZE);
        //std::cout<<"R90: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::DIAMONDR90;
        }

        diff = suitDifference(histoArr, diamondCountsR180, HISTOARRSIZE);
        //std::cout<<"R180: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::DIAMONDR180;
        }

        diff = suitDifference(histoArr, diamondCountsR270, HISTOARRSIZE);
        //std::cout<<"R270: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::DIAMONDR270;
        }

        
        //Club
        //std::cout<<"Clubs:\n";
        diff = suitDifference(histoArr, clubCountsR0, HISTOARRSIZE);
        //std::cout<<"R0: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::CLUB;
        }

        diff = suitDifference(histoArr, clubCountsR90, HISTOARRSIZE);
        //std::cout<<"R90: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::CLUBR90;
        }

        diff = suitDifference(histoArr, clubCountsR180, HISTOARRSIZE);
        //std::cout<<"R180: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::CLUBR180;
        }

        diff = suitDifference(histoArr, clubCountsR270, HISTOARRSIZE);
        //std::cout<<"R270: "<<diff<<"\n";
        if (diff < best)
        {
            best = diff;
            suit = Suit::CLUBR270;
        }
    }
    else
    {
        suit = Suit::NONEFOUND;
    }
    
    cvReleaseImage(&redSuitGrayScale);
    cvReleaseImage(&cannied);
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&rotatedRedSuit);
    cvReleaseImage(&percentsRotatedRed);
}

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
