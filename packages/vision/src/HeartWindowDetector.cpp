/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/src/HeartWindowDetector.cpp
 */

// STD Includes
#include <assert.h>
#include <math.h>
#include <set>
#include <iostream>

// Library Includes
#include <boost/foreach.hpp>
#include "cv.h"

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

#include "vision/include/Camera.h"
#include "vision/include/Color.h"
#include "vision/include/Events.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/HeartWindowDetector.h"

namespace ram {
namespace vision {

HeartWindowDetector::HeartWindowDetector(core::ConfigNode config,
                               core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_redFilter(0),
    m_blueFilter(0),
    m_blobDetector(config, eventHub)
{
    init(config);
}

HeartWindowDetector::HeartWindowDetector(Camera* camera) :
    cam(camera),
    m_redFilter(0),
    m_blueFilter(0)
{
    init(core::ConfigNode::fromString("{}"));
}

HeartWindowDetector::~HeartWindowDetector()
{
    delete m_redFilter;
    delete m_blueFilter;

    delete frame;
    delete redFrame;
    delete blueFrame;
    delete processingFrame;
}

void HeartWindowDetector::init(core::ConfigNode config)
{
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "debug",
                         "Debug level", 2, &m_debug, 0, 2);

    propSet->addProperty(config, false, "maxAspectRatio",
                         "Maximum aspect ratio (width/height)",
                         1.1, &m_maxAspectRatio);

    propSet->addProperty(config, false, "minAspectRatio",
                         "Minimum aspect ratio (width/height)",
                         0.25, &m_minAspectRatio);

    propSet->addProperty(config, false, "minWidth",
                         "Minimum width for a blob",
                         50, &m_minWidth);

    propSet->addProperty(config, false, "minHeight",
                         "Minimum height for a blob",
                         50, &m_minHeight);

    propSet->addProperty(config, false, "maxCenterYDisagreement",
                        "Maximum distance between the window and background centers",
                        1000, &m_centerYDisagreement);

    propSet->addProperty(config, false, "minPixelPercentage",
                         "Minimum percentage of pixels / area",
                         0.1, &m_minPixelPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "maxPixelPercentage",
                         "Maximum percentage of pixels / area",
                         1.0, &m_maxPixelPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "innerMinPixelPercentage",
                         "Minimum percentage for inner background",
                         0.1, &m_innerMinPixelPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "erodeIterations",
                         "Number of times to erode the binary image",
                         0, &m_erodeIterations);

    propSet->addProperty(config, false, "dilateIterations",
                         "Number of times to dilate the binary image",
                         0, &m_dilateIterations);

    m_redFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_redFilter->addPropertiesToSet(propSet, &config,
                                    "RedL", "Red Luminance",
                                    "RedC", "Red Chrominance",
                                    "RedH", "Red Hue",
                                    0, 255, 0, 255, 0, 255);

    m_blueFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_blueFilter->addPropertiesToSet(propSet, &config,
                                     "BlueL", "Blue Luminance",
                                     "BlueC", "Blue Chrominance",
                                     "BlueH", "Blue Hue",
                                     0, 255, 0, 255, 0, 255);

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);
    
