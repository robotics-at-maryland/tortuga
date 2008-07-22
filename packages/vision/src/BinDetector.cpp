/*
  (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/BinDetector.cpp
 */

// STD Includes
#include <iostream>
#include <cmath>
#include <algorithm>
#include <sstream>
//#include <cassert>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include <log4cpp/Category.hh>
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/BinDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"

#include "math/include/Vector2.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Vision"));



namespace ram {
namespace vision {

static bool binToCenterComparer(BinDetector::Bin b1, BinDetector::Bin b2)
{
    return b1.distanceTo(0,0) < b2.distanceTo(0,0);
}   
    
BinDetector::Bin::Bin() :
    Blob(0, 0, 0, 0, 0, 0, 0),
    m_normX(0),
    m_normY(0),
    m_angle(math::Degree(0)),
    m_id(0),
    m_suit(Suit::NONEFOUND)
{
}
    
BinDetector::Bin::Bin(BlobDetector::Blob blob, double x, double y,
                      math::Degree angle, int id, Suit::SuitType suit) :
    Blob(blob),
    m_normX(x),
    m_normY(y),
    m_angle(angle),
    m_id(id),
    m_suit(suit)
{
}

double BinDetector::Bin::distanceTo(Bin& otherBin)
{
    return distanceTo(otherBin.getX(), otherBin.getY());
}

double BinDetector::Bin::distanceTo(double x, double y)
{
    math::Vector2 mCenter(getX(), getY());
    math::Vector2 otherCenter(x, y);
    return (mCenter-otherCenter).length();
}
    
void BinDetector::Bin::draw(Image* image)
{
    IplImage* out = image->asIplImage();
    // Draw green rectangle around the blob
    CvPoint tl,tr,bl,br;
    tl.x = bl.x = getMinX();
    tr.x = br.x = getMaxX();
    tl.y = tr.y = getMinY();
    bl.y = br.y = getMaxY();
    cvLine(out, tl, tr, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tl, bl, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tr, br, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, bl, br, CV_RGB(0,255,0), 3, CV_AA, 0);

    // Now draw my id
    std::stringstream ss;
    ss << getId();
    Image::writeText(image, ss.str(), tl.x, tl.y);

    std::stringstream ss2;
    ss2 << getAngle().valueDegrees();
    Image::writeText(image, ss2.str(), br.x-30, br.y-15);

    // Now do the suit
    if (Suit::NONEFOUND == m_suit)
        Image::writeText(image, "None", bl.x, bl.y - 15);
    else if (Suit::UNKNOWN == m_suit)
        Image::writeText(image, "Unknown", bl.x, bl.y - 15);
    else if (Suit::HEART == m_suit)
        Image::writeText(image, "Heart", bl.x, bl.y - 15);
    else if (Suit::DIAMOND == m_suit)
        Image::writeText(image, "Diamond", bl.x, bl.y - 15);
    else if (Suit::SPADE == m_suit)
        Image::writeText(image, "Spade", bl.x, bl.y - 15);
    else if (Suit::CLUB == m_suit)
        Image::writeText(image, "Club", bl.x, bl.y - 15);
}
    
    
BinDetector::BinDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    suitDetector(config,eventHub),
    blobDetector(config,eventHub),
    m_centered(false)
{
    init(config);
    setSuitDetectionOn(true);
}
    
void BinDetector::init(core::ConfigNode config)
{
    rotated = cvCreateImage(cvSize(640,480), 8, 3);
    // Its only 480 by 640 if the cameras on sideways
    binFrame =cvCreateImage(cvGetSize(rotated),8,3);
    bufferFrame = cvCreateImage(cvGetSize(rotated),8,3);
    memset(bufferFrame->imageData, 0,
           bufferFrame->width * bufferFrame->height * 3);
    whiteMaskedFrame = cvCreateImage(cvGetSize(rotated),8,3);
    blackMaskedFrame = cvCreateImage(cvGetSize(rotated),8,3);
    m_found=0;
/*    foundHeart = false;
    foundSpade = false;
    foundDiamond = false;
    foundClub = false;
    foundEmpty = false;*/
    m_runSuitDetector = false;
    m_centeredLimit = config["centeredLimit"].asDouble(0.1);
    m_sameBinThreshold = config["sameBinThreshold"].asDouble(0.2);
    m_maxAspectRatio = config["maxAspectRatio"].asDouble(3);
    
    m_blackMaskMinimumPercent = config["blackMaskMinimumPercent"].asInt(10);
    m_blackMaskMaxTotalIntensity = config["blackMaskMaxTotalIntensity"].asInt(350);
    
    m_whiteMaskMinimumPercent = config["whiteMaskMinimumPercent"].asInt(30);
    m_whiteMaskMinimumIntensity = config["whiteMaskMinimumIntensity"].asInt(190);
    m_binID = 0;
    for (int i = 0; i < 4; i++)
    {
        binImages[i] = cvCreateImage(cvSize(128,128),8,3);
    }
}
    
BinDetector::~BinDetector()
{
    cvReleaseImage(&binFrame);
    cvReleaseImage(&rotated);
    cvReleaseImage(&bufferFrame);
    for (int i = 0; i < 4; i++)
    {
        cvReleaseImage((&binImages[i]));
    }
}

void BinDetector::processImage(Image* input, Image* out)
{
    /*First argument to white_detect is a ratios frame, then a regular one*/
    IplImage* image =(IplImage*)(*input);
    IplImage* output = NULL;
    if (out != NULL)
    {
        output = (IplImage*)(*out);
    }
//std::cout<<"startup"<<std::endl;
    
    //This is only right if the camera is on sideways... again.
    //rotate90Deg(image,rotated);

    //Else just copy
    cvCopyImage(image,rotated);
    image=rotated;//rotated is poorly named when camera is on correctly... oh well.
        
    // Set image to a newly copied space so we don't write over opencv's 
    // private memory space... since opencv has a bad habit of making
    // assumptions about what I want to do. :)
    cvCopyImage(image,binFrame);
    
    // Fill in output image.
    if (out)
    {
        OpenCVImage temp(binFrame, false);
        out->copyFrom(&temp);
    }
    
    // Make image into each pixel being the percentage of its total value
    to_ratios(image);
    
    // Image is now in percents, binFrame is now the base image.
    /*int totalWhiteCount = */white_mask(image,binFrame, whiteMaskedFrame, m_whiteMaskMinimumPercent, m_whiteMaskMinimumIntensity);
    /*int totalBlackCount = */black_mask(image,binFrame, blackMaskedFrame, m_blackMaskMinimumPercent, m_blackMaskMaxTotalIntensity);
    
    if (output)
    {
    for (int count = 0; count < binFrame->width* binFrame->height * 3; count++)
    {
        if (whiteMaskedFrame->imageData[count] != 0)
        {
//            binFrame->imageData[count] = 255;
              output->imageData[count] = 255;
        }
    }
    }
    if (output)
    {
    for (int count = 0; count < binFrame->width* binFrame->height * 3; count++)
    {
        if (blackMaskedFrame->imageData[count] != 0)
        {
//            binFrame->imageData[count] = 0;
            output->imageData[count] = 0;
        }
    }
    }
//    if (output)
//    {
//    suitMask(image,output);
//    }

    // Find all the white blobs
    blobDetector.setMinimumBlobSize(1000);
    OpenCVImage whiteMaskWrapper(whiteMaskedFrame,false);
    blobDetector.processImage(&whiteMaskWrapper);
    BlobDetector::BlobList whiteBlobs = blobDetector.getBlobs();
    
    // Find all the black blobs
    blobDetector.setMinimumBlobSize(500);
    OpenCVImage blackMaskWrapper(blackMaskedFrame,false);
    blobDetector.processImage(&blackMaskWrapper);
    BlobDetector::BlobList blackBlobs = blobDetector.getBlobs();

    // List of found blobs which are bins
    BlobDetector::BlobList binBlobs;

//    BOOST_FOREACH(BlobDetector::Blob whiteBlob, whiteBlobs)
//    {
//        if (output)
//            whiteBlob.draw(out);
//    }
//    
//    BOOST_FOREACH(BlobDetector::Blob blackBlob, blackBlobs)
//    {
//        if (output)
//            blackBlob.draw(out);
//    }

    // NOTE: all blobs sorted largest to smallest
    BOOST_FOREACH(BlobDetector::Blob blackBlob, blackBlobs)
    {
        BOOST_FOREACH(BlobDetector::Blob whiteBlob, whiteBlobs)
        {
            // Sadly, this totally won't work at the edges of the screen...
            // crap damn.
            if (whiteBlob.containsInclusive(blackBlob,2) &&
                blackBlob.getAspectRatio() < m_maxAspectRatio)
            {
                // blackBlobs[blackBlobIndex] is the black rectangle of a bin
                binBlobs.push_back(blackBlob);
                
                // Don't add it once for each white blob containing it,
                // thatd be dumb.
                break;
            }
            else
            {
                //Not a bin.
            }
        }
    }
    
    if (out)
    {
        std::stringstream ss;
        ss << binBlobs.size();
        Image::writeText(out, ss.str(),0,200);
    }
    
//    std::cout<<"Made set of bins"<<std::endl;
//    std::cout<<"Num bins found: " << binBlobs.size() <<std::endl;
    
/*    bool seeHeart=false;
    bool seeDiamond=false;
    bool seeClub=false;
    bool seeSpade=false;
    bool seeEmpty=false;*/

    // Publish lost event
    if (m_found && (binBlobs.size() == 0))
    {
        m_found = false;
        m_centered = false;
        publish(EventType::BIN_LOST, core::EventPtr(new core::Event()));
    }
    
    if (binBlobs.size() > 0)
    {
        // For each black blob inside a white blob (ie, for each bin blob) 
        // take the rectangular portion containing the black blob and pass it
        // off to SuitDetector
        BinList candidateBins;
        
        int binNumber = 0;
        BOOST_FOREACH(BlobDetector::Blob binBlob, binBlobs)
        {
            processBin(binBlob, m_runSuitDetector, candidateBins, binNumber,
                       out);
            binNumber++;
        }
        
        if (candidateBins.size() > 1 && candidateBins.size() <= 4)
        {
            int curX = -1;
            int curY = -1;
            int prevX = -1;
            int prevY = -1;
            int binsCenterX = 0;
            int binsCenterY = 0;
            double innerAngles[3];//If you change this from a 3, also change the loops below
            int angleCounter = 0;
            BOOST_FOREACH(Bin bin, candidateBins)
            {
                binsCenterX += bin.getCenterX();
                binsCenterY += bin.getCenterY();
                prevX = curX;
                prevY = curY;
                curX = bin.getCenterX();
                curY = bin.getCenterY();
                
                if (prevX == -1 && prevY == -1)
                {
                    // the first one
                }
                else
                {
                    CvPoint prev;
                    CvPoint cur;
                    prev.x = prevX;
                    prev.y = prevY;
                    cur.x = curX;
                    cur.y = curY;
                    
                    //Swap so we always get answers mod 180.
                    if (prev.x > cur.x || (prev.x == cur.x && prev.y > cur.y))
                    {
                        CvPoint swap = prev;
                        prev = cur;
                        cur = swap;
                    }
                    
                    double innerAng = atan2(cur.y - prev.y,cur.x - prev.x);
                    
                    if (out)
                    {
                        cvLine(output, prev, cur, CV_RGB(255,0,0), 5, CV_AA, 0 );                    
                    }
                    innerAngles[angleCounter] = innerAng;
                    angleCounter++;
                }
            }
            
            double sinTotal = 0;
            double cosTotal = 0;
            for (int i = 0; i < angleCounter && i < 3; i++)
            {
                sinTotal+=sin(innerAngles[i]);
                cosTotal+=cos(innerAngles[i]);
            }
            
            double finalAngleAcrossBins = atan2(sinTotal,cosTotal);
            
            CvPoint drawStart, drawEnd;
            drawStart.x = binsCenterX/(angleCounter+1);
            drawStart.y = binsCenterY/(angleCounter+1);
            
            drawEnd.x = drawStart.x + cosTotal / (angleCounter) * 250;
            drawEnd.y = drawStart.y + sinTotal / (angleCounter) * 250;
            if (out)
            {
                cvLine(output, drawStart, drawEnd, CV_RGB(255,255,0),5, CV_AA,0); 
            }
    //        printf("final angle across bins %f:\n", finalAngleAcrossBins);
            
            math::Radian angleAcrossBins(finalAngleAcrossBins);
            
            math::Degree finalInnerAngleForJoe(angleAcrossBins.valueDegrees());
//            printf("Final Inner Angle For Joe: %f\n", finalInnerAngleForJoe.valueDegrees());
            BinEventPtr event(new BinEvent(finalInnerAngleForJoe));
            publish(EventType::MULTI_BIN_ANGLE, event);
        }
        
        // Sort candidate bins on distance from center
        candidateBins.sort(binToCenterComparer);
        
        // List of new bins
        BinList newBins;
        
        // Sort through our candidate bins and match them to the old ones
        BOOST_FOREACH(Bin candidateBin, candidateBins)
        {
            // Go through the list of current bins and find the closest bin
            double currentMin = 10000;
            Bin minBin;
            BOOST_FOREACH(Bin currentBin, m_bins)
            {
                double distance = currentBin.distanceTo(candidateBin);
                if (distance < currentMin)
                {
                    currentMin = distance;
                    minBin = currentBin;
                }
            }
            
            // If its close enough, we transfer the ID
            if (currentMin < m_sameBinThreshold)
            {
                // Transfer Id
                candidateBin._setId(minBin.getId());
                // Remove from list to search against
                m_bins.remove(minBin);
            }
            
            // Store bin in our list of new bins
            newBins.push_back(candidateBin);
        }


        // Anybody left we didn't find this iteration, so its been dropped
        BOOST_FOREACH(Bin bin, m_bins)
        {
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 
                                           bin.getSuit(), bin.getAngle()));
            event->id = bin.getId();
            publish(EventType::BIN_DROPPED, event);
        }

