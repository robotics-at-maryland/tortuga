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
#include <boost/foreach.hpp>

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
    m_almostHitPercentage = config["almostHitPercentage"].asDouble(0.2);
    m_topRemovePercentage = config["topRemovePercentage"].asDouble(0);
    m_redPercentage = config["redPercentage"].asDouble(40);
    m_redIntensity = config["redIntensity"].asInt(200);
    m_maxAspectRatio = config["maxAspectRatio"].asDouble(2);
    
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
    return m_redLightCenterX;
}

double RedLightDetector::getY()
{
    return m_redLightCenterY;
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

    // Resize images if needed
    if ((image->width != (int)input->getHeight()) &&
        (image->height != (int)input->getWidth()))
    {
        cvReleaseImage(&image);
        image = cvCreateImage(cvSize(input->getHeight(), input->getWidth()), 8, 3);
        cvReleaseImage(&raw);
        raw=cvCreateImage(cvGetSize(image),8,3);
        cvReleaseImage(&flashFrame);
        flashFrame=cvCreateImage(cvGetSize(image), 8, 3);
    }
    
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

    // Process Image
    to_ratios(image);
    CvPoint boundUR = {0};
    CvPoint boundLL = {0};
    boundUR.x = 0;
    boundUR.y = 0;
    boundLL.x = 0;
    boundLL.y = 0;
    redMask(image, flashFrame, (int)m_redPercentage, m_redIntensity);
    
    // Find the red blobs
    m_blobDetector.setMinimumBlobSize(minRedPixels);
    OpenCVImage temp(flashFrame, false);
    m_blobDetector.processImage(&temp);
    
    // See if we have any
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();
    int redPixelCount = 0;

    // Determine if the light has been found or lost
    double lightPixelRadius = 0;

    if (blobs.size() > 0)
    {
        BlobDetector::Blob redBlob(0, 0, 0, 0, 0, 0, 0);

        // Attempt to find a valid blob
        if (processBlobs(blobs, redBlob))
        {
            // Record info
            //BlobDetector::Blob redBlob(blobs[0]);
            lightCenter.x = redBlob.getCenterX();
            lightCenter.y = redBlob.getCenterY();
            boundUR.x = redBlob.getMaxX();
            boundUR.y = redBlob.getMaxY();
            boundLL.x = redBlob.getMinX();
            boundLL.y = redBlob.getMinY();
            redPixelCount = redBlob.getSize();
            
            lightPixelRadius = sqrt((double)redPixelCount/M_PI);
            minRedPixels=(int)(redPixelCount * m_foundMinPixelScale);
        
            found=true; //completely ignoring the state machine for the time being.
//	        	        cout<<"FOUND RED LIGHT "<<endl;
        }
    }	
    else
    {
        // Just lost the light so issue a lost event
        if (found)
            publish(EventType::LIGHT_LOST, core::EventPtr(new core::Event()));
            
        found=false; //Completely ignoring the state machine for the time being.
        if (minRedPixels > m_initialMinRedPixels)
            minRedPixels = (int)(minRedPixels * m_lostMinPixelScale);
        else
            minRedPixels = m_initialMinRedPixels;
    }

    // Do all the needed work if the light is found
    if (found)
    {
        // Shift origin to the center
        m_redLightCenterX = -1 * ((image->width / 2) - lightCenter.x);
        m_redLightCenterY = (image->height / 2) - lightCenter.y;
    
        // Normalize (-1 to 1)
        m_redLightCenterX = m_redLightCenterX / ((double)(image->width)) * 2.0;
        m_redLightCenterY = m_redLightCenterY / ((double)(image->height)) * 2.0;

        // Account for the aspect ratio difference
        // 640/480
        m_redLightCenterX *= (double)image->height / image->width;

        publishFoundEvent(lightPixelRadius);
        
        // Tell the watcher we are really freaking close to the light
        int pixelThreshold = (int)(input->getHeight() * input->getWidth() *
                                   m_almostHitPercentage);
        if (redPixelCount > pixelThreshold)
        {
            publish(EventType::LIGHT_ALMOST_HIT,
                    core::EventPtr(new core::Event()));
        }
    }

    
/*    if (found)
    {
        std::cout << 1 <<" "<< m_redLightCenterX << " "
                  << m_redLightCenterY << " " << redPixelCount
                  << std::endl;
    }
    else
    {
        std::cout << "0 0 0 0" << std::endl;
        }*/

    if (output)
    {
        // Only draw debug info if we found the light
        if (found)
        {
            CvPoint tl,tr,bl,br;
            tl.x = bl.x = std::max(lightCenter.x-4,0);
            tr.x = br.x = std::min(lightCenter.x+4,raw->width-1);
            tl.y = tr.y = std::min(lightCenter.y+4,raw->height-1);
            br.y = bl.y = std::max(lightCenter.y-4,0);
            
            cvLine(raw, tl, tr, CV_RGB(0,0,255), 3, CV_AA, 0 );
            cvLine(raw, tl, bl, CV_RGB(0,0,255), 3, CV_AA, 0 );
            cvLine(raw, tr, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
            cvLine(raw, bl, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
            
            cvRectangle(raw, boundUR, boundLL, CV_RGB(0,255,0), 2, CV_AA, 0);
            
            // clamp values
            lightCenter.x = std::min(lightCenter.x, raw->width-1);
            lightCenter.x = std::max(lightCenter.x, 0);
            lightCenter.y = std::min(lightCenter.y, raw->height-1);
            lightCenter.y = std::max(lightCenter.y, 0);
            int radius = std::max((int)sqrt((double)redPixelCount/M_PI), 1);
         
            cvCircle(raw, lightCenter, radius, CV_RGB(0,255,0), 2, CV_AA, 0);
        }
        
        OpenCVImage temp(raw, false);
        output->copyFrom(&temp);
    }
}

void RedLightDetector::publishFoundEvent(double lightPixelRadius)
{
    if (found)
    {
        RedLightEventPtr event(new RedLightEvent(0, 0));
        
        event->x = m_redLightCenterX;
        event->y = m_redLightCenterY;
        event->azimuth = math::Degree(
            (78.0 / 2) * event->x * -1.0 *
            (double)flashFrame->width/flashFrame->height);
        event->elevation = math::Degree((105.0 / 2) * event->y * 1);
        
        // Compute range (assume a sphere)
        double lightRadius = 0.25; // feet
        event->range = (lightRadius * image->width) /
            (lightPixelRadius * tan(78.0/2 * (M_PI/180)));
        
        publish(EventType::LIGHT_FOUND, event);
    }
}
    
bool RedLightDetector::processBlobs(const BlobDetector::BlobList& blobs,
                                    BlobDetector::Blob& outBlob)
{
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        double boundWidth = fabs(blob.getMaxX() - blob.getMinX());
        double boundHeight = fabs(blob.getMaxY() - blob.getMinY());
        double aspectRatio = boundWidth / boundHeight;
        // Ensure its always positive
        if (aspectRatio < 1)
            aspectRatio = 1 / aspectRatio;
        
        // Determine if we have actual found the light
        if (aspectRatio < m_maxAspectRatio)
        {
            // Found our blob record and leave
            outBlob = blob;
            return true;
        }
    }
    
    return false;
}
    
    
} // namespace vision
} // namespace ram
