/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/TargetDetector.cpp
 */

// STD Includes
#include <algorithm>
#include <cmath>

// Library Includes
#include "highgui.h"
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/TargetDetector.h"
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/include/ColorFilter.h"

#include "core/include/PropertySet.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

namespace ram {
namespace vision {

TargetDetector::TargetDetector(core::ConfigNode config,
                               core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_filter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_image(new OpenCVImage(640, 480)),
    m_found(false),
    m_targetCenterX(0),
    m_targetCenterY(0),
    m_maxAspectRatio(0),
    m_topRemovePercentage(0),
    m_bottomRemovePercentage(0)
{
    init(config);
}
    
void TargetDetector::init(core::ConfigNode config)
{
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "topRemovePercentage",
        "% of the screen from the top to be blacked out",
        0.0, &m_topRemovePercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "bottomRemovePercentage",
        "% of the screen from the bottom to be blacked out",
        0.0, &m_bottomRemovePercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "maxAspectRatio",
        "Max aspect ratio of the blob",
        2.0, &m_maxAspectRatio, 0.0, 10.0);

    propSet->addProperty(config, false, "filtBMin",
        "Min blue value for a green pixel",  0,
	boost::bind(&ColorFilter::getChannel1Low, m_filter),
	boost::bind(&ColorFilter::setChannel1Low, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtBMax",
        "Max blue value for a green pixel",  255,
	boost::bind(&ColorFilter::getChannel1High, m_filter),
	boost::bind(&ColorFilter::setChannel1High, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtGMin",
        "Min green value for a green pixel",  0,
	boost::bind(&ColorFilter::getChannel2Low, m_filter),
	boost::bind(&ColorFilter::setChannel2Low, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtGMax",
        "Max green value for a green pixel",  255,
	boost::bind(&ColorFilter::getChannel2High, m_filter),
	boost::bind(&ColorFilter::setChannel2High, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtRMin",
        "Min red value for a green pixel",  0,
	boost::bind(&ColorFilter::getChannel3Low, m_filter),
	boost::bind(&ColorFilter::setChannel3Low, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtRMax",
        "Max red value for a green pixel",  255,
	boost::bind(&ColorFilter::getChannel3High, m_filter),
	boost::bind(&ColorFilter::setChannel3High, m_filter, _1), 0, 255);

    // Working images
    //    frame = new ram::vision::OpenCVImage(640,480);
}
    
TargetDetector::~TargetDetector()
{
  //    delete frame;
}

double TargetDetector::getX()
{
    return m_targetCenterX;
}

double TargetDetector::getY()
{
    return m_targetCenterY;
}

void TargetDetector::setTopRemovePercentage(double percent)
{
    m_topRemovePercentage = percent;
}

void TargetDetector::setBottomRemovePercentage(double percent)
{
    m_bottomRemovePercentage = percent;
}
    
void TargetDetector::processImage(Image* input, Image* output)
{
   m_image->copyFrom(input);

   // Filter the image
   m_filter->filterImage(m_image);

   // Do the debug display
   if (output)
   {
       // Make the output exactly match the input
       output->copyFrom(input);

       // Color all found pixels pink
       unsigned char* inData = m_image->getData();
       unsigned char* outData = output->getData();
       size_t numPixels = input->getHeight() * input->getWidth();
    
       for (size_t i = 0; i < numPixels; ++i)
       {
    	   if (*inData)
	   {
	     *outData = 147; // B
	     *(outData + 1) = 20; // G
	     *(outData + 2) = 255; // R
	   }
        
	   inData += 3;
	   outData += 3;
       }
   }

  /*    // Resize images if needed
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
        m_targetCenterX = -1 * ((image->width / 2) - lightCenter.x);
        m_targetCenterY = (image->height / 2) - lightCenter.y;
    
        // Normalize (-1 to 1)
        m_targetCenterX = m_targetCenterX / ((double)(image->width)) * 2.0;
        m_targetCenterY = m_targetCenterY / ((double)(image->height)) * 2.0;

        // Account for the aspect ratio difference
        // 640/480
        m_targetCenterX *= (double)image->width / image->height;

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

    
    //    if (found)
    //    {
    //        std::cout << 1 <<" "<< m_targetCenterX << " "
    //                  << m_targetCenterY << " " << redPixelCount
    //                  << std::endl;
    //    }
    //    else
    //    {
    //        std::cout << "0 0 0 0" << std::endl;
    //        }

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
	}*/
}

  /*void TargetDetector::publishFoundEvent(double lightPixelRadius)
{
    if (found)
    {
        TargetEventPtr event(new TargetEvent(0, 0));
        
        event->x = m_targetCenterX;
        event->y = m_targetCenterY;
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
    }*/
    
bool TargetDetector::processBlobs(const BlobDetector::BlobList& blobs,
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
