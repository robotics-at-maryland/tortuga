/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/HedgeDetector.cpp
 */

// STD Includes
#include <algorithm>
#include <vector>
#include <iostream>

// Library Includes
#include "highgui.h"
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/HedgeDetector.h"
#include "vision/include/Events.h"
#include "vision/include/ColorFilter.h"

#include "core/include/PropertySet.h"

namespace ram {
namespace vision {

HedgeDetector::HedgeDetector(core::ConfigNode config,
                             core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_colorFilter(0),
    m_blobDetector(config, eventHub),
    frame(0),
    greenFrame(0)
    
{
    init(config);
}

HedgeDetector::HedgeDetector(Camera* camera) :
    cam(0),
    m_colorFilter(0)
{
    init(core::ConfigNode::fromString("{}"));
}

void HedgeDetector::init(core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());


    propSet->addProperty(config, false, "debug",
                         "Debug level", 2, &m_debug, 0, 3);

    propSet->addProperty(config, false, "maxAspectRatio",
                         "Maximum aspect ratio (width/height)",
                         2.5, &m_maxAspectRatio);

    propSet->addProperty(config, false, "minAspectRatio",
                         "Minimum aspect ratio (width/height)",
                         0.75, &m_minAspectRatio);

    propSet->addProperty(config, false, "minWidth",
                         "Minimum width for a blob",
                         50, &m_minWidth);

    propSet->addProperty(config, false, "minHeight",
                         "Minimum height for a blob",
                         50, &m_minHeight);

    propSet->addProperty(config, false, "minPixelPercentage",
                         "Minimum percentage of pixels / area",
                         0.1, &m_minPixelPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "maxPixelPercentage",
                         "Maximum percentage of pixels / area",
                         1.0, &m_maxPixelPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "maxDistance",
                         "Maximum distance between two blobs from different frames",
                         15.0, &m_maxDistance);

    m_colorFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_colorFilter->addPropertiesToSet(propSet, &config,
                                    "L", "Luminance",
                                    "C", "Chrominance",
                                    "H", "Hue",
                                    0, 255, 0, 255, 0, 255);

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);


    // Working images
    frame = new vision::OpenCVImage(640, 480, Image::PF_BGR_8);
    greenFrame = new vision::OpenCVImage(640, 480, Image::PF_BGR_8);

}

HedgeDetector::~HedgeDetector()
{
    delete frame;
    delete greenFrame;

    delete m_colorFilter;
}

void HedgeDetector::show(char* window)
{
    cvShowImage(window,((IplImage*)(frame)));
}

void HedgeDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}


bool HedgeDetector::processColor(Image* input, Image* output,
                                  ColorFilter& filter,
                                  BlobDetector::Blob& outBlob)
{
    output->copyFrom(input);
    output->setPixelFormat(Image::PF_RGB_8);
    output->setPixelFormat(Image::PF_LCHUV_8);


    if(m_debug == 3) {
        OpenCVImage debug1(640, 480, Image::PF_GRAY_8);
        OpenCVImage debug2(640, 480, Image::PF_GRAY_8);
        OpenCVImage debug3(640, 480, Image::PF_GRAY_8);
        unsigned char* lchData = (unsigned char *) output->getData();
        unsigned char* debug1Data = (unsigned char *) debug1.getData();
        unsigned char* debug2Data = (unsigned char *) debug2.getData();
        unsigned char* debug3Data = (unsigned char *) debug3.getData();

        for(int i=0; i<640*480; i++)
        {
            *debug1Data = lchData[0];
            debug1Data += 1;
            lchData += 3;
        }
        Image::showImage(&debug1);

        lchData = (unsigned char *) output->getData();

        for(int i=0; i<640*480; i++)
        {
            *debug2Data = lchData[1];
            debug2Data += 1;
            lchData += 3;
        }
        Image::showImage(&debug2);

        lchData = (unsigned char *) output->getData();

        for(int i=0; i<640*480; i++)
        {
            *debug3Data = lchData[2];
            debug3Data += 1;
            lchData += 3;
        }
        Image::showImage(&debug3);
    }


    filter.filterImage(output);

    OpenCVImage debug(output->getWidth(), output->getHeight(),
                              Image::PF_BGR_8);
    m_blobDetector.processImage(output, &debug);
    //Image::showImage(&debug);
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Sanity check blob
        double percent = (double) blob.getSize() /
            (blob.getHeight() * blob.getWidth());
        if (blob.getTrueAspectRatio() <= m_maxAspectRatio &&
            blob.getTrueAspectRatio() >= m_minAspectRatio &&
            m_minWidth < blob.getWidth() &&
            m_minHeight < blob.getHeight() &&
            percent > m_minPixelPercentage &&
            percent < m_maxPixelPercentage)
        {
            outBlob = blob;
            return true;
        }
    }

    return false;
}

void HedgeDetector::processImage(Image* input, Image* output)
{
    frame->copyFrom(input);
    
    BlobDetector::Blob hedgeBlob;
    bool found = false;

    if((found = processColor(frame, greenFrame, *m_colorFilter, hedgeBlob))) {
        publishFoundEvent(hedgeBlob);
    } else {
        // Publish lost event if this was found previously
        if(m_found) {
            publishLostEvent();
        }
    }
    m_found = found;

    if(output)
    {
        if(m_debug == 0) {
            output->copyFrom(frame);
        } else {
            output->copyFrom(frame);
            
            // Color in the hedge
            unsigned char* data = output->getData();
            unsigned char* greenPtr = greenFrame->getData();
            unsigned char* end = output->getData() +
                (output->getWidth() * output->getHeight() * 3);
            
            for (; data != end; data += 3) {
                if (*greenPtr) {
                    data[0] = 0;
                    data[1] = 255;
                    data[2] = 0;
                }
                greenPtr += 3;
            }

            if(m_debug == 2) {

                if(found){
                    CvPoint center;
                    center.x = (hedgeBlob.getMaxX() - hedgeBlob.getMinX()) / 2 +
                        hedgeBlob.getMinX();
                    center.y = (hedgeBlob.getMaxY() - hedgeBlob.getMinY()) / 2 +
                        hedgeBlob.getMinY();
                    
                    hedgeBlob.drawStats(output);

                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 255, 0), -1);
                }
            }
        }
    }
}

void HedgeDetector::publishFoundEvent(BlobDetector::Blob& blob)
{
    HedgeEventPtr event(new HedgeEvent());

    int imageX = (blob.getMaxX() - blob.getMinX()) / 2 + blob.getMinX();
    int imageY = (blob.getMaxY() - blob.getMinY()) / 2 + blob.getMinY();
    
    double centerX, centerY;
    Detector::imageToAICoordinates(frame, imageX, imageY,
                                   centerX, centerY);

    event->x = centerX;
    event->y = centerY;
    event->squareNess = 1.0 / blob.getTrueAspectRatio();
    event->range = 1.0 - (((double)blob.getHeight()) /
                          ((double)frame->getHeight()));

    publish(EventType::HEDGE_FOUND, event);

}

void HedgeDetector::publishLostEvent()
{
    HedgeEventPtr event(new HedgeEvent());    
    publish(EventType::HEDGE_LOST, event);
}

IplImage* HedgeDetector::getAnalyzedImage()
{
    return greenFrame->asIplImage();
}


} // namespace vision
} // namespace ram