        // Track whether we lose the bin which is centered
        //if (m_centered)
        //{
        //    int centeredID;
        //}
        
        // Sort list by distance from center and copy it over the old one
        newBins.sort(binToCenterComparer);
        m_bins = newBins;
                
        // Now publish found & centered events
        math::Vector2 toCenter(getX(), getY());
        if (toCenter.normalise() < m_centeredLimit)
        {
            if(!m_centered)
            {
                m_centered = true;
                BinEventPtr event(new BinEvent(getX(), getY(), getSuit(), getAngle()));
                publish(EventType::BIN_CENTERED, event);
            }
        }
        else
        {
            m_centered = false;
        }
        
        BOOST_FOREACH(Bin bin, m_bins)
        {
            if (out)
                bin.draw(out);
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 
                                           bin.getSuit(), bin.getAngle()));
            event->id = bin.getId();
            publish(EventType::BIN_FOUND, event);
        }
    }
    
    
    //Publishing bin lost events example,
    // This is invalid!
/*    if (foundHeart && !seeHeart)
    {
        std::cout<<"Lost Heart"<<std::endl;
        BinEventPtr event(new BinEvent(binX, binY, Suit::HEART));
        publish(EventType::BIN_LOST, event);
    }
    if (foundSpade && !seeSpade)
    {
        std::cout<<"Lost Spade"<<std::endl;
        BinEventPtr event(new BinEvent(binX, binY, Suit::SPADE));
        publish(EventType::BIN_LOST, event);
    }
    if (foundDiamond && !seeDiamond)
    {
        std::cout<<"Lost Diamond"<<std::endl;
        BinEventPtr event(new BinEvent(binX, binY, Suit::DIAMOND));
        publish(EventType::BIN_LOST, event);
    }
    if (foundClub && !seeClub)
    {
        std::cout<<"Lost Club"<<std::endl; 
        BinEventPtr event(new BinEvent(binX, binY, Suit::CLUB));
        publish(EventType::BIN_LOST, event);
    }
    if (foundEmpty && !seeEmpty)
    {
        std::cout<<"Lost Empty"<<std::endl;
        BinEventPtr event(new BinEvent(binX, binY, Suit::NONEFOUND));
        publish(EventType::BIN_LOST, event);
        }
    
    foundHeart = seeHeart;
    foundSpade = seeSpade;
    foundDiamond = seeDiamond;
    foundClub = seeClub;
    foundEmpty = seeEmpty;*/
}

