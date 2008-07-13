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
//#include <cassert>

// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/BinDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"

#include "math/include/Vector2.h"

namespace ram {
namespace vision {

BinDetector::BinDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub), cam(0), suitDetector(config,eventHub), blobDetector(config,eventHub), m_centered(false)
{
    init(config);
}
    
BinDetector::BinDetector(Camera* camera) :
    cam(camera), suitDetector(cam), blobDetector(100),
    m_centered(false)
{
    init(core::ConfigNode::fromString("{}"));
}

void BinDetector::init(core::ConfigNode config)
{
    frame = new OpenCVImage(640, 480);
    rotated = cvCreateImage(cvSize(640,480),8,3);//Its only 480 by 640 if the cameras on sideways
    binFrame =cvCreateImage(cvGetSize(rotated),8,3);
    bufferFrame = cvCreateImage(cvGetSize(rotated),8,3);
    memset(bufferFrame->imageData, 0,
           bufferFrame->width * bufferFrame->height * 3);
    whiteMaskedFrame = cvCreateImage(cvGetSize(rotated),8,3);
    blackMaskedFrame = cvCreateImage(cvGetSize(rotated),8,3);
    m_found=0;
    foundHeart = false;
    foundSpade = false;
    foundDiamond = false;
    foundClub = false;
    foundEmpty = false;
    numBinsFound=0;
    m_centeredLimit = config["centeredLimit"].asDouble(0.1);
}
    
BinDetector::~BinDetector()
{
    delete frame;
    cvReleaseImage(&binFrame);
    cvReleaseImage(&rotated);
    cvReleaseImage(&bufferFrame);
}

void BinDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}
    
