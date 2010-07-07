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
    m_lBlobDetector(config, eventHub),
    m_rBlobDetector(config, eventHub),
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
                         "Debug level", 2, &m_debug, 0, 2);

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

    propSet->addProperty(config, false, "erodeIterations",
                         "Number of times to erode the binary image",
                         0, &m_erodeIterations);

    propSet->addProperty(config, false, "dilateIterations",
                         "Number of times to dilate the binary image",
                         0, &m_dilateIterations);

    propSet->addProperty(config, false, "lostPoleThreshold",
                         "Threshold for deciding we cannot see one of the poles."
                         "Percentage relative to full center height.",
                         0.0, &m_poleThreshold, 0.0, 1.0);

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
                                 BlobDetector::Blob& leftBlob,
                                 BlobDetector::Blob& rightBlob,
                                 BlobDetector::Blob& outBlob)
{
    output->copyFrom(input);
    output->setPixelFormat(Image::PF_RGB_8);
    output->setPixelFormat(Image::PF_LCHUV_8);

    filter.filterImage(output);

    // Erode and dilate the image (only if necessary)
    if (m_erodeIterations > 0) {
        IplImage* img = output->asIplImage();
        cvErode(img, img, NULL, m_erodeIterations);
    }

    if (m_dilateIterations > 0) {
        IplImage* img = output->asIplImage();
        cvDilate(img, img, NULL, m_dilateIterations);
    }

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
        if (1.0/blob.getTrueAspectRatio() <= m_maxAspectRatio &&
            1.0/blob.getTrueAspectRatio() >= m_minAspectRatio &&
            m_minWidth < blob.getWidth() &&
            m_minHeight < blob.getHeight() &&
            percent > m_minPixelPercentage &&
            percent < m_maxPixelPercentage)
        {
            processSides(output, blob, leftBlob, rightBlob);
            outBlob = blob;
            return true;
        }
    }

    return false;
}

bool HedgeDetector::processSides(Image* input, 
                                 BlobDetector::Blob& fullBlob,
                                 BlobDetector::Blob& leftBlob,
                                 BlobDetector::Blob& rightBlob)
{
    int width = fullBlob.getWidth();
    int height = fullBlob.getHeight();

    int minX = fullBlob.getMinX(), maxX = fullBlob.getMaxX();
    int minY = fullBlob.getMinY(), maxY = fullBlob.getMaxY();

    // data storage for half hedge sub images
    unsigned char *lbuffer = new unsigned char[width * height * 3];
    unsigned char *rbuffer = new unsigned char[width * height * 3];

    // extract left and right halves of the hedge candidate
    Image *lFrame = Image::extractSubImage(
        input, lbuffer, minX, minY, minX + width/2, maxY);

    Image *rFrame = Image::extractSubImage(
        input, rbuffer, minX + width/2, minY, maxX, maxY);

    m_lBlobDetector.processImage(lFrame);
    m_rBlobDetector.processImage(rFrame);

    BlobDetector::BlobList lBlobs = m_lBlobDetector.getBlobs();
    BlobDetector::BlobList rBlobs = m_rBlobDetector.getBlobs();

    leftBlob = lBlobs[0];
    rightBlob = rBlobs[0];

    delete lFrame;
    delete rFrame;

    return true;
}

