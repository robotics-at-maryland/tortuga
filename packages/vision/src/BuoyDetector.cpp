/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/BuoyDetector.cpp
 */

// STD Includes
#include <math.h>
#include <algorithm>
#include <iostream>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include <log4cpp/Category.hh>

#include <boost/foreach.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

#include "vision/include/BuoyDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Color.h"
#include "vision/include/Events.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/RegionOfInterest.h"
#include "vision/include/Utility.h"
#include "vision/include/VisionSystem.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("BuoyDetector"));

namespace ram {
namespace vision {

BuoyDetector::BuoyDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_redFound(false),
    m_greenFound(false),
    m_yellowFound(false),
    m_redFilter(0),
    m_greenFilter(0),
    m_yellowFilter(0),
    m_blobDetector(config, eventHub),
    m_checkBlack(false),
    m_minBlackPercentage(0),
    m_maxTotalBlackCheckSize(0),
    m_physicalWidthMeters(0.26)
{
    init(config);
}

BuoyDetector::BuoyDetector(Camera* camera) :
    cam(camera),
    m_redFound(false),
    m_greenFound(false),
    m_yellowFound(false),
    m_redFilter(0),
    m_greenFilter(0),
    m_yellowFilter(0),
    m_checkBlack(false),
    m_minBlackPercentage(0),
    m_maxTotalBlackCheckSize(0),
    m_physicalWidthMeters(0.26)
{
    init(core::ConfigNode::fromString("{}"));
}

void BuoyDetector::init(core::ConfigNode config)
{
    LOGGER.info("Initializing Buoy Detector");
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

    propSet->addProperty(config, false, "minPixels",
                         "Minimum number of pixels", 15, &m_minPixels);

    propSet->addProperty(config, false, "maxDistance",
                         "Maximum distance between two blobs from different frames",
                         15.0, &m_maxDistance);

    propSet->addProperty(config, false, "almostHitPercentage",
                         "Radius when the buoy is considered almost hit",
                         0.2, &m_almostHitPercentage, 0.0, 1.0);


    propSet->addProperty(config, false, "checkBlack",
                         "Whether or not to look for black beneath the buoy",
                         false, &m_checkBlack);

    propSet->addProperty(config, false, "minBlackPercentage",
                         "The precentage of the subwindow that must be black",
                         0.25, &m_minBlackPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "maxTotalBlackCheckSize",
                         "The biggest fraction of the window for the black check",
                         0.1, &m_maxTotalBlackCheckSize, 0.0, 1.0);


    propSet->addProperty(config, false, "topIgnorePercentage",
        "% of the screen from the top to be blacked out",
        0.0, &m_topIgnorePercentage);

    propSet->addProperty(config, false, "bottomIgnorePercentage",
        "% of the screen from the bottom to be blacked out",
        0.0, &m_bottomIgnorePercentage);

    propSet->addProperty(config, false, "leftIgnorePercentage",
        "% of the screen from the left to be blacked out",
        0.0, &m_leftIgnorePercentage);

    propSet->addProperty(config, false, "rightIgnorePercentage",
        "% of the screen from the right to be blacked out",
        0.0, &m_rightIgnorePercentage);
    



    m_redFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_redFilter->addPropertiesToSet(propSet, &config,
                                    "RedL", "Red Luminance",
                                    "RedC", "Red Chrominance",
                                    "RedH", "Red Hue",
                                    0, 255, 0, 255, 0, 255);
    m_greenFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_greenFilter->addPropertiesToSet(propSet, &config,
                                      "GreenL", "Green Luminance",
                                      "GreenC", "Green Chrominance",
                                      "GreenH", "Green Hue",
                                      0, 255, 0, 255, 0, 255);
    m_yellowFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_yellowFilter->addPropertiesToSet(propSet, &config,
                                       "YellowL", "Yellow Luminance",
                                       "YellowC", "Yellow Chrominance",
                                       "YellowH", "Yellow Hue",
                                       0, 255, 0, 255, 0, 255);

    m_blackFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_blackFilter->addPropertiesToSet(propSet, &config,
                                      "BlackL", "Black Luminance",
                                      "BlackC", "Black Chrominance",
                                      "BlackH", "Black Hue",
                                      0, 255, 0, 255, 0, 255);



    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);
    
    // Working images
    frame = new OpenCVImage(640, 480, Image::PF_BGR_8);

    redFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    greenFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    yellowFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    blackFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);

    LOGGER.info("foundRed redX redY redRange redWidth "
                "redHeight redNumPixels redPixelPct" 
                "foundGreen greenX greenY greenRange greenWidth "
                "greenHeight greenNumPixels greenPixelPct"
                "foundYellow yellowX yellowY yellowRange yellowWidth "
                "yellowHeight yellowNumPixel yellowPixelPct");
}

