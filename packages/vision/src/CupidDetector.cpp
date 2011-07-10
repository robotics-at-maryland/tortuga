/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/src/CupidDetector.cpp
 */

// STD Includes
#include <assert.h>
#include <math.h>
#include <set>
#include <iostream>

// Library Includes
#include <boost/foreach.hpp>
#include "cv.h"
#include <log4cpp/Category.hh>

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
#include "vision/include/CupidDetector.h"
#include "vision/include/VisionSystem.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("CupidDetector"));

namespace ram {
namespace vision {

CupidDetector::CupidDetector(core::ConfigNode config,
                                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_redFilter(0),
    m_blueFilter(0),
    m_blobDetector(config, eventHub),
    m_physicalWidthMeters(0.61),
    m_physicalHeightMeters(0.61)
{
    init(config);
}

CupidDetector::CupidDetector(Camera* camera) :
    cam(camera),
    m_redFilter(0),
    m_blueFilter(0),
    m_physicalWidthMeters(0.61),
    m_physicalHeightMeters(0.61)
{
    init(core::ConfigNode::fromString("{}"));
}

CupidDetector::~CupidDetector()
{
    delete m_redFilter;
    delete m_blueFilter;

    delete m_frame;
    delete m_redFrame;
    delete m_blueFrame;
    delete m_processingFrame;
}

void CupidDetector::init(core::ConfigNode config)
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
    m_frame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    m_redFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    m_blueFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    m_processingFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    m_heartsFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);

