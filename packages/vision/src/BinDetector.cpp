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
    
BinDetector::Bin::Bin(double x, double y, math::Degree angle, int id, Suit::SuitType suit) :
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
    
BinDetector::BinDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    suitDetector(config,eventHub),
    blobDetector(config,eventHub),
    m_centered(false)
{
    init(config);
}
    
void BinDetector::init(core::ConfigNode config)
{
    rotated = cvCreateImage(cvSize(640,480),8,3);//Its only 480 by 640 if the cameras on sideways
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
    m_binID = 0;
}
    
BinDetector::~BinDetector()
{
    cvReleaseImage(&binFrame);
    cvReleaseImage(&rotated);
    cvReleaseImage(&bufferFrame);
}

void BinDetector::processImage(Image* input, Image* out)
{
    /*First argument to white_detect is a ratios frame, then a regular one*/
    IplImage* image =(IplImage*)(*input);
    IplImage* output = NULL;
    if (out != NULL)
        output = (IplImage*)(*out);
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
    /*int totalWhiteCount = */white_mask(image,binFrame, whiteMaskedFrame);
    /*int totalBlackCount = */black_mask(image,binFrame, blackMaskedFrame);

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

    // NOTE: all blobs sorted largest to smallest
    BOOST_FOREACH(BlobDetector::Blob blackBlob, blackBlobs)
    {
        BOOST_FOREACH(BlobDetector::Blob whiteBlob, whiteBlobs)
        {
            // Sadly, this totally won't work at the edges of the screen...
            // crap damn.
            if (whiteBlob.containsInclusive(blackBlob))
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
        publish(EventType::BIN_LOST, core::EventPtr(new core::Event()));
    }
    
    if (binBlobs.size() > 0)
    {
        // For each black blob inside a white blob (ie, for each bin blob) 
        // take the rectangular portion containing the black blob and pass it
        // off to SuitDetector
        BinList candidateBins;
    
        BOOST_FOREACH(BlobDetector::Blob binBlob, binBlobs)
        {
            processBin(binBlob, m_runSuitDetector, candidateBins, out);
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
            Bin minBin(0,0,math::Degree(0), 0, Suit::NONEFOUND);
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
                BinEventPtr event(new BinEvent(getX(), getY(), getSuit()));
                publish(EventType::BIN_CENTERED, event);
            }
        }
        else
        {
            m_centered = false;
        }
        
        BOOST_FOREACH(Bin bin, m_bins)
        {
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 
                                           bin.getSuit()));
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


void BinDetector::processBin(BlobDetector::Blob bin, bool detectSuit,
                             BinList& newBins, Image* output)
{
    if (output)
    {
        IplImage* out = output->asIplImage();
        // Draw green rectangle around the blob
        CvPoint tl,tr,bl,br;
        tl.x=bl.x=bin.getMinX();
        tr.x=br.x=bin.getMaxX();
        tl.y=tr.y=bin.getMinY();
        bl.y=br.y=bin.getMaxY();
        cvLine(out, tl, tr, CV_RGB(0,255,0), 3, CV_AA, 0 );
        cvLine(out, tl, bl, CV_RGB(0,255,0), 3, CV_AA, 0 );
        cvLine(out, tr, br, CV_RGB(0,255,0), 3, CV_AA, 0 );
        cvLine(out, bl, br, CV_RGB(0,255,0), 3, CV_AA, 0 );
    }
//        std::cout<<"Finished writing to output image"<<std::endl;
    double binX = bin.getCenterX();
    double binY = bin.getCenterY();

    // Map the image cordinate system to one 90 degrees rotated and with the
    // center in the middle of the image, and going from -1 to 1
    binX /=640;
    binY /=480;
    
    binX -= .5;
    binY -= .5;
    
    binX *= 2;
    binY *= 2;
    
    binX *= 640.0/480.0;

    // Swap because of the image rotation
    float swap = binX;
    binX = binY;
    binY = swap;

    // Find suit if desired
    Suit::SuitType suit = Suit::NONEFOUND;
    if (detectSuit)
    {
        OpenCVImage wrapper(binFrame, false);
        suit = determineSuit(bin, &wrapper, output);
    }
    
    // Create bin add it to the list (and incremet the binID)
    newBins.push_back(Bin(binX, binY, math::Degree(0), m_binID++, suit));
    
    m_found = true;
}

Suit::SuitType BinDetector::determineSuit(BlobDetector::Blob bin, Image* input,
                                          Image* output)
{
    // Create the image to hold the bin blob
    int width = (bin.getMaxX()-bin.getMinX()+1)/4*4;
    int height = (bin.getMaxY()-bin.getMinY()+1)/4*4;
    OpenCVImage binImage(width, height);

    // Extra bin blob into image
    CvPoint2D32f binCenter = cvPoint2D32f(bin.getCenterX(), bin.getCenterY());
    cvGetRectSubPix(input->asIplImage(), binImage.asIplImage(), binCenter);
    
    // Process image to find suit
//        std::cout<<"starting suit detection"<<std::endl;
    suitDetector.processImage(&binImage);
//        std::cout<<"finished suit detection"<<std::endl;
//        std::cout<<"Suit: " << suitDetector.getSuit()<<std::endl;

    // Filter suit type
    Suit::SuitType suitFound = suitDetector.getSuit(); //In case we ever want to use the suit detector...
    Suit::SuitType suit = Suit::NONEFOUND;

        
    if (suitFound == Suit::CLUB || suitFound == Suit::CLUBR90 || suitFound == Suit::CLUBR180 || suitFound == Suit::CLUBR270)
    {
        //seeClub = true;
        suit = Suit::CLUB;
        std::cout<<"Found Club Bin"<<std::endl;
        
    }
    else if (suitFound == Suit::SPADE || suitFound == Suit::SPADER90 || suitFound == Suit::SPADER180 || suitFound == Suit::SPADER270)
    {
        //seeSpade = true;
        suit = Suit::SPADE;
        std::cout<<"Found Spade Bin"<<std::endl;
        
    }
    else if (suitFound == Suit::HEART || suitFound == Suit::HEARTR90 || suitFound == Suit::HEARTR180 || suitFound == Suit::HEARTR270)
    {
        //seeHeart = true;
        suit = Suit::HEART;
        std::cout<<"Found Heart Bin"<<std::endl;
    }
    else if (suitFound == Suit::DIAMOND || suitFound == Suit::DIAMONDR90 || suitFound == Suit::DIAMONDR180 || suitFound == Suit::DIAMONDR270)
    {
        //seeDiamond = true;
        suit = Suit::DIAMOND;
        std::cout<<"Found Diamond Bin"<<std::endl;
    }
    else if (suitFound == Suit::UNKNOWN)
    {
        suit = Suit::UNKNOWN;
        std::cout<<"Found an unknown bin, rotate above it until we figure out what it is!"<<std::endl;
    }
    else if (suitFound == Suit::NONEFOUND)
    {
        //seeEmpty = true;
        suit = Suit::NONEFOUND;
        std::cout<<"Found empty Bin"<<std::endl;
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
    
} // namespace vision
} // namespace ram
