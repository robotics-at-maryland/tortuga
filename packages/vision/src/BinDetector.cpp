/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/BinDetector.cpp
 */


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
    Detector(eventHub),
    cam(0),
    m_centered(false)
{
    init(config);
}
    
BinDetector::BinDetector(Camera* camera) :
    cam(camera),
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
	m_found=0;
	binX=-1;
	binY=-1;
	binCount=0;

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
	int binx=-1;
	int biny=-1;
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
	
	to_ratios(image);
	binCount=white_detect(image,binFrame, bufferFrame, &binx,&biny);
	if (biny!=-1 && binx!=-1)
	{
		binY=binx;
		binY/=image->width;
		binX=biny;
		binX/=image->height;
                binX -= .5;
                binY -= .5;
                binX *= 2;
                binY *= 2;
                binY *= 640.0/480.0;
                
		m_found=true;
                
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
	else
	{
            if (m_found)
                publish(EventType::BIN_LOST, core::EventPtr(new core::Event()));
            
            m_found=false;
            binX=-1;
            binY=-1;
	}

        if (output)
        {
            // Mark up frame for debugging
            CvPoint tl,tr,bl,br;
            tl.x=bl.x= std::max(binx-4,0);
            tr.x=br.x= std::min(binx+4,binFrame->width-1);
            tl.y=tr.y= std::min(biny+4,binFrame->height-1);
            br.y=bl.y= std::max(biny-4,0);
            
            cvLine(binFrame, tl, tr, CV_RGB(0,255,0), 3, CV_AA, 0 );
            cvLine(binFrame, tl, bl, CV_RGB(0,255,0), 3, CV_AA, 0 );
            cvLine(binFrame, tr, br, CV_RGB(0,255,0), 3, CV_AA, 0 );
            cvLine(binFrame, bl, br, CV_RGB(0,255,0), 3, CV_AA, 0 );
            
            OpenCVImage temp(binFrame, false);
            output->copyFrom(&temp);
        }
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
    
double BinDetector::getX()
{
	return binX;
}

double BinDetector::getY()
{
	return binY;
}

} // namespace vision
} // namespace ram
