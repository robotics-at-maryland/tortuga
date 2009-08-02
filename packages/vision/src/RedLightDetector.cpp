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
#include <limits>
// Library Includes
#include "highgui.h"
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/RedLightDetector.h"
#include "vision/include/Events.h"
#include "vision/include/ColorFilter.h"

#include "core/include/PropertySet.h"

#ifndef M_PI
#define M_PI 3.1415926
#endif

namespace ram {
namespace vision {

RedLightDetector::RedLightDetector(core::ConfigNode config,
                                   core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_filter(0)
{
    init(config);
}
    
RedLightDetector::RedLightDetector(Camera* camera) :
    cam(camera),
    m_filter(0)
{
    init(core::ConfigNode::fromString("{}"));
}

void RedLightDetector::init(core::ConfigNode config)
{
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());
    propSet->addProperty(config, false, "initialMinPixels",
        "Red pixel count required for blob to a buoy",
        400, &m_initialMinRedPixels);
    minRedPixels = m_initialMinRedPixels;

    propSet->addProperty(config, false, "foundMinPixelScale",
        "Scale value for the buoy pixel count when its found",
        0.85, &m_foundMinPixelScale);
    
    propSet->addProperty(config, false, "lostMinPixelScale",
        "Scale value for the buoy pixel count before its found",
        0.75, &m_lostMinPixelScale);

    propSet->addProperty(config, false, "almostHitPercentage",
        "% of visible screen at which point the ball is considered \"hit\"",
        0.2, &m_almostHitPercentage);

    propSet->addProperty(config, false, "topRemovePercentage",
        "% of the screen from the top to be blacked out",
        0.0, &m_topRemovePercentage);

    propSet->addProperty(config, false, "bottomRemovePercentage",
        "% of the screen from the bottom to be blacked out",
        0.0, &m_bottomRemovePercentage);

    propSet->addProperty(config, false, "leftRemovePercentage",
        "% of the screen from the left to be blacked out",
        0.0, &m_leftRemovePercentage);

    propSet->addProperty(config, false, "rightRemovePercentage",
        "% of the screen from the right to be blacked out",
        0.0, &m_rightRemovePercentage);
    
    propSet->addProperty(config, false, "redPercentage",
        "Minimum % of total color in the pixel which must be red",
        40.0, &m_redPercentage);

    propSet->addProperty(config, false, "redIntensity",
        "Minimum absolute intensity of the red pixel value",
        200, &m_redIntensity);

    propSet->addProperty(config, false, "maxAspectRatio",
        "Max aspect ratio of the blob",
        2.0, &m_maxAspectRatio);

    // Color filter
    propSet->addProperty(config, false, "useLUVFilter",
        "Use LUV based color filter",  false, &m_useLUVFilter);

    m_filter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_filter->addPropertiesToSet(propSet, &config,
                                 "L", "L*",
                                 "U", "Blue Chrominance",
                                 "V", "Red Chrominance",
                                 0, 255,  // L defaults // 180,255
                                 0, 200,  // U defaults // 76, 245
                                 200, 255); // V defaults // 200,255

    // Make sure the configuration is valid
    //propSet->verifyConfig(config, true);
    
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
    image=cvCreateImage(cvSize(640,480),8,3);//480 by 640 if we put the camera on sideways again...
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

void RedLightDetector::setTopRemovePercentage(double percent)
{
    m_topRemovePercentage = percent;
}

void RedLightDetector::setBottomRemovePercentage(double percent)
{
    m_bottomRemovePercentage = percent;
}

void RedLightDetector::setUseLUVFilter(bool value)
{
    m_useLUVFilter = value;
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
    // Resize images if needed
    if ((image->width != (int)input->getWidth()) &&
        (image->height != (int)input->getHeight()))
    {
        cvReleaseImage(&image);
        image = cvCreateImage(cvSize(input->getWidth(),
                                     input->getHeight()), 8, 3);
        cvReleaseImage(&raw);
        raw=cvCreateImage(cvGetSize(image),8,3);
        cvReleaseImage(&flashFrame);
        flashFrame=cvCreateImage(cvGetSize(image), 8, 3);
    }

    cvCopyImage(input->asIplImage(), image);
    cvCopyImage(image, flashFrame);
    
    // Remove top chunk if desired
    if (m_topRemovePercentage != 0)
    {
        int linesToRemove = (int)(m_topRemovePercentage * image->height);
        size_t bytesToBlack = linesToRemove * image->width * 3;
        memset(image->imageData, 0, bytesToBlack);
        memset(flashFrame->imageData, 0, bytesToBlack);
    }
    

    if (m_bottomRemovePercentage != 0)
    {
//        printf("Removing \n");
        int linesToRemove = (int)(m_bottomRemovePercentage * image->height);
        size_t bytesToBlack = linesToRemove * image->width * 3;
        memset(&(image->imageData[image->width * image->height * 3 - bytesToBlack]), 0, bytesToBlack);
        memset(&(flashFrame->imageData[flashFrame->width * flashFrame->height * 3 - bytesToBlack]), 0, bytesToBlack);
    }

    if (m_rightRemovePercentage != 0)
    {
        size_t lineSize = image->width * 3;
        size_t bytesToBlack = (int)(m_rightRemovePercentage * lineSize);;
        for (int i = 0; i < image->height; ++i)
        {
            size_t offset = i * lineSize - bytesToBlack;
            memset(image->imageData + offset, 0, bytesToBlack);
            memset(flashFrame->imageData + offset, 0, bytesToBlack);
        }
    }

    if (m_leftRemovePercentage != 0)
    {
        size_t lineSize = image->width * 3;
        size_t bytesToBlack = (int)(m_leftRemovePercentage * lineSize);;
        for (int i = 0; i < image->height; ++i)
        {
            size_t offset = i * lineSize;
            memset(image->imageData + offset, 0, bytesToBlack);
            memset(flashFrame->imageData + offset, 0, bytesToBlack);
        }
    }

    
    // Process Image
    CvPoint boundUR = {0};
    CvPoint boundLL = {0};
    boundUR.x = 0;
    boundUR.y = 0;
    boundLL.x = 0;
    boundLL.y = 0;

    if (m_useLUVFilter)
        filterForRedNew(flashFrame);
    else
        filterForRedOld(image, flashFrame);

    
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
	    // Transform to the AI's coordinates then publish the event
	    Detector::imageToAICoordinates(input, lightCenter.x, lightCenter.y,
					   m_redLightCenterX, m_redLightCenterY);
	    publishFoundEvent(lightPixelRadius);
	    
	    // Tell the watcher we are really freaking close to the light
	    int pixelSize = (int)(input->getHeight() * input->getWidth());
	    pixelSize = (int)(pixelSize * (1 - m_topRemovePercentage -
					   m_bottomRemovePercentage));
	    
	    int pixelThreshold = (int)(pixelSize * m_almostHitPercentage);
	    if (redPixelCount > pixelThreshold)
	    {
		publish(EventType::LIGHT_ALMOST_HIT,
			core::EventPtr(new core::Event()));
	    }
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
        cvCopyImage(flashFrame, raw);            
        // Only draw debug info if we found the light
        if (found)
        {
//            cvCopyImage(image, raw);
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


void RedLightDetector::filterForRedOld(IplImage* image, IplImage* flashFrame)
{
    to_ratios(image);
    redMask(image, flashFrame, (int)m_redPercentage, m_redIntensity);
}

void RedLightDetector::filterForRedNew(IplImage* image)
{
    cvCvtColor(image, image, CV_BGR2Luv);
    Image* tmpImage = new OpenCVImage(image, false);
    m_filter->filterImage(tmpImage);
    delete tmpImage;
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
            (double)flashFrame->height/flashFrame->width);
        event->elevation = math::Degree((105.0 / 2) * event->y * 1);
        
        // Compute range (assume a sphere)
        double lightRadius = 0.25; // feet
        event->range = (lightRadius * image->height) /
            (lightPixelRadius * tan(78.0/2 * (M_PI/180)));
        
        publish(EventType::LIGHT_FOUND, event);
    }
}
    
bool RedLightDetector::processBlobs(const BlobDetector::BlobList& blobs,
                                    BlobDetector::Blob& outBlob)
{
    //Assumed sorted biggest to smallest by BlobDetector.
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Determine if we have actual found the light
        if (blob.getAspectRatio() < m_maxAspectRatio)
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
