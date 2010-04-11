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
#include <sstream>

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
    m_range(0),
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
                         "How \"skinny\" a blob can be",
                         4.0, &m_maxAspectRatio, 1.0, 10.0);

    propSet->addProperty(config, false, "minAspectRatio",
                         "How \"fat\" a blob can be",
                         0.8, &m_minAspectRatio, 0.0, 1.0);

    
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
    m_filter->addPropertiesToSet(propSet, &config,
                                 "L", "L*",
                                 "U", "Blue Chrominance",
                                 "V", "Red Chrominance",
                                 0, 255,  // L defaults
                                 0, 75,  // U defaults (0, 75)
                                 137, 247); // V defaults (137, 181)

    /// TODO: add a found pixel drop off

    // Make sure the configuration is valid
    //propSet->verifyConfig(config, true);
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
    cvCvtColor(input->asIplImage(), m_image->asIplImage(), CV_BGR2Luv);

    // Filter for green
    filterForGreen(m_image);

    // Find all the green blobs
    m_blobDetector.setMinimumBlobSize(m_minGreenPixels);
    m_blobDetector.processImage(m_image);
    std::vector<BlobDetector::Blob> greenBlobs = m_blobDetector.getBlobs();

    // Filter the blobs to get only the valid candidate target blobs
    std::vector<BlobDetector::Blob> candidateBlobs;
    processGreenBlobs(greenBlobs, candidateBlobs);

    // Determine if we found the target
    BlobDetector::Blob targetBlob;
    if (candidateBlobs.size() > 0)
    {
        m_found = true;
        targetBlob = candidateBlobs[0];
    }
    else
    {
        // Just lost the light so issue a lost event
        if (m_found)
            publish(EventType::TARGET_LOST, core::EventPtr(new core::Event()));
        m_found = false;
    }


    if (m_found)
    {
        // Determine the corindates of the target
        Detector::imageToAICoordinates(m_image, 
                                       targetBlob.getCenterX(),
                                       targetBlob.getCenterY(),
                                       m_targetCenterX,
                                       m_targetCenterY);

        // Determine range
        m_range = 1.0 - (((double)targetBlob.getHeight()) /
                         ((double)m_image->getHeight()));

        // Determine the squareness
        double aspectRatio = targetBlob.getTrueAspectRatio();
        if (aspectRatio < 1)
            m_squareNess = 1.0;
        else
            m_squareNess = 1.0/aspectRatio;

        // Notify every that we have found the target
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
            if ((*inData))
            {
                *outData = 147; // B
                *(outData + 1) = 20; // G
                *(outData + 2) = 255; // R
            }
	    
            inData += 3;
            outData += 3;
        }

        // Draw all blobs
        BOOST_FOREACH(BlobDetector::Blob blob, candidateBlobs)
        {
            blob.draw(output, false);
            blob.drawStats(output);
        }

        /*BOOST_FOREACH(BlobDetector::Blob blob, nullBlobs)
          {
          blob.draw(output, false, 255, 0, 0);
          blob.drawStats(output);
          }*/

        // Draw our target blob if we found it
        if (m_found)
        {
       	    targetBlob.draw(output, true);
            targetBlob.drawStats(output);
            
            std::stringstream ss;
            ss.precision(2);
            ss << "S:" << m_squareNess;
            targetBlob.drawTextUL(output, ss.str());
            
            std::stringstream ss2;
            ss2.precision(2);
            ss2 << "R:" << m_range;
            targetBlob.drawTextUL(output, ss2.str(), 0, 15);
        }
    }

    /// TODO: consider detection stragies for side on blob
    /// TODO: do some blob merging as needed

    // Invert the images and find all the non-green blobs
    /*unsigned char* data = m_image->getData();
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
      std::vector<BlobDetector::Blob> nullBlobs = m_blobDetector.getBlobs();*/
}

void TargetDetector::publishFoundEvent()
{
    if (m_found)
    {
        TargetEventPtr event(new TargetEvent(
                                 m_targetCenterX,
                                 m_targetCenterY,
                                 m_squareNess,
                                 m_range));
        
        publish(EventType::TARGET_FOUND, event);
    }
}

void TargetDetector::filterForGreen(Image* input)
{
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

}
    
bool TargetDetector::processGreenBlobs(const BlobDetector::BlobList& blobs,
                                       BlobDetector::BlobList& outBlobs)
{
    bool foundBlobs = false;
    
    //Assumed sorted biggest to smallest by BlobDetector.
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Determine if we have a valid green blob
        double aspectRatio = blob.getTrueAspectRatio();
        bool aspectRatioSmallEnough = aspectRatio < m_maxAspectRatio;
        bool aspectRatioBigEnough = m_minAspectRatio < aspectRatio;
        if (aspectRatioSmallEnough && aspectRatioBigEnough)
        {
            outBlobs.push_back(blob);
            foundBlobs = true;
        }
    }

    return foundBlobs;
}
    
    
} // namespace vision
} // namespace ram