void BinDetector::drawBinImage(Image* imgToShow, int binNumber,
			       Image* output)
{
    Image::drawImage(imgToShow, 
		     binNumber * 128, 
		     0,
		     output, 
		     output);
}

void BinDetector::processBin(BlobDetector::Blob bin, bool detectSuit,
                             BinList& newBins, int binNum, Image* output)
{
    if (binNum > 3)
        return;
//        std::cout<<"Finished writing to output image"<<std::endl;
    double binX = bin.getCenterX();
    double binY = bin.getCenterY();

    // Map the image coordinate system to one 90 degrees rotated and with the
    // center in the middle of the image, and going from -1 to 1
    
    // Shift origin to the center
    binX = -1 * ((binFrame->width / 2) - binX);
    binY = (binFrame->height / 2) - binY;
    
    // Normalize (-1 to 1)
    binX = binX / ((double)(binFrame->width)) * 2.0;
    binY = binY / ((double)(binFrame->height)) * 2.0;
    
    // Account for the aspect ratio difference
    // 640/480      
    binX *= (double)binFrame->width / binFrame->height;

    // Create the image to hold the bin blob, give a little bit of extra space
    // around the black.  (10 pixels)
    int width = (bin.getMaxX()-bin.getMinX()+31)/4*4;
    int height = (bin.getMaxY()-bin.getMinY()+31)/4*4;


    //Regular sizing
    // Create the image to hold the bin blob
//    int width = (bin.getMaxX()-bin.getMinX()+1)/4*4;
//    int height = (bin.getMaxY()-bin.getMinY()+1)/4*4;
    OpenCVImage binImage(width, height);
    // Extra bin blob into image
    CvPoint2D32f binCenter = cvPoint2D32f(bin.getCenterX(), bin.getCenterY());
    
    if ((binCenter.x - width/2 - 1 < 0) || 
        (binCenter.x + width/2 + 1 >= binFrame->width) ||
        (binCenter.y - height/2 -1 < 0) ||
        (binCenter.y + height/2 +1 >= binFrame->height))
    {
        newBins.push_back(Bin(bin, binX, binY, math::Degree(0), m_binID++, Suit::UNKNOWN));
        return;
    }
    
    cvGetRectSubPix(binFrame, binImage.asIplImage(), binCenter);
    math::Radian angle = calculateAngleOfBin(bin, &binImage, &binImage);
    //Image::showImage(&binImage);
    // Find suit if desired
    Suit::SuitType suit = Suit::NONEFOUND;
    
    // Rotate image to straight
    OpenCVImage rotatedRedSuitWrapper(binImage.getWidth(),
                                          binImage.getHeight());
    vision::Image::transform(&binImage, &rotatedRedSuitWrapper, -angle);
    
    // Set fields as percent of colors
    OpenCVImage percentsRotatedRedWrapper(
        rotatedRedSuitWrapper.getWidth(),
        rotatedRedSuitWrapper.getHeight());
        
    cvCopyImage(rotatedRedSuitWrapper.asIplImage(),
                percentsRotatedRedWrapper.asIplImage());
    to_ratios(percentsRotatedRedWrapper.asIplImage());
        
    if (detectSuit)
    {

//        drawBinImage(&maskedRotatedRed, binNum);

        // Now mask just red (ie. make it white)
        suitMask(percentsRotatedRedWrapper.asIplImage(), rotatedRedSuitWrapper.asIplImage());
        
        if (cropImage(rotatedRedSuitWrapper.asIplImage(), binNum))
        {
            OpenCVImage wrapper(binImages[binNum],false);
//            Image::showImage(&wrapper);
            suit = determineSuit(&wrapper);
	    if (output)
               drawBinImage(&wrapper, binNum, output);
        }
        else
        {
            suit = Suit::UNKNOWN;
//            printf("No suit found\n");
        }
    }
    
    double angInDegrees = angle.valueDegrees();
    double angleToReturn = 90-angInDegrees;
    
    if (angleToReturn >= 90)
        angleToReturn -= 180;
    else if (angleToReturn < -90)
        angleToReturn += 180;
    
    math::Degree finalJoeAngle(angleToReturn);
    // Create bin add it to the list (and incremet the binID)
    newBins.push_back(Bin(bin, binX, binY, finalJoeAngle, m_binID++, suit));
    
    m_found = true;
}