BuoyDetector::~BuoyDetector()
{
    delete m_redFilter;
    delete m_greenFilter;
    delete m_yellowFilter;

    delete frame;
    delete redFrame;
    delete greenFrame;
}

void BuoyDetector::update()
{
    cam->getImage(frame);
    processImage(frame);
}

bool BuoyDetector::processColor(Image* input, Image* output,
                                ColorFilter& filter,
                                BlobDetector::Blob& outBlob)
{
    const int imgWidth = input->getWidth();
    const int imgHeight = input->getHeight();
    const int imgPixels = imgWidth * imgHeight;

    output->copyFrom(input);
    output->setPixelFormat(Image::PF_RGB_8);
    output->setPixelFormat(Image::PF_LCHUV_8);

    filter.filterImage(output);

    m_blobDetector.processImage(output);
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

    bool foundBlob = false;
    
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Sanity check blob
        double percent = (double) blob.getSize() /
            (blob.getWidth() * blob.getHeight());
        if (blob.getTrueAspectRatio() < m_maxAspectRatio &&
            blob.getHeight() > m_minHeight &&
            blob.getWidth() > m_minWidth &&
            blob.getSize() > m_minPixels &&
            percent > m_minPixelPercentage &&
            blob.getTrueAspectRatio() > m_minAspectRatio)
        {
            foundBlob = true;

            // Check for black below blob (if desired)
            if (m_checkBlack)
            {
                int height = blob.getHeight();
                double totalSize = blob.getHeight() * blob.getWidth();
        
                // Only check this when the buoy should be far away
                if ((totalSize / imgPixels) < m_maxTotalBlackCheckSize )
                {
                    double blackCount = Utility::countWhitePixels(blackFrame,
                                                                  blob.getMinX(), 
                                                                  blob.getMinY() + height,
                                                                  blob.getMaxX(), 
                                                                  blob.getMaxY() + height);
            
                    double blackPercentage = blackCount / totalSize;
                    if (blackPercentage < m_minBlackPercentage)
                        foundBlob = false;
                }
            }

            if(foundBlob)
            {
                outBlob = blob;
                break;
            }
        }
    }

 

    return foundBlob;
}