    // Working images
    frame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    redFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    blueFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    processingFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
}

void HeartWindowDetector::update()
{
    cam->getImage(frame);
    processImage(frame);
}

void HeartWindowDetector::processImage(Image* input, Image* output)
{
    frame->copyFrom(input);

    BlobDetector::Blob redBlob, blueBlob;


    // process the image to find the red square
    bool redFound = processColor(frame, redFrame, *m_redFilter, redBlob);
    if(redFound)
        publishFoundEvent(redBlob, Color::RED);
    else if(m_redFound)
        publishLostEvent(Color::RED);
    m_redFound = redFound;


    // process the image to find the blue square
    bool blueFound = processColor(frame, blueFrame, *m_blueFilter, blueBlob);
    if(blueFound)
        publishFoundEvent(blueBlob, Color::BLUE);
    else if(m_blueFound)
            publishLostEvent(Color::BLUE);
    m_blueFound = blueFound;


    // provide debugging output
    if(output)
    {
        if(m_debug == 0)
            output->copyFrom(frame);
        else
        {
            output->copyFrom(frame);

            // Color in the targets (as we find them)
            unsigned char* data = output->getData();
            unsigned char* redPtr = redFrame->getData();
            unsigned char* bluePtr = blueFrame->getData();
            unsigned char* end = output->getData() +
                (output->getWidth() * output->getHeight() * 3);
            
            for (; data != end; data += 3) {

                if(*redPtr)
                {
                    data[0] = 0;
                    data[1] = 0;
                    data[2] = 255;
                } 
                
                if(*bluePtr) 
                {
                    data[0] = 255;
                    data[1] = 0;
                    data[2] = 0;
                }
                
                if(*redPtr && *bluePtr)
                {
                    data[0] = 255;
                    data[1] = 0;
                    data[2] = 255;
                }

                // Advance all of the pointers
                redPtr += 3;
                bluePtr += 3;
            }
            
            if(m_debug == 2)
            {
                if(redFound)
                {
                    CvPoint center;
                    center.x = redBlob.getCenterX();
                    center.y = redBlob.getCenterY();

                    redBlob.drawStats(output);

                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 0, 150), -1);

                }

                if(blueFound)
                {
                    CvPoint center;
                    center.x = blueBlob.getCenterX();
                    center.y = blueBlob.getCenterY();

                    blueBlob.drawStats(output);

                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(150, 0, 0), -1);
                }

            } // debug == 2
        } // debug != 0
    } // output
}


bool HeartWindowDetector::processColor(Image* input, Image* output,
                                  ColorFilter& filter,
                                  BlobDetector::Blob& outputBlob)
{
    // Set the pixel format for processing
    processingFrame->copyFrom(input);
    processingFrame->setPixelFormat(Image::PF_RGB_8);
    processingFrame->setPixelFormat(Image::PF_LCHUV_8);

    filter.filterImage(processingFrame, output);

    // Erode the image (only if necessary)
    IplImage* img = output->asIplImage();
    if (m_erodeIterations > 0) {
        cvErode(img, img, NULL, m_erodeIterations);
    }

    // Dilate the image (only if necessary)
    if (m_dilateIterations > 0) {
        cvDilate(img, img, NULL, m_dilateIterations);
    }
    
    m_blobDetector.processImage(output);
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Sanity check blob
        double pixelPercentage = blob.getFillPercentage();
        double aspect = blob.getTrueAspectRatio();

        if (aspect <= m_maxAspectRatio &&
            aspect >= m_minAspectRatio &&
            m_minHeight <= blob.getHeight() &&
            m_minWidth <= blob.getWidth() &&
            m_minPixelPercentage <= pixelPercentage &&
            m_maxPixelPercentage >= pixelPercentage)
        {
            outputBlob = blob;
            return true;
        }
    }

    return false;
}

void HeartWindowDetector::show(char* window)
{
    vision::Image::showImage(frame, "HeartWindowDetector");
}

IplImage* HeartWindowDetector::getAnalyzedImage()
{
    return frame->asIplImage();
}

void HeartWindowDetector::publishFoundEvent(const BlobDetector::Blob& blob,
                                       Color::ColorType color)
{
    WindowEventPtr event(new WindowEvent());
    event->color = color;

    Detector::imageToAICoordinates(frame,
                                   blob.getCenterX(),
                                   blob.getCenterY(),
                                   event->x,
                                   event->y);

    // Determine range
    event->range = 1.0;

    // Determine the squareness
    event->squareNess = blob.getAspectRatio();

    publish(EventType::WINDOW_FOUND, event);
}

void HeartWindowDetector::publishLostEvent(Color::ColorType color)
{
    WindowEventPtr event(new WindowEvent());
    event->color = color;
    publish(EventType::WINDOW_LOST, event);
}

} // namespace vision
} // namespace ram
