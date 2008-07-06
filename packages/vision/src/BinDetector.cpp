/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/BinDetector.cpp
 */

// STD Includes
#include <algorithm>

// Library Includes
//#include "assert.h"
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
    
void BinDetector::processImage(Image* input, Image* output)
{
	/*First argument to white_detect is a ratios frame, then a regular one*/
	IplImage* image =(IplImage*)(*input);
	
	//This is only right if the camera is on sideways... again.
	//rotate90Deg(image,rotated);

	//Else just copy
	cvCopyImage(image,rotated);
	image=rotated;//rotated is poorly named when camera is on correctly... oh well.
	//Set image to a newly copied space so we don't write over opencv's private memory space...
	//since opencv has a bad habit of making assumptions about what I want to do. :)
	cvCopyImage(image,binFrame);
	
    //Fill in output image.
    if (output)
    {
        OpenCVImage temp(binFrame, false);
        output->copyFrom(&temp);
    }
    
	to_ratios(image);
	
	//image is now in percents, binFrame is now the base image.
	
	/*int totalWhiteCount = */white_mask(image,binFrame, whiteMaskedFrame);
	/*int totalBlackCount = */black_mask(image,binFrame, blackMaskedFrame);
	
    blobDetector.setMinimumBlobSize(1000);
    OpenCVImage whiteMaskWrapper(whiteMaskedFrame,false);
    blobDetector.processImage(&whiteMaskWrapper);
    if (!blobDetector.found())
    {
        //no blobs found.
        return;
    }
	std::vector<BlobDetector::Blob> whiteBlobs = blobDetector.getBlobs();

    blobDetector.setMinimumBlobSize(500);
    OpenCVImage blackMaskWrapper(blackMaskedFrame,false);
	blobDetector.processImage(&blackMaskWrapper);
    if (!blobDetector.found())
    {
        //no blobs found.
        return;
    }
    std::vector<BlobDetector::Blob> blackBlobs = blobDetector.getBlobs();
    
    std::sort(whiteBlobs.begin(), whiteBlobs.end(), 
              ram::vision::BlobDetector::BlobComparer::compare);
    std::sort(blackBlobs.begin(), blackBlobs.end(), 
              ram::vision::BlobDetector::BlobComparer::compare);

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
    
    //for each black blob inside a white blob (ie, for each bin blob)
    //take the rectangular portion containing the black blob and pass it off to SuitDetector
    for (unsigned int binBlobIndex = 0; binBlobIndex < binBlobs.size(); binBlobIndex++)
    {
        BlobDetector::Blob blackBlobOfBin = binBlobs[binBlobIndex];
        IplImage* redSuit = cvCreateImage(cvSize((blackBlobOfBin.getMaxX()-blackBlobOfBin.getMinX()+1)/4*4,(blackBlobOfBin.getMaxY()-blackBlobOfBin.getMinY()+1)/4*4), IPL_DEPTH_8U, 3);
        cvGetRectSubPix(binFrame, redSuit, cvPoint2D32f((blackBlobOfBin.getMaxX()+blackBlobOfBin.getMinX())/2, (blackBlobOfBin.getMaxY()+blackBlobOfBin.getMinY())/2));
        OpenCVImage redSuitWrapper(redSuit,true);
        suitDetector.processImage(&redSuitWrapper);
        
        
        //Suit suitFound = suitDetector.getSuit(); //In case we ever want to use the suit detector...


        float binX, binY;
        binX = blackBlobOfBin.getCenterX();
        binY = blackBlobOfBin.getCenterY();
        
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

        binX /=640;
        binY /=480;
        
        binX -= .5;
        binY -= .5;
        
        binX *= 2;
        binY *= 2;
        
        binX *= 640/480;
        
        bool stupid = true;
        //And now they're swapped... for some stupid reason.
        if (stupid)
        {
            float swap = binX;
            binX = binY;
            binY = swap;
        }
        
        m_found = true;
        BinEventPtr event(new BinEvent(binX, binY));
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

    }
    
    //Publishing bin lost events example
    //publish(EventType::BIN_LOST, core::EventPtr(new core::Event()));

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