void BinDetector::unrotateBin(math::Radian angleOfBin, Image* redSuit, 
			      Image* rotatedRedSuit)
{
    float m[6];
    CvMat M = cvMat( 2, 3, CV_32F, m );

    double factor = -angleOfBin.valueRadians();
    m[0] = (float)(cos(factor));
    m[1] = (float)(sin(factor));
    m[2] = redSuit->getWidth() * 0.5f;
    m[3] = -m[1];
    m[4] = m[0];
    m[5] = redSuit->getWidth() * 0.5f;
    
    cvGetQuadrangleSubPix(redSuit->asIplImage(), rotatedRedSuit->asIplImage(),&M);
}

math::Radian BinDetector::calculateAngleOfBin(BlobDetector::Blob bin, 
					      Image* input, Image* output)
{
    IplImage* redSuit = (IplImage*)(*input);
    IplImage* redSuitGrayScale = cvCreateImage(cvGetSize(redSuit),IPL_DEPTH_8U,1);

    cvCvtColor(redSuit,redSuitGrayScale,CV_BGR2GRAY);
    IplImage* cannied = cvCreateImage(cvGetSize(redSuitGrayScale), 8, 1 );

    cvCanny( redSuitGrayScale, cannied, 50, 200, 3 );
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    
//    OpenCVImage wrapper(cannied, false);
//    Image::showImage(&wrapper);
    lines = cvHoughLines2( cannied, storage, CV_HOUGH_PROBABILISTIC, 3, CV_PI/180, 150, 5, 50);
        
    float longestLineLength = -1;
    float angle = 0;
    for(int i = 0; i < lines->total; i++ )
    {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
        float lineX = line[1].x - line[0].x; 
        float lineY = line[1].y - line[0].y;
        
        if (output)
        {
            cvLine(output->asIplImage(), line[0], line[1], CV_RGB(255,255,0), 5, CV_AA, 0 );
        }
//        printf("Line dimensions: %f, %f\n", lineX, lineY);

        if (longestLineLength < (lineX * lineX + lineY * lineY))
        {
            angle = atan2(lineY,lineX);
            longestLineLength = lineX * lineX + lineY * lineY;
        }
    }
    
    if (lines->total == 0)
        return math::Degree(-180);
    cvReleaseImage(&cannied);
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&redSuitGrayScale);
    return math::Radian(angle);
}