    LOGGER.info("Found CentroidX CentroidY RangeW RangeH Range Width Height blobPixels PixelPercentage");
}

void CupidDetector::update()
{
    cam->getImage(m_frame);
    processImage(m_frame);
}

void CupidDetector::processImage(Image* input, Image* output)
{
    input->setPixelFormat(Image::PF_BGR_8);
    m_frame->copyFrom(input);

    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

    BlobDetector::Blob redBlob, blueBlob, smallHeartBlob, largeHeartBlob;

    // process the image to find the red square
    bool redFound = processColor(m_frame, m_redFrame, *m_redFilter, redBlob);
    bool heartsFound = false;
    if(redFound)
    {
        findHearts(m_frame, *m_redFilter, redBlob,
                   smallHeartBlob, largeHeartBlob);
        if(heartsFound)
        {
            publishFoundEvent(redBlob, Color::RED);

            int blobPixels = redBlob.getSize();
            double blobWidth = redBlob.getWidth();
            double blobHeight = redBlob.getHeight();

            double fracWidth = blobWidth / xPixelWidth;
            double fracHeight = blobHeight / yPixelHeight;

            double rangeW = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
            double rangeH = m_physicalHeightMeters / (2 * std::tan(yFOV.valueRadians() * fracHeight / 2));
            double range = (rangeW + rangeH) / 2;

            int width = redBlob.getWidth();
            int height = redBlob.getHeight();

            LOGGER.infoStream() << "1" << " "
                                << redBlob.getTrueCenterX() << " "
                                << redBlob.getTrueCenterY() << " "
                                << rangeW << " "
                                << rangeH << " "
                                << range << " "
                                << width << " "
                                << height << " "
                                << blobPixels << " "
                                << blobPixels / (width * height);
        }
    }
    else if(m_redFound)
    {
        publishLostEvent(Color::RED);
        LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
    }
    m_redFound = redFound;

    bool blueFound = false;
    if(!redFound)
    {
        // process the image to find the blue square
        blueFound = processColor(m_frame, m_blueFrame, *m_blueFilter, blueBlob);
        if(blueFound)
        {
            heartsFound = findHearts(m_frame, *m_blueFilter, blueBlob,
                                     smallHeartBlob, largeHeartBlob);
            if(heartsFound)
            {
                publishFoundEvent(blueBlob, Color::BLUE);

                int blobPixels = blueBlob.getSize();
                double blobWidth = blueBlob.getWidth();
                double blobHeight = blueBlob.getHeight();

                double fracWidth = blobWidth / xPixelWidth;
                double fracHeight = blobHeight / yPixelHeight;

                double rangeW = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
                double rangeH = m_physicalHeightMeters / (2 * std::tan(yFOV.valueRadians() * fracHeight / 2));
                double range = (rangeW + rangeH) / 2;

                int width = blueBlob.getWidth();
                int height = blueBlob.getHeight();

                LOGGER.infoStream() << "1" << " "
                                    << blueBlob.getTrueCenterX() << " "
                                    << blueBlob.getTrueCenterY() << " "
                                    << range << " "
                                    << width << " "
                                    << height << " "
                                    << blobPixels << " "
                                    << blobPixels / (width * height);
            }
        }
        else if(m_blueFound)
        {
            publishLostEvent(Color::BLUE);
            LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                                << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
        }
        m_blueFound = blueFound;
    }


    // provide debugging output
    if(output)
    {
        output->copyFrom(m_frame);
        if(m_debug > 0)
        {
            Image::fillMask(output, m_redFrame, 200, 0, 0);
            Image::fillMask(output, m_blueFrame, 0, 0, 200);
            
            if(m_debug == 2)
            {
                if(redFound)
                    drawDebugCircle(redBlob, output, 200, 0, 0);

                if(blueFound)
                    drawDebugCircle(blueBlob, output, 0, 0, 200);
                
                if(heartsFound)
                {
                    BlobDetector::Blob debugBlob;
                    if(blueFound)
                        debugBlob = blueBlob;
                    else
                        debugBlob = redBlob;

                    CvPoint smallP1;
                    smallP1.x = debugBlob.getMinX() + smallHeartBlob.getMinX();
                    smallP1.y = debugBlob.getMinY() + smallHeartBlob.getMinY();

                    CvPoint smallP2;
                    smallP2.x = debugBlob.getMinX() + smallHeartBlob.getMaxX();
                    smallP2.y = debugBlob.getMinY() + smallHeartBlob.getMaxY();

                    CvPoint smallCenter;
                    smallCenter.x = (smallP1.x + smallP2.x) / 2;
                    smallCenter.y = (smallP1.y + smallP2.y) / 2;

                    cvRectangle(output->asIplImage(), smallP1, smallP2,
                                cvScalar(100,100,100),3);
                    cvCircle(output->asIplImage(), smallCenter, 3,
                             cvScalar(255, 255, 255));

                    CvPoint largeP1;
                    largeP1.x = debugBlob.getMinX() + largeHeartBlob.getMinX();
                    largeP1.y = debugBlob.getMinY() + largeHeartBlob.getMinY();

                    CvPoint largeP2;
                    largeP2.x = debugBlob.getMinX() + largeHeartBlob.getMaxX();
                    largeP2.y = debugBlob.getMinY() + largeHeartBlob.getMaxY();

                    CvPoint largeCenter;
                    largeCenter.x = (largeP1.x + largeP2.x) / 2;
                    largeCenter.y = (largeP1.y + largeP2.y) / 2;

                    cvRectangle(output->asIplImage(), largeP1, largeP2,
                                cvScalar(100,100,100),3);
                    cvCircle(output->asIplImage(), largeCenter, 3,
                             cvScalar(255, 255, 255));
                }
            } // debug == 2
        } // debug > 0
    } // output
}

bool CupidDetector::processColor(Image* input, Image* output,
                                 ColorFilter& filter,
                                 BlobDetector::Blob& outputBlob)
{
    // Set the pixel format for processing
    m_processingFrame->copyFrom(input);
    m_processingFrame->setPixelFormat(Image::PF_RGB_8);
    m_processingFrame->setPixelFormat(Image::PF_LCHUV_8);

    filter.filterImage(m_processingFrame, output);


    IplImage* img = output->asIplImage();
    // Dilate the image (only if necessary)
    if (m_dilateIterations > 0) 
    {
        cvDilate(img, img, NULL, m_dilateIterations);
    }

    // Erode the image (only if necessary)
    if (m_erodeIterations > 0) 
    {
        cvErode(img, img, NULL, m_erodeIterations);
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
    
bool CupidDetector::findHearts(Image *input,
                               ColorFilter& filter,
                               BlobDetector::Blob& windowBlob,
                               BlobDetector::Blob& smallHeart,
                               BlobDetector::Blob& largeHeart)
{
    m_heartsFrame->copyFrom(input);
    m_heartsFrame->setPixelFormat(Image::PF_RGB_8);
    m_heartsFrame->setPixelFormat(Image::PF_LCHUV_8);

    unsigned char *buffer = new unsigned char[windowBlob.getWidth()*windowBlob.getHeight()*3];
    Image *innerFrame = Image::extractSubImage(
        m_heartsFrame, buffer,
        windowBlob.getMinX(), windowBlob.getMinY(),
        windowBlob.getMaxX(), windowBlob.getMaxY());

    filter.inverseFilterImage(innerFrame);

    m_blobDetector.processImage(innerFrame);
    delete innerFrame;
    delete [] buffer;

    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();
    
    // for now assume the largest blob is the big heart and the
    // second largest blob is the small heart.  if this is not
    // the case we probably are screwed anyway.  if we don't have
    // two blobs, we arent looking at the cupid so we should return
    // false.
    bool foundHearts = true;
    if(blobs.size() >= 2)
    {
        largeHeart = blobs[0];
        smallHeart = blobs[1];
    }
    else
    {
        foundHearts = false;
    }
    return foundHearts;
}


void CupidDetector::show(char* window)
{
    vision::Image::showImage(m_frame, "CupidDetector");
}

void CupidDetector::drawDebugCircle(BlobDetector::Blob blob,
                                    Image* output,
                                    unsigned char R,
                                    unsigned char G,
                                    unsigned char B)
{
    CvPoint center;
    center.x = blob.getTrueCenterX();
    center.y = blob.getTrueCenterY();
    
    blob.drawStats(output);

    cvCircle(output->asIplImage(), center, 6, cvScalar(B, G, R), -1);
}

IplImage* CupidDetector::getAnalyzedImage()
{
    return m_frame->asIplImage();
}

void CupidDetector::publishFoundEvent(const BlobDetector::Blob& blob,
                                            Color::ColorType color)
{
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

    CupidEventPtr event(new CupidEvent());

    double centerX, centerY;
    Detector::imageToAICoordinates(m_frame, blob.getTrueCenterX(), blob.getTrueCenterY(),
                                   centerX, centerY);
    
    double fracWidth = static_cast<double>(blob.getWidth()) / xPixelWidth;
    double fracHeight = static_cast<double>(blob.getHeight()) / yPixelHeight;

    double rangeW = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
    double rangeH = m_physicalHeightMeters / (2 * std::tan(yFOV.valueRadians() * fracHeight / 2));
    double range = (rangeW + rangeH) / 2;

    int minX = blob.getMinX();
    int maxX = blob.getMaxX();
    int minY = blob.getMinY();
    int maxY = blob.getMaxY();

    bool touchingEdge = false;
    if(minX == 0 || minY == 0 || maxX == xPixelWidth || maxY == yPixelHeight)
        touchingEdge = true;

    // Determine range
    event->x = centerX;
    event->y = centerY;
    event->range = range;
    event->color = color;
    event->touchingEdge = touchingEdge;

    publish(EventType::CUPID_FOUND, event);
    publish(EventType::CUPID_SMALL_FOUND, event);
    publish(EventType::CUPID_LARGE_FOUND, event);
}

void CupidDetector::publishLostEvent(Color::ColorType color)
{
    WindowEventPtr event(new WindowEvent());
    event->color = color;
    publish(EventType::WINDOW_LOST, event);
}

} // namespace vision
} // namespace ram