void BuoyDetector::processImage(Image* input, Image* output)
{
    
    frame->copyFrom(input);

    int topRowsToIgnore = static_cast<int>(m_topIgnorePercentage * frame->getHeight());
    int bottomRowsToIgnore = static_cast<int>(m_bottomIgnorePercentage * frame->getHeight());
    int leftColsToIgnore = static_cast<int>(m_leftIgnorePercentage * frame->getWidth());
    int rightColsToIgnore = static_cast<int>(m_rightIgnorePercentage * frame->getWidth());

    int initialMinX = leftColsToIgnore;
    int initialMaxX = frame->getWidth() - rightColsToIgnore;
    int initialMinY = topRowsToIgnore;
    int initialMaxY = frame->getHeight() - bottomRowsToIgnore;

    RegionOfInterest initialROI = RegionOfInterest(initialMinX, initialMaxX,
                                                   initialMinY, initialMaxY);

    int framePixels = initialROI.area();
    int almostHitPixels = framePixels * m_almostHitPercentage;

    // Filter for black if needed
    if (m_checkBlack)
    {
        blackFrame->copyFrom(frame);
        blackFrame->setPixelFormat(Image::PF_RGB_8);
        blackFrame->setPixelFormat(Image::PF_LCHUV_8);

        m_blackFilter->filterImage(blackFrame);
    }

    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();

    BlobDetector::Blob redBlob;
    bool redFound = processColor(frame, redFrame, *m_redFilter, redBlob);
    if (redFound)
    {
        publishFoundEvent(redBlob, Color::RED);
        int blobPixels = redBlob.getSize();

        if(blobPixels > almostHitPixels)
            publish(EventType::BUOY_ALMOST_HIT,
                    core::EventPtr(new core::Event()));

        double fracWidth = static_cast<double>(redBlob.getWidth()) / xPixelWidth;
        double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
        int width = redBlob.getWidth();
        int height = redBlob.getHeight();

        LOGGER.infoStream() << "1" << " "
                            << redBlob.getCenterX() << " "
                            << redBlob.getCenterY() << " "
                            << range << " "
                            << width << " "
                            << height << " "
                            << blobPixels << " "
                            << blobPixels / (width * height);
    } 
    else 
    {
        // Publish lost event if this was found previously
        if (m_redFound) 
            publishLostEvent(Color::RED);

        LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
        
    }
    m_redFound = redFound;





    BlobDetector::Blob greenBlob;
    bool greenFound = processColor(frame, greenFrame, *m_greenFilter, greenBlob);
    if (greenFound)
    {
        publishFoundEvent(greenBlob, Color::GREEN);
        int blobPixels = greenBlob.getSize();

        if(blobPixels > almostHitPixels)
            publish(EventType::BUOY_ALMOST_HIT,
                    core::EventPtr(new core::Event()));

        double fracWidth = static_cast<double>(greenBlob.getWidth()) / xPixelWidth;
        double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
        int width = greenBlob.getWidth();
        int height = greenBlob.getHeight();
        
        LOGGER.infoStream() << "1" << " "
                            << greenBlob.getCenterX() << " "
                            << greenBlob.getCenterY() << " "
                            << range << " "
                            << width << " "
                            << height << " "
                            << blobPixels << " "
                            << blobPixels / (width * height);

    }
    else
    {
        // Publish lost event if this was found previously
        if (m_greenFound)
            publishLostEvent(Color::GREEN);

        LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
        
    }
    m_greenFound = greenFound;

    BlobDetector::Blob yellowBlob;
    bool yellowFound = processColor(frame, yellowFrame, *m_yellowFilter, yellowBlob);
    if (yellowFound)
    {
        publishFoundEvent(yellowBlob, Color::YELLOW);
        int blobPixels = yellowBlob.getSize();

        if(blobPixels > almostHitPixels)
            publish(EventType::BUOY_ALMOST_HIT,
                    core::EventPtr(new core::Event()));

        double fracWidth = static_cast<double>(yellowBlob.getWidth()) / xPixelWidth;
        double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
        int width = greenBlob.getWidth();
        int height = greenBlob.getHeight();

        LOGGER.infoStream() << "1" << " "
                            << greenBlob.getCenterX() << " "
                            << greenBlob.getCenterY() << " "
                            << range << " "
                            << width << " "
                            << height << " "
                            << blobPixels << " "
                            << blobPixels / (width * height);

    } 
    else
    {
        // Publish lost event if this was found previously
        if (m_yellowFound)
            publishLostEvent(Color::YELLOW);

        LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
        
    }
    m_yellowFound = yellowFound;

    if(output)
    {
        output->copyFrom(frame);
        if (m_debug >= 1) {
            output->copyFrom(frame);

            Image::fillMask(output, redFrame, 255, 0, 0);
            Image::fillMask(output, greenFrame, 0, 255, 0);
            Image::fillMask(output, yellowFrame, 255, 255, 0);
            Image::fillMask(output, blackFrame, 147, 20, 255);
        }

        if (m_debug == 2) {
            if (redFound)
                drawBuoyDebug(output, redBlob, 255, 0, 0);
            if (greenFound)
                drawBuoyDebug(output, greenBlob, 0, 255, 0);
            if (yellowFound)
                drawBuoyDebug(output, yellowBlob, 255, 255, 0);
        }
    }
}

void BuoyDetector::publishFoundEvent(BlobDetector::Blob& blob, Color::ColorType color)
{
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

    BuoyEventPtr event = BuoyEventPtr(new BuoyEvent());
    
    double centerX = 0, centerY = 0;
    Detector::imageToAICoordinates(frame, blob.getCenterX(), blob.getCenterY(),
                                   centerX, centerY);

    double blobWidth = blob.getWidth();
    double fracWidth = blobWidth / xPixelWidth;
    double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));

    int minX = blob.getMinX();
    int maxX = blob.getMaxX();
    int minY = blob.getMinY();
    int maxY = blob.getMaxY();

    bool touchingEdge = false;
    if(minX == 0 || minY == 0 || maxX == xPixelWidth || maxY == yPixelHeight)
        touchingEdge = true;

    event->x = centerX;
    event->y = centerY;
    event->range = range;
    event->azimuth = math::Degree((-1) * (xFOV / 2) * centerX);
    event->elevation = math::Degree((yFOV / 2) * centerY);
    event->color = color;
    event->touchingEdge = touchingEdge;

    publish(EventType::BUOY_FOUND, event);
}

void BuoyDetector::publishLostEvent(Color::ColorType color)
{
    BuoyEventPtr event(new BuoyEvent());
    event->color = color;
    
    publish(EventType::BUOY_LOST, event);
}

void BuoyDetector::drawBuoyDebug(Image* debugImage, BlobDetector::Blob &blob,
                   unsigned char red, unsigned char green,
                   unsigned char blue)
{
    CvPoint center;
    center.x = blob.getCenterX();
    center.y = blob.getCenterY();
    
    blob.drawStats(debugImage);
    blob.draw(debugImage);

    cvCircle(debugImage->asIplImage(), center, 3,
             cvScalar(blue, green, red), -1);
}

} // namespace vision
} // namespace ram