void HedgeDetector::processImage(Image* input, Image* output)
{
    frame->copyFrom(input);
    
    BlobDetector::Blob hedgeBlob, leftBlob, rightBlob;
    bool found = false;

    if((found = processColor(frame, greenFrame, *m_colorFilter,
                             leftBlob, rightBlob, hedgeBlob))) {
        publishFoundEvent(hedgeBlob, leftBlob, rightBlob);

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

                    // draw full rectangle and center point
                    cvRectangle(output->asIplImage(),
                                cvPoint(hedgeBlob.getMinX(), hedgeBlob.getMinY()),
                                cvPoint(hedgeBlob.getMaxX(), hedgeBlob.getMaxY()),
                                cvScalar(255,255,255), 2);

                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 255, 0), -1);

                    // draw left rectangle and center point
                    cvRectangle(output->asIplImage(),
                                cvPoint(hedgeBlob.getMinX() + leftBlob.getMinX(),
                                        hedgeBlob.getMinY() + leftBlob.getMinY()),
                                cvPoint(hedgeBlob.getMinX() + leftBlob.getMaxX(),
                                        hedgeBlob.getMinY() + leftBlob.getMaxY()),
                                cvScalar(150,150,150), 2);
                    
                    CvPoint lCenter;
                    lCenter.x = hedgeBlob.getMinX() + 
                        (leftBlob.getMaxX() - leftBlob.getMinX()) / 2 +
                        leftBlob.getMinX();
                    lCenter.y = hedgeBlob.getMinY() + 
                        (leftBlob.getMaxY() - leftBlob.getMinY()) / 2 +
                        leftBlob.getMinY();
                    
                    cvCircle(output->asIplImage(),
                             lCenter,
                             3, cvScalar(0,0,0), -1);

                    // draw right rectangle and center point
                    cvRectangle(output->asIplImage(),
                                cvPoint(hedgeBlob.getMinX() + rightBlob.getMinX(),
                                        hedgeBlob.getMinY() + rightBlob.getMinY()),
                                cvPoint(hedgeBlob.getMinX() + hedgeBlob.getWidth() / 2 + 
                                        rightBlob.getMaxX(),
                                        hedgeBlob.getMinY() + rightBlob.getMaxY()),
                                cvScalar(150,150,150), 2);

                    CvPoint rCenter;
                    rCenter.x = hedgeBlob.getMinX() + hedgeBlob.getWidth() / 2 + 
                        (rightBlob.getMaxX() - rightBlob.getMinX()) / 2 +
                        rightBlob.getMinX();
                    rCenter.y = hedgeBlob.getMinY() + 
                        (rightBlob.getMaxY() - rightBlob.getMinY()) / 2 +
                        rightBlob.getMinY();

                    cvCircle(output->asIplImage(),
                             rCenter,
                             3, cvScalar(0,0,0), -1);

                }
            }
        }
    }
}

void HedgeDetector::publishFoundEvent(BlobDetector::Blob& blob,
                                      BlobDetector::Blob& leftBlob,
                                      BlobDetector::Blob& rightBlob)
{
    HedgeEventPtr event(new HedgeEvent());

    int imFullY = (blob.getMaxY() - blob.getMinY()) / 2 
        + blob.getMinY();
 

    int imLeftX = blob.getMinX() + (leftBlob.getMaxX() - leftBlob.getMinX()) / 2 
        + leftBlob.getMinX();
    int imLeftY = blob.getMinY() + (leftBlob.getMaxY() - leftBlob.getMinY()) / 2 
        + leftBlob.getMinY();


    int imRightX = blob.getMinX() + blob.getWidth() / 2 + 
        (rightBlob.getMaxX() - rightBlob.getMinX()) / 2 
        + rightBlob.getMinX(); 
    int imRightY = blob.getMinY() +  (rightBlob.getMaxY() - rightBlob.getMinY()) / 2 
        + rightBlob.getMinY();

    double minPoleY = m_poleThreshold * imFullY;

    bool haveLeft = minPoleY < imLeftY;
    bool haveRight = minPoleY < imRightY;
    
    double leftX, leftY, rightX, rightY;
    Detector::imageToAICoordinates(frame, imLeftX, imLeftY, leftX, leftY);

    Detector::imageToAICoordinates(frame, imRightX, imRightY, rightX, rightY);

    event->leftX = leftX;
    event->leftY = leftY;
    event->rightX = rightX;
    event->rightY = rightY;
    event->haveLeft = haveLeft;
    event->haveRight = haveRight;
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
