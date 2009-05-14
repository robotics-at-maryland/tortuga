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
    m_squareNess(0),
    m_distance(0),
    m_minGreenPixels(0),
    m_erodeIterations(0),
    m_dilateIterations(0),
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

    // Hack properties to deal noise from the surface of the water
    propSet->addProperty(config, false, "topRemovePercentage",
        "% of the screen from the top to be blacked out",
        0.0, &m_topRemovePercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "bottomRemovePercentage",
        "% of the screen from the bottom to be blacked out",
        0.0, &m_bottomRemovePercentage, 0.0, 1.0);

    // Standard tuning properties
    propSet->addProperty(config, false, "maxAspectRatio",
        "Max aspect ratio of the blob",
        2.0, &m_maxAspectRatio, 0.0, 10.0);

    propSet->addProperty(config, false, "minGreenPixels",
        "The minimum pixel count of the green target blob",
        500, &m_minGreenPixels, 0, 50000);

    propSet->addProperty(config, false, "erodeIterations",
        "How many times to erode the filtered image",
	0, &m_erodeIterations, 0, 10);

    propSet->addProperty(config, false, "dilateIterations",
        "How many times to dilate the filtered image",
         0, &m_dilateIterations, 0, 10);

    // Color filter properties
    propSet->addProperty(config, false, "filtYMin",
        "Min blue value for a green pixel",  1,
	boost::bind(&ColorFilter::getChannel1Low, m_filter),
	boost::bind(&ColorFilter::setChannel1Low, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtYMax",
        "Max blue value for a green pixel",  255,
	boost::bind(&ColorFilter::getChannel1High, m_filter),
	boost::bind(&ColorFilter::setChannel1High, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtUMin",
        "Min green value for a green pixel",  0,//80,
	boost::bind(&ColorFilter::getChannel2Low, m_filter),
	boost::bind(&ColorFilter::setChannel2Low, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtUMax",
	"Max green value for a green pixel",  255, //141,
	boost::bind(&ColorFilter::getChannel2High, m_filter),
	boost::bind(&ColorFilter::setChannel2High, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtVMin",
	"Min red value for a green pixel",  0, //98,
	boost::bind(&ColorFilter::getChannel3Low, m_filter),
	boost::bind(&ColorFilter::setChannel3Low, m_filter, _1), 0, 255);

    propSet->addProperty(config, false, "filtVMax",
        "Max red value for a green pixel",  119, //119,
	boost::bind(&ColorFilter::getChannel3High, m_filter),
	boost::bind(&ColorFilter::setChannel3High, m_filter, _1), 0, 255);

    /// TODO: add a found pixel drop off
}
    
TargetDetector::~TargetDetector()
{
    delete m_image;
}

bool TargetDetector::found()
{
    return m_found;
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
   // Ensure our working image is the same size
   if ((m_image->getWidth() != input->getWidth()) || 
       (m_image->getHeight() != input->getHeight()))
   {
       m_image->copyFrom(input);
   }
   
    // Copy to our working image and convert to YUV
    cvCvtColor(input->asIplImage(), m_image->asIplImage(), CV_BGR2YCrCb);

    // Remove top and bottom chunks if desired
    if (m_topRemovePercentage != 0)
    {
        int linesToRemove = (int)(m_topRemovePercentage * m_image->getHeight());
        size_t bytesToBlack = linesToRemove * m_image->getWidth() * 3;
        memset(m_image->getData(), 0, bytesToBlack);
    }

    if (m_bottomRemovePercentage != 0)
    {
        int linesToRemove = 
	    (int)(m_bottomRemovePercentage * m_image->getHeight());
        size_t bytesToBlack = linesToRemove * m_image->getWidth() * 3;
	int startIdx = 
            m_image->getWidth() * m_image->getHeight() * 3 - bytesToBlack;
        memset(&(m_image->getData()[startIdx]), 0, bytesToBlack);
    }

    // Filter the image so all green is white, and everything else is black
    m_filter->filterImage(m_image);

    if (m_erodeIterations)
    {
      cvErode(m_image->asIplImage(), m_image->asIplImage(), 0, 
	      m_erodeIterations);
    }
    if (m_dilateIterations)
    {
      cvDilate(m_image->asIplImage(), m_image->asIplImage(), 0, 
	       m_dilateIterations);
    }

    // Find all the green blobs
    m_blobDetector.setMinimumBlobSize(m_minGreenPixels);
    m_blobDetector.processImage(m_image);
    std::vector<BlobDetector::Blob> blobs = m_blobDetector.getBlobs();
    BlobDetector::Blob targetBlob;

    /// TODO: consider detection stragies for side on blob
    /// TODO: do some blob merging as needed

    // Invert the images and find all the non-green blobs
    unsigned char* data = m_image->getData();
    size_t numPixels = m_image->getHeight() * m_image->getWidth();
    
    for (size_t i = 0; i < numPixels; ++i)
    {
        if (*data)
	{
	    *data = 0; // B
	    *(data + 1) = 0; // G
	    *(data + 2) = 0; // R
	}	
	else
	{
	    *data = 255; // B
	    *(data + 1) = 255; // G
	    *(data + 2) = 255; // R
	}
	data += 3;
    }

    m_blobDetector.setMinimumBlobSize(300);
    m_blobDetector.processImage(m_image);
    std::vector<BlobDetector::Blob> nullBlobs = m_blobDetector.getBlobs();

    if (blobs.size() > 0)
    {
        m_found = true;
	targetBlob = blobs[0];
    }
    else
    {
        // Just lost the light so issue a lost event
        if (m_found)
            publish(EventType::TARGET_LOST, core::EventPtr(new core::Event()));
	m_found = false;
    }

    // Find all non-green blob with a green blob around them

    // Filter those based on criteria like similar aspect ratios and size
    // Also you can filter based on the how much of the combine space they take

    // Return the biggest one

    if (m_found)
    {
        Detector::imageToAICoordinates(m_image, 
				       targetBlob.getCenterX(),
				       targetBlob.getCenterY(),
				       m_targetCenterX,
				       m_targetCenterY);

        publishFoundEvent();
    }
    

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
            if (!(*inData))
	    {
	        *outData = 147; // B
		*(outData + 1) = 20; // G
		*(outData + 2) = 255; // R
	    }
	    
	    inData += 3;
	    outData += 3;
	}

	// Draw all blobs
	BOOST_FOREACH(BlobDetector::Blob blob, blobs)
	{
	    blob.draw(output, false);
	    blob.drawStats(output);
	}

	BOOST_FOREACH(BlobDetector::Blob blob, nullBlobs)
	{
	    blob.draw(output, false, 255, 0, 0);
	    blob.drawStats(output);
	}

	// Draw our target blob if we found it
	if (m_found)
	{
       	    targetBlob.draw(output, true);
	    targetBlob.drawStats(output);
	}
    }

  /*    // Resize images if needed


    // Do all the needed work if the light is found
    if (found)
    {
        
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

void TargetDetector::publishFoundEvent()
{
    if (m_found)
    {
        TargetEventPtr event(new TargetEvent(
	    m_targetCenterX,
	    m_targetCenterY,
	    m_squareNess,
	    m_distance));
        
        publish(EventType::TARGET_FOUND, event);
    }
}
    
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