Suit::SuitType BinDetector::determineSuit(Image* input,
                                          Image* output)
{
//        std::cout<<"finished suit detection"<<std::endl;
//        std::cout<<"Suit: " << suitDetector.getSuit()<<std::endl;
    suitDetector.processImage(input,output);
	
    // Filter suit type
    Suit::SuitType suitFound = suitDetector.getSuit(); //In case we ever want to use the suit detector...
    Suit::SuitType suit = Suit::NONEFOUND;

        
    if (suitFound == Suit::CLUB || suitFound == Suit::CLUBR90 || suitFound == Suit::CLUBR180 || suitFound == Suit::CLUBR270)
    {
        //seeClub = true;
        suit = Suit::CLUB;
//        std::cout<<"Found Club Bin"<<std::endl;
        
    }
    else if (suitFound == Suit::SPADE || suitFound == Suit::SPADER90 || suitFound == Suit::SPADER180 || suitFound == Suit::SPADER270)
    {
        //seeSpade = true;
        suit = Suit::SPADE;
//        std::cout<<"Found Spade Bin"<<std::endl;
        
    }
    else if (suitFound == Suit::HEART || suitFound == Suit::HEARTR90 || suitFound == Suit::HEARTR180 || suitFound == Suit::HEARTR270)
    {
        //seeHeart = true;
        suit = Suit::HEART;
//        std::cout<<"Found Heart Bin"<<std::endl;
    }
    else if (suitFound == Suit::DIAMOND || suitFound == Suit::DIAMONDR90 || suitFound == Suit::DIAMONDR180 || suitFound == Suit::DIAMONDR270)
    {
        //seeDiamond = true;
        suit = Suit::DIAMOND;
//        std::cout<<"Found Diamond Bin"<<std::endl;
    }
    else if (suitFound == Suit::UNKNOWN)
    {
        suit = Suit::UNKNOWN;
//        std::cout<<"Found an unknown bin, rotate above it until we figure out what it is!"<<std::endl;
    }
    else if (suitFound == Suit::NONEFOUND)
    {
        //seeEmpty = true;
        suit = Suit::NONEFOUND;
//        std::cout<<"Found empty Bin"<<std::endl;
    }
    return suit;
}
    
