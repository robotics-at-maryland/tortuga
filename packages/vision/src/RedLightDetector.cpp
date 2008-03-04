/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/RedLightDetector.cpp
 */

// STD Includes
#include <algorithm>
#include <cmath>

// Library Includes
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/RedLightDetector.h"
#include "vision/include/Events.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

namespace ram {
namespace vision {

RedLightDetector::RedLightDetector(core::ConfigNode config,
                                   core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0)
{
    init(config);
}
    
RedLightDetector::RedLightDetector(Camera* camera) :
    cam(camera)    
{
    init(core::ConfigNode::fromString("{}"));
}

void RedLightDetector::init(core::ConfigNode config)
{
    // Detection variables
    m_initialMinRedPixels = config["intialMinPixels"].asInt(400);
    minRedPixels = m_initialMinRedPixels;
    
    m_foundMinPixelScale = config["lostMinPixelScale"].asDouble(0.85);
    m_lostMinPixelScale = config["foundMinPixelScale"].asDouble(0.75);
    m_almostHitPixels = config["almostHitPixels"].asInt((int)(640*480*0.2));
    m_topRemovePercentage = config["topRemovePercentage"].asDouble(0);

    
    // State machine variables 
    found=false;
    lightFramesOff=0;
    lightFramesOn=0;
    blinks=0;
    spooky=0;
    startCounting=false;
    lightCenter.x=0;
    lightCenter.y=0;

    // Working images
    frame = new ram::vision::OpenCVImage(640,480);
    image=cvCreateImage(cvSize(480,640),8,3);//480 by 640 if we put the camera on sideways again...
    raw=cvCreateImage(cvGetSize(image),8,3);
    flashFrame=cvCreateImage(cvGetSize(image), 8, 3);
    saveFrame=cvCreateImage(cvSize(640,480),8,3);    
}
    
RedLightDetector::~RedLightDetector()
{
    delete frame;
    cvReleaseImage(&flashFrame);
    cvReleaseImage(&image);
    cvReleaseImage(&raw);
    cvReleaseImage(&saveFrame);
}

double RedLightDetector::getX()
{
    return redLightCenterX;
}

double RedLightDetector::getY()
{
    return redLightCenterY;
}

void RedLightDetector::show(char* window)
{
    //Chris:  If you want to see an image other than the raw image with a box drawn if light found
    //You need to change the IplImage* being drawn here.  To see the effects of the red mask you want to change this line to flashFrame.
    //To see the mask calibrations being used for the red mask, you need to open up main.cpp  and find redMask.
    cvShowImage(window,((IplImage*)(raw)));
}


IplImage* RedLightDetector::getAnalyzedImage()
{
    rotate90DegClockwise(raw, saveFrame);
    return (IplImage*)(saveFrame);
}


void RedLightDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}
    
void RedLightDetector::processImage(Image* input, Image* output)
{
    IplImage* sideways =(IplImage*)(*input);
    rotate90Deg(sideways,image);//  Don't do this unless we put the cameras on sideways again...
	//	image=(IplImage*)(*frame);
    cvCopyImage(image,raw);//Now both are rotated 90 degrees
    cvCopyImage(image, flashFrame);

    // Remove top chunck if desired
    if (m_topRemovePercentage != 0)
    {
        int linesToRemove = (int)(m_topRemovePercentage * image->height);
        size_t bytesToBlack = linesToRemove * image->width * 3;
        memset(image->imageData, 0, bytesToBlack);
        memset(flashFrame->imageData, 0, bytesToBlack);
    }
    
    to_ratios(image);
    CvPoint p;
    redMask(image,flashFrame);
    
    int redPixelCount = histogram(flashFrame,&p.x,&p.y);
    
    if (redPixelCount<minRedPixels)
    {
        // Just lost the light so issue a lost event
        if (found)
            publish(EventType::LIGHT_LOST, core::EventPtr(new core::Event()));
            
        
        p.x=p.y=-1;
        found=false; //Completely ignoring the state machine for the time being.
        if (minRedPixels > m_initialMinRedPixels)
            minRedPixels = (int)(minRedPixels * m_lostMinPixelScale);
        else
            minRedPixels = m_initialMinRedPixels;;
    }	
    else
    {   
        minRedPixels=(int)(redPixelCount * m_foundMinPixelScale);
        found=true; //completely ignoring the state machine for the time being.
//	        	        cout<<"FOUND RED LIGHT "<<endl;
        CvPoint tl,tr,bl,br;
        tl.x = bl.x = std::max(p.x-4,0);
        tr.x = br.x = std::min(p.x+4,raw->width-1);
        tl.y = tr.y = std::min(p.y+4,raw->height-1);
        br.y = bl.y = std::max(p.y-4,0);
	
        cvLine(raw, tl, tr, CV_RGB(0,0,255), 3, CV_AA, 0 );
        cvLine(raw, tl, bl, CV_RGB(0,0,255), 3, CV_AA, 0 );
        cvLine(raw, tr, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
        cvLine(raw, bl, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
        lightCenter.x = p.x;
        lightCenter.y = p.y;

        redLightCenterX = lightCenter.x;
        redLightCenterY = lightCenter.y;
    
        // Shift origin to the center
        redLightCenterX = -1 * ((image->width / 2) - redLightCenterX);
        redLightCenterY = (image->height / 2) - redLightCenterY;
    
        // Normalize (-1 to 1)
        redLightCenterX = redLightCenterX / ((double)(image->width)) * 2.0;
        redLightCenterY = redLightCenterY / ((double)(image->height)) * 2.0;

        RedLightEventPtr event(new RedLightEvent(0, 0));
        event->x = redLightCenterX;
        event->y = redLightCenterY;
        event->azimuth = math::Degree((78.0 / 2) * event->x * -1);
        event->elevation = math::Degree((105.0 / 2) * event->y * 1);

        // Compute range (assume a sphere)
        double lightRadius = 0.25; // feet
        event->range = (lightRadius * image->width) /
            (sqrt((double)redPixelCount/M_PI) * tan(78.0/2 * (M_PI/180)));
        
        publish(EventType::LIGHT_FOUND, event);

        // Tell the watcher we are really freaking close to the light
        if (redPixelCount > m_almostHitPixels)
        {
            publish(EventType::LIGHT_ALMOST_HIT,
                    core::EventPtr(new core::Event()));
        }
    }

    
/*    if (found)
    {
        std::cout << 1 <<" "<< redLightCenterX << " "
                  << redLightCenterY << " " << redPixelCount
                  << std::endl;
    }
    else
    {
        std::cout << "0 0 0 0" << std::endl;
        }*/

    if (output)
    {
        OpenCVImage temp(raw, false);
        output->copyFrom(&temp);
    }
}

} // namespace vision
} // namespace ram
