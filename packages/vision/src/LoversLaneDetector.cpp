/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/LoversLaneDetector.cpp
 */

// STD Includes
#include <algorithm>
#include <vector>
#include <iostream>

// Library Includes
#include "highgui.h"
#include <boost/foreach.hpp>
#include <log4cpp/Category.hh>
#include <boost/bind.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/LoversLaneDetector.h"
#include "vision/include/Events.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/ImageFilter.h"
#include "vision/include/VisionSystem.h"
#include "core/include/PropertySet.h"


static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("LoversLaneDetector"));

namespace ram {
namespace vision {

LoversLaneDetector::LoversLaneDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_colorFilter(0),
    m_blobDetector(config, eventHub),
    m_lBlobDetector(config, eventHub),
    m_rBlobDetector(config, eventHub),
    frame(0),
    greenFrame(0),
    m_physicalHeightMeters(1.2),
    m_colorFilterLookupTable(false),
    m_lookupTablePath("")
    
{
    init(config);
}

LoversLaneDetector::LoversLaneDetector(Camera* camera) :
    cam(0),
    m_colorFilter(0),
    m_physicalHeightMeters(1.2),
    m_colorFilterLookupTable(false),
    m_lookupTablePath("")
{
    init(core::ConfigNode::fromString("{}"));
}

void LoversLaneDetector::init(core::ConfigNode config)
{
    LOGGER.info("Not IMPLEMENTED YET");
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

    propSet->addProperty(config, false, "openIterations",
                         "Number of times to apply the open morphological operation",
                         0, &m_openIterations);

    propSet->addProperty(config, false, "closeIterations",
                         "Number of times to apply the close morphological operation",
                         0, &m_closeIterations);

    propSet->addProperty(config, false, "lostPoleThreshold",
                         "Threshold for deciding we cannot see one of the poles."
                         "Percentage relative to full center height.",
                         0.0, &m_poleThreshold, 0.0, 1.0);

    propSet->addProperty(config, false, "ColorFilterLookupTable",
                         "True uses color filter lookup table", false,
                         boost::bind(&LoversLaneDetector::getLookupTable, this),
                         boost::bind(&LoversLaneDetector::setLookupTable, this, _1));
    
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

LoversLaneDetector::~LoversLaneDetector()
{
    delete frame;
    delete greenFrame;

    if ( m_colorFilterLookupTable ) 
        delete m_tableColorFilter;

    delete m_colorFilter;
}

void LoversLaneDetector::show(char* window)
{
    cvShowImage(window,((IplImage*)(frame)));
}

void LoversLaneDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}

bool LoversLaneDetector::getLookupTable()
{
    return m_colorFilterLookupTable;
}

void LoversLaneDetector::setLookupTable(bool lookupTable)
{
    if ( lookupTable ) {
        m_colorFilterLookupTable = true;

        m_lookupTablePath = // CHANGE THIS TO LOVERS LANE DEFAULT
            "/home/steven/ImageFilter/LookupTables/doubleGreenBuoyBlend1.5.serial";
        m_tableColorFilter = new TableColorFilter(m_lookupTablePath);
        
    } else {
        m_colorFilterLookupTable = false;
    }
}

bool LoversLaneDetector::processColor(Image* input, Image* output,
                                      ImageFilter& filter,
                                      BlobDetector::Blob& leftBlob,
                                      BlobDetector::Blob& rightBlob,
                                      BlobDetector::Blob& outBlob)
{
    output->copyFrom(input);
    output->setPixelFormat(Image::PF_RGB_8);
    output->setPixelFormat(Image::PF_LCHUV_8);

    if ( m_colorFilterLookupTable )
        filter.filterImage(input, output);
    else
        filter.filterImage(output);

    // Open the image if requested
    if (m_openIterations > 0)
    {
        IplImage *img = output->asIplImage();
        cvErode(img, img, NULL, m_openIterations);
        cvDilate(img, img, NULL, m_openIterations);
    }

    // Close the image if requested
    if (m_closeIterations > 0)
    {
        IplImage *img = output->asIplImage();
        cvDilate(img, img, NULL, m_closeIterations);
        cvErode(img, img, NULL, m_closeIterations);
    }

    // Erode and dilate the image (only if necessary)
    if (m_dilateIterations > 0) {
        IplImage* img = output->asIplImage();
        cvDilate(img, img, NULL, m_dilateIterations);
    }

    if (m_erodeIterations > 0) {
        IplImage* img = output->asIplImage();
        cvErode(img, img, NULL, m_erodeIterations);
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

bool LoversLaneDetector::processSides(Image* input, 
                                      BlobDetector::Blob& fullBlob,
                                      BlobDetector::Blob& leftBlob,
                                      BlobDetector::Blob& rightBlob)
{
    int width = fullBlob.getWidth();
    int height = fullBlob.getHeight();

    int minX = fullBlob.getMinX(), maxX = fullBlob.getMaxX();
    int minY = fullBlob.getMinY(), maxY = fullBlob.getMaxY();

    // data storage for half LoversLane sub images
    unsigned char *lbuffer = new unsigned char[width * height * 3];
    unsigned char *rbuffer = new unsigned char[width * height * 3];

    // extract left and right halves of the LoversLane candidate
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


    delete [] lbuffer;
    delete [] rbuffer;
    delete lFrame;
    delete rFrame;

    return true;
}

void LoversLaneDetector::processImage(Image* input, Image* output)
{
    frame->copyFrom(input);
    
    BlobDetector::Blob loversLaneBlob, leftBlob, rightBlob;
    bool found = false;
    
    if ( m_colorFilterLookupTable ) {
        found = processColor(frame, greenFrame, *m_tableColorFilter,
                             leftBlob, rightBlob, loversLaneBlob);
    } else {
        found = processColor(frame, greenFrame, *m_colorFilter,
                             leftBlob, rightBlob, loversLaneBlob);
    }

    if(found) {
        publishFoundEvent(loversLaneBlob, leftBlob, rightBlob);
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
            
            // Color in the LoversLane
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
                    center.x = (loversLaneBlob.getMaxX() - loversLaneBlob.getMinX()) / 2 +
                        loversLaneBlob.getMinX();
                    center.y = (loversLaneBlob.getMaxY() - loversLaneBlob.getMinY()) / 2 +
                        loversLaneBlob.getMinY();

                    loversLaneBlob.drawStats(output);

                    // draw full rectangle and center point
                    cvRectangle(output->asIplImage(),
                                cvPoint(loversLaneBlob.getMinX(), loversLaneBlob.getMinY()),
                                cvPoint(loversLaneBlob.getMaxX(), loversLaneBlob.getMaxY()),
                                cvScalar(255,255,255), 2);

                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 255, 0), -1);

                    // draw left rectangle and center point
                    cvRectangle(output->asIplImage(),
                                cvPoint(loversLaneBlob.getMinX() + leftBlob.getMinX(),
                                        loversLaneBlob.getMinY() + leftBlob.getMinY()),
                                cvPoint(loversLaneBlob.getMinX() + leftBlob.getMaxX(),
                                        loversLaneBlob.getMinY() + leftBlob.getMaxY()),
                                cvScalar(150,150,150), 2);
                    
                    CvPoint lCenter;
                    lCenter.x = loversLaneBlob.getMinX() + 
                        (leftBlob.getMaxX() - leftBlob.getMinX()) / 2 +
                        leftBlob.getMinX();
                    lCenter.y = loversLaneBlob.getMinY() + 
                        (leftBlob.getMaxY() - leftBlob.getMinY()) / 2 +
                        leftBlob.getMinY();
                    
                    cvCircle(output->asIplImage(),
                             lCenter,
                             3, cvScalar(0,0,0), -1);

                    // draw right rectangle and center point
                    cvRectangle(output->asIplImage(),
                                cvPoint(loversLaneBlob.getMinX() + loversLaneBlob.getWidth() / 2 +
                                        rightBlob.getMinX(),
                                        loversLaneBlob.getMinY() + rightBlob.getMinY()),
                                cvPoint(loversLaneBlob.getMinX() + loversLaneBlob.getWidth() / 2 + 
                                        rightBlob.getMaxX(),
                                        loversLaneBlob.getMinY() + rightBlob.getMaxY()),
                                cvScalar(150,150,150), 2);

                    CvPoint rCenter;
                    rCenter.x = loversLaneBlob.getMinX() + loversLaneBlob.getWidth() / 2 + 
                        (rightBlob.getMaxX() - rightBlob.getMinX()) / 2 +
                        rightBlob.getMinX();
                    rCenter.y = loversLaneBlob.getMinY() + 
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

void LoversLaneDetector::publishFoundEvent(BlobDetector::Blob& blob,
                                           BlobDetector::Blob& leftBlob,
                                           BlobDetector::Blob& rightBlob)
{
    LoversLaneEventPtr event(new LoversLaneEvent());

    int imFullY = (blob.getMaxY() - blob.getMinY()) / 2 
        + blob.getMinY();
    int imFullX = (blob.getMaxX() - blob.getMinX()) / 2 + blob.getMinX();
    

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

    if (haveLeft && !haveRight)
        imRightY = imLeftY;

    if (haveRight && !haveLeft)
        imLeftY = imRightY;
    
    double leftX, leftY, rightX, rightY;
    Detector::imageToAICoordinates(frame, imLeftX, imLeftY, leftX, leftY);

    Detector::imageToAICoordinates(frame, imRightX, imRightY, rightX, rightY);

//    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();

    double blobHeight = blob.getHeight();
    double fracHeight = blobHeight / yPixelHeight;
    double range = m_physicalHeightMeters / (2 * std::tan(yFOV.valueRadians() * fracHeight / 2));

    event->leftX = leftX;
    event->leftY = leftY;
    event->rightX = rightX;
    event->rightY = rightY;
    event->centerX = imFullX;
    event->centerY = imFullY;
    event->haveLeft = haveLeft;
    event->haveRight = haveRight;
    event->squareNess = 1.0 / blob.getTrueAspectRatio();
    event->range = range;

    publish(EventType::LOVERSLANE_FOUND, event);

}

void LoversLaneDetector::publishLostEvent()
{
    LoversLaneEventPtr event(new LoversLaneEvent());    
    publish(EventType::LOVERSLANE_LOST, event);
}

IplImage* LoversLaneDetector::getAnalyzedImage()
{
    return greenFrame->asIplImage();
}


} // namespace vision
} // namespace ram
