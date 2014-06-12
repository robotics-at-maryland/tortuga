/*
 * Copyright (C) 2012 Robotics at Maryland
 * Copyright (C) 2012 Gary Sullivan
 * All rights reserved.
 *
 * Author: Gary Sullivan <gsulliva@umd.edu>
 * File:  packages/vision/src/CaesarDetector.cpp
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
#include <boost/bind.hpp>

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
#include "vision/include/ImageFilter.h"
#include "vision/include/CaesarDetector.h"
#include "vision/include/VisionSystem.h"
#include "vision/include/TableColorFilter.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("CaesarDetector"));

namespace ram {
namespace vision {

CaesarDetector::CaesarDetector(core::ConfigNode config,
                                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_redFilter(0),
    m_blueFilter(0),
    m_blobDetector(config, eventHub),
    m_frame(0),
    m_redFrame(0),
    m_blueFrame(0),
    m_processingFrame(0),
    m_redFound(0),
    m_blueFound(0),
    m_physicalWidthMeters(0.61),
    m_physicalHeightMeters(0.61),
    m_colorFilterLookupTable(false),
    m_redLookupTablePath(""),
    m_blueLookupTablePath("")
{
    init(config);
}

CaesarDetector::CaesarDetector(Camera* camera) :
    cam(camera),
    m_redFilter(0),
    m_blueFilter(0),
    m_blobDetector(),
    m_frame(0),
    m_redFrame(0),
    m_blueFrame(0),
    m_processingFrame(0),
    m_redFound(0),
    m_blueFound(0),
    m_physicalWidthMeters(0.61),
    m_physicalHeightMeters(0.61)
{
    init(core::ConfigNode::fromString("{}"));
}

CaesarDetector::~CaesarDetector()
{
    delete m_redFilter;
    delete m_blueFilter;

    if ( m_colorFilterLookupTable )
    {
        delete m_redTableColorFilter;
        delete m_blueTableColorFilter;
    }
    delete m_frame;
    delete m_redFrame;
    delete m_blueFrame;
    delete m_processingFrame;
}

void CaesarDetector::init(core::ConfigNode config)
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

    propSet->addProperty(config, false, "minHoleAspectRatio",
                         "Minimum aspect ratio for the holes",
                         0.8, &m_minHoleAspectRatio);

    propSet->addProperty(config, false, "maxHoleAspectRatio",
                         "Maximum aspect ratio for the holes",
                         1.2, &m_maxHoleAspectRatio);

    propSet->addProperty(config, false, "minHolePixelPct",
                         "Minimum pixel percentage of hole",
                         0.4, &m_minHolePixelPct);

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

    propSet->addProperty(config, false, "ColorFilterLookupTable",
                         "True uses color filter lookup table", false,
                         boost::bind(&CaesarDetector::getLookupTable, this),
                         boost::bind(&CaesarDetector::setLookupTable, this, _1));
    
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
    m_holesFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);

    LOGGER.info("Found CentroidX CentroidY RangeW RangeH Range Width Height blobPixels PixelPercentage");
}

void CaesarDetector::update()
{
    cam->getImage(m_frame);
    processImage(m_frame);
}

bool CaesarDetector::getLookupTable()
{
    return m_colorFilterLookupTable;
}

void CaesarDetector::setLookupTable(bool lookupTable)
{
    if ( lookupTable ) {
        m_colorFilterLookupTable = true;

        // Initializing ColorFilterTable
        m_redLookupTablePath = 
            "/home/steven/ImageFilter/LookupTables/doubleRedBuoyBlend1.5.serial";
        m_redTableColorFilter = new TableColorFilter(m_redLookupTablePath);

        m_blueLookupTablePath = 
            "/home/steven/ImageFilter/LookupTables/doubleYellowBuoyBlend1.5.serial";
        m_blueTableColorFilter = new TableColorFilter(m_blueLookupTablePath);
    } else {
        m_colorFilterLookupTable = false;
    }
}

void CaesarDetector::processImage(Image* input, Image* output)
{
    input->setPixelFormat(Image::PF_BGR_8);
    m_frame->copyFrom(input);

    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

    BlobDetector::Blob redBlob, blueBlob, smallHoleBlob, largeHoleBlob;
    m_blobDetector.setMinimumBlobSize(100);

    // process the image to find the red square
    bool redFound;
    bool redHolesFound = false;

    if ( m_colorFilterLookupTable )
        redFound = processColor(m_frame, m_redFrame, *m_redTableColorFilter, redBlob);
    else
        redFound = processColor(m_frame, m_redFrame, *m_redFilter, redBlob);

    if(redFound)
    {
        redHolesFound = findHoles(m_frame, *m_redFilter, redBlob,
                                    smallHoleBlob, largeHoleBlob);
        if(redHolesFound)
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

    // process the image to find the blue square
    bool blueFound = false;
    bool blueHolesFound = false;

    if ( m_colorFilterLookupTable ) {
        blueFound = processColor(m_frame, m_blueFrame, 
                                 *m_blueTableColorFilter, blueBlob);
    } else {
        blueFound = processColor(m_frame, m_blueFrame, *m_blueFilter, blueBlob);
    }

    // process the image to find the blue square
    blueFound = processColor(m_frame, m_blueFrame, *m_blueFilter, blueBlob);
    if(blueFound)
    {
        blueHolesFound = findHoles(m_frame, *m_blueFilter, blueBlob,
                                     smallHoleBlob, largeHoleBlob);
        if(blueHolesFound)
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
                
                if(blueFound && blueHolesFound)
                {
                    BlobDetector::Blob debugBlob;

                    CvPoint smallP1;
                    smallP1.x = blueBlob.getMinX() + smallHoleBlob.getMinX();
                    smallP1.y = blueBlob.getMinY() + smallHoleBlob.getMinY();

                    CvPoint smallP2;
                    smallP2.x = blueBlob.getMinX() + smallHoleBlob.getMaxX();
                    smallP2.y = blueBlob.getMinY() + smallHoleBlob.getMaxY();

                    CvPoint smallCenter;
                    smallCenter.x = (smallP1.x + smallP2.x) / 2;
                    smallCenter.y = (smallP1.y + smallP2.y) / 2;

                    cvRectangle(output->asIplImage(), smallP1, smallP2,
                                cvScalar(100,100,100),3);
                    cvCircle(output->asIplImage(), smallCenter, 3,
                             cvScalar(255, 255, 255));

                    CvPoint largeP1;
                    largeP1.x = blueBlob.getMinX() + largeHoleBlob.getMinX();
                    largeP1.y = blueBlob.getMinY() + largeHoleBlob.getMinY();

                    CvPoint largeP2;
                    largeP2.x = blueBlob.getMinX() + largeHoleBlob.getMaxX();
                    largeP2.y = blueBlob.getMinY() + largeHoleBlob.getMaxY();

                    CvPoint largeCenter;
                    largeCenter.x = (largeP1.x + largeP2.x) / 2;
                    largeCenter.y = (largeP1.y + largeP2.y) / 2;

                    cvRectangle(output->asIplImage(), largeP1, largeP2,
                                cvScalar(100,100,100),3);
                    cvCircle(output->asIplImage(), largeCenter, 3,
                             cvScalar(255, 255, 255));
                }
                if(redFound && redHolesFound)
                {
                    CvPoint smallP1;
                    smallP1.x = redBlob.getMinX() + smallHoleBlob.getMinX();
                    smallP1.y = redBlob.getMinY() + smallHoleBlob.getMinY();

                    CvPoint smallP2;
                    smallP2.x = redBlob.getMinX() + smallHoleBlob.getMaxX();
                    smallP2.y = redBlob.getMinY() + smallHoleBlob.getMaxY();

                    CvPoint smallCenter;
                    smallCenter.x = (smallP1.x + smallP2.x) / 2;
                    smallCenter.y = (smallP1.y + smallP2.y) / 2;

                    cvRectangle(output->asIplImage(), smallP1, smallP2,
                                cvScalar(100,100,100),3);
                    cvCircle(output->asIplImage(), smallCenter, 3,
                             cvScalar(255, 255, 255));

                    CvPoint largeP1;
                    largeP1.x = redBlob.getMinX() + largeHoleBlob.getMinX();
                    largeP1.y = redBlob.getMinY() + largeHoleBlob.getMinY();

                    CvPoint largeP2;
                    largeP2.x = redBlob.getMinX() + largeHoleBlob.getMaxX();
                    largeP2.y = redBlob.getMinY() + largeHoleBlob.getMaxY();

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

bool CaesarDetector::processColor(Image* input, Image* output,
                                       ImageFilter& filter,
                                       BlobDetector::Blob& outputBlob)
{
    // Set the pixel format for processing
    m_processingFrame->copyFrom(input);
    m_processingFrame->setPixelFormat(Image::PF_RGB_8);
    m_processingFrame->setPixelFormat(Image::PF_LCHUV_8);

    if ( m_colorFilterLookupTable )
        filter.filterImage(input, output);
    else 
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
    
bool CaesarDetector::findHoles(Image *input,
                               ColorFilter& filter,
                               BlobDetector::Blob& windowBlob,
                               BlobDetector::Blob& smallHole,
                               BlobDetector::Blob& largeHole)
{
    m_holesFrame->copyFrom(input);
    m_holesFrame->setPixelFormat(Image::PF_RGB_8);
    m_holesFrame->setPixelFormat(Image::PF_LCHUV_8);

    unsigned char *buffer = new unsigned char[windowBlob.getWidth()*windowBlob.getHeight()*3];
    Image *innerFrame = Image::extractSubImage(
        m_holesFrame, buffer,
        windowBlob.getMinX(), windowBlob.getMinY(),
        windowBlob.getMaxX(), windowBlob.getMaxY());

    filter.inverseFilterImage(innerFrame);

    m_blobDetector.processImage(innerFrame);
    delete innerFrame;
    delete [] buffer;

    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();
    
    // for now assume the largest blob is the big hole and the
    // second largest blob is the small hole.  if this is not
    // the case we probably are screwed anyway.  if we don't have
    // two blobs, we arent looking at the cupid so we should return
    // false.
    bool foundHoles = false;
    bool foundLarge = false;

    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        if(!foundHoles)
        {
            // figure out if the blob could be a hole
            double aspect = blob.getAspectRatio();
            double pixelPct = blob.getFillPercentage();

            if(aspect > m_minHoleAspectRatio &&
               aspect < m_maxHoleAspectRatio &&
               pixelPct > m_minHolePixelPct)
            {
                // we have found a blob so if it is the first
                // one, assign it to the large hole
                if(!foundLarge)
                {
                    largeHole = blob;
                    foundLarge = true;
                }
                // if we have already found the large hole,
                // it must be the small hole
                else
                {
                    smallHole = blob;
                    foundHoles = true;
                }
            }
        }
    }

    return foundHoles;
}


void CaesarDetector::show(char* window)
{
    vision::Image::showImage(m_frame, "CaesarDetector");
}

void CaesarDetector::drawDebugCircle(BlobDetector::Blob blob,
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

IplImage* CaesarDetector::getAnalyzedImage()
{
    return m_frame->asIplImage();
}

void CaesarDetector::publishFoundEvent(const BlobDetector::Blob& blob,
                                            Color::ColorType color)
{
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

    CaesarEventPtr event(new CaesarEvent());

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

    publish(EventType::CAESAR_FOUND, event);
    publish(EventType::CAESAR_SMALL_FOUND, event);
    publish(EventType::CAESAR_LARGE_FOUND, event);
}

void CaesarDetector::publishLostEvent(Color::ColorType color)
{
    WindowEventPtr event(new WindowEvent());
    event->color = color;
    publish(EventType::CAESAR_LOST, event);
}

} // namespace vision
} // namespace ram