float BinDetector::getX()
{
    if (m_bins.size() > 0)
        return m_bins.front().getX();
    else
        return 0;
}
float BinDetector::getY()
{
    if (m_bins.size() > 0)
        return m_bins.front().getY();
    else
        return 0;
}
math::Degree BinDetector::getAngle()
{
    if (m_bins.size() > 0)
        return m_bins.front().getAngle();
    else
        return math::Degree(0);
}

Suit::SuitType BinDetector::getSuit()
{
    if (m_bins.size() > 0)
        return m_bins.front().getSuit();
    else
        return Suit::NONEFOUND;
}

BinDetector::BinList BinDetector::getBins()
{
    return m_bins;
}

bool BinDetector::found()
{
    return m_found;
}

//Returns false on failure, puts suit into scaledRedSuit.
bool BinDetector::cropImage(IplImage* rotatedRedSuit, int binNum)
{   
    int minSuitX = 999999;
    int minSuitY = 999999;
    int maxSuitX = 0;
    int maxSuitY = 0;
    //            int redCX, redCY;
//    cvDilate(rotatedRedSuit,rotatedRedSuit,NULL, 1);
    OpenCVImage mySuit(rotatedRedSuit,false);
    //   int size = 0;
    blobDetector.setMinimumBlobSize(100);
    blobDetector.processImage(&mySuit);
    if (!blobDetector.found())
    {
        return false;
        //no suit found, don't make a histogram
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
//        binImages[binNum] = NULL;
    }
        
    onlyRedSuitRows = onlyRedSuitCols = (onlyRedSuitRows > onlyRedSuitCols ? onlyRedSuitRows : onlyRedSuitCols);


    if (onlyRedSuitRows >= rotatedRedSuit->width || onlyRedSuitCols >= rotatedRedSuit->height)
    {
        return false;
//        binImages[binNum] = NULL;
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
    
    cvResize(onlyRedSuit, binImages[binNum], CV_INTER_LINEAR);

//    for (int redIndex = 2; redIndex < 128 * 128 * 3; redIndex+=3)
//    {
//        if (binImages[binNum]->imageData[redIndex] != 0)
//            size++;
//    }

//    printf("Bin Num: %d, Size: %d", binNum, size);
    
    cvReleaseImage(&onlyRedSuit);
    return true;
}

void BinDetector::setSuitDetectionOn(bool on)
{
    m_runSuitDetector = on;
}
    
} // namespace vision
} // namespace ram