void BinDetector::processImage(Image* input, Image* out)
{
    /*First argument to white_detect is a ratios frame, then a regular one*/
    IplImage* image =(IplImage*)(*input);
    IplImage* output = NULL;
    if (out != NULL)
        output = (IplImage*)(*out);
//    std::cout<<"startup"<<std::endl;
    
    //This is only right if the camera is on sideways... again.
    //rotate90Deg(image,rotated);

    //Else just copy
    cvCopyImage(image,rotated);
    image=rotated;//rotated is poorly named when camera is on correctly... oh well.
    //Set image to a newly copied space so we don't write over opencv's private memory space...
    //since opencv has a bad habit of making assumptions about what I want to do. :)
    cvCopyImage(image,binFrame);
    
    //Fill in output image.
    if (out)
    {
        OpenCVImage temp(binFrame, false);
        out->copyFrom(&temp);
    }
    
    to_ratios(image);
    
    //image is now in percents, binFrame is now the base image.
    
  //  std::cout<<"starting masks"<<std::endl;
    /*int totalWhiteCount = */white_mask(image,binFrame, whiteMaskedFrame);
    /*int totalBlackCount = */black_mask(image,binFrame, blackMaskedFrame);
    
  //  std::cout<<"masks complete"<<std::endl;
    blobDetector.setMinimumBlobSize(1000);
    OpenCVImage whiteMaskWrapper(whiteMaskedFrame,false);
    blobDetector.processImage(&whiteMaskWrapper);
  //  std::cout<<"One blob detection done"<<std::endl;
    if (!blobDetector.found())
    {
    //    std::cout<<"No white found"<<std::endl;

        //no blobs found.
    //    return;
    }
  //  std::cout<<"white found"<<std::endl;
    std::vector<BlobDetector::Blob> whiteBlobs = blobDetector.getBlobs();

    blobDetector.setMinimumBlobSize(500);
    OpenCVImage blackMaskWrapper(blackMaskedFrame,false);
    //std::cout<<"blob finding black"<<std::endl;
    blobDetector.processImage(&blackMaskWrapper);
    if (!blobDetector.found())
    {
        //no blobs found.
        //std::cout<<"no black found"<<std::endl;
    //    return;
    }
    
//    std::cout<<"black found"<<std::endl;
    // Blobs sorted largest to smaller
    std::vector<BlobDetector::Blob> blackBlobs = blobDetector.getBlobs();
    std::vector<BlobDetector::Blob> binBlobs;
    
    for (unsigned int blackBlobIndex = 0; blackBlobIndex < blackBlobs.size(); blackBlobIndex++)
    {
        for (unsigned int whiteBlobIndex = 0; whiteBlobIndex < whiteBlobs.size(); whiteBlobIndex++)
        {
            //Sadly, this totally won't work at the edges of the screen... crap damn.
            if (whiteBlobs[whiteBlobIndex].containsInclusive(blackBlobs[blackBlobIndex]))
            {
                //blackBlobs[blackBlobIndex] is the black rectangle of a bin
                binBlobs.push_back(blackBlobs[blackBlobIndex]);
                break;//Don't add it once for each white blob containing it, thatd be dumb.
            }
            else
            {
                //Not a bin.
            }
        }
    }
    
//    std::cout<<"Made set of bins"<<std::endl;
//    std::cout<<"Num bins found: " << binBlobs.size() <<std::endl;
    
    bool seeHeart=false;
    bool seeDiamond=false;
    bool seeClub=false;
    bool seeSpade=false;
    bool seeEmpty=false;
    //for each black blob inside a white blob (ie, for each bin blob)
    //take the rectangular portion containing the black blob and pass it off to SuitDetector
    for (unsigned int binBlobIndex = 0; binBlobIndex < binBlobs.size(); binBlobIndex++)
    {
//        std::cout<<"bin " << binBlobIndex<<std::endl;
        BlobDetector::Blob blackBlobOfBin = binBlobs[binBlobIndex];
        IplImage* redSuit = cvCreateImage(cvSize((blackBlobOfBin.getMaxX()-blackBlobOfBin.getMinX()+1)/4*4,(blackBlobOfBin.getMaxY()-blackBlobOfBin.getMinY()+1)/4*4), IPL_DEPTH_8U, 3);
        cvGetRectSubPix(binFrame, redSuit, cvPoint2D32f((blackBlobOfBin.getMaxX()+blackBlobOfBin.getMinX())/2, (blackBlobOfBin.getMaxY()+blackBlobOfBin.getMinY())/2));
        OpenCVImage redSuitWrapper(redSuit,true);
//        std::cout<<"starting suit detection"<<std::endl;
        suitDetector.processImage(&redSuitWrapper);
//        std::cout<<"finished suit detection"<<std::endl;
//        std::cout<<"Suit: " << suitDetector.getSuit()<<std::endl;
        
        Suit::SuitType suitFound = suitDetector.getSuit(); //In case we ever want to use the suit detector...
        Suit::SuitType suit = Suit::NONEFOUND;
        binX = blackBlobOfBin.getCenterX();
        binY = blackBlobOfBin.getCenterY();
        
        if (suitFound == Suit::CLUB || suitFound == Suit::CLUBR90 || suitFound == Suit::CLUBR180 || suitFound == Suit::CLUBR270)
        {
            seeClub = true;
            suit = Suit::CLUB;
            std::cout<<"Found Club Bin"<<std::endl;

        }
        else if (suitFound == Suit::SPADE || suitFound == Suit::SPADER90 || suitFound == Suit::SPADER180 || suitFound == Suit::SPADER270)
        {
            seeSpade = true;
            suit = Suit::SPADE;
            std::cout<<"Found Spade Bin"<<std::endl;

        }
        else if (suitFound == Suit::HEART || suitFound == Suit::HEARTR90 || suitFound == Suit::HEARTR180 || suitFound == Suit::HEARTR270)
        {
            seeHeart = true;
            suit = Suit::HEART;
            std::cout<<"Found Heart Bin"<<std::endl;
        }
        else if (suitFound == Suit::DIAMOND || suitFound == Suit::DIAMONDR90 || suitFound == Suit::DIAMONDR180 || suitFound == Suit::DIAMONDR270)
        {
            seeDiamond = true;
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
            seeEmpty = true;
            suit = Suit::NONEFOUND;
            std::cout<<"Found empty Bin"<<std::endl;
        }
            
//        std::cout<<"Writing to output image"<<std::endl;
        if (output)
        {
            CvPoint tl,tr,bl,br;
            tl.x=bl.x=blackBlobOfBin.getMinX();
            tr.x=br.x=blackBlobOfBin.getMaxX();
            tl.y=tr.y=blackBlobOfBin.getMinY();
            bl.y=br.y=blackBlobOfBin.getMaxY();
            cvLine(output, tl, tr, CV_RGB(0,255,0), 3, CV_AA, 0 );
            cvLine(output, tl, bl, CV_RGB(0,255,0), 3, CV_AA, 0 );
            cvLine(output, tr, br, CV_RGB(0,255,0), 3, CV_AA, 0 );
            cvLine(output, bl, br, CV_RGB(0,255,0), 3, CV_AA, 0 );
        }
//        std::cout<<"Finished writing to output image"<<std::endl;


//This part here just looks completely wrong... It'll pass the tests sure...
//But I still think its completely wrong.
        binX /=640;
        binY /=480;
        
        binX -= .5;
        binY -= .5;
        
        binX *= 2;
        binY *= 2;
        
        binX *= 640.0/480.0;
        
        bool stupid = true;
        //And now they're swapped... for some stupid reason.
        if (stupid)
        {
            float swap = binX;
            binX = binY;
            binY = swap;
        }

        
        m_found = true;
        BinEventPtr event(new BinEvent(binX, binY, suit));
        publish(EventType::BIN_FOUND, event);

        // Determine Centered
        math::Vector2 toCenter(binX, binY);
        if (toCenter.normalise() < m_centeredLimit)
        {
            if(!m_centered)
            {
                m_centered = true;
                publish(EventType::BIN_CENTERED, event);
            }
        }
        else
        {
            m_centered = false;
        }
//        std::cout<<"End of bin"<<std::endl;
    }
    
    //Publishing bin lost events example
    if (foundHeart && !seeHeart)
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
    foundEmpty = seeEmpty;
}

float BinDetector::getX()
{
return binX;
}
float BinDetector::getY()
{
return binY;
}

void BinDetector::show(char* window)
{
    cvShowImage(window,((IplImage*)(binFrame)));
}

IplImage* BinDetector::getAnalyzedImage()
{
    return (IplImage*)(binFrame);
}

bool BinDetector::found()
{
    return m_found;
}
    
} // namespace vision
} // namespace ram
