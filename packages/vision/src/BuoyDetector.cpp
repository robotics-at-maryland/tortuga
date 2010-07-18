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
#include "vision/include/SegmentationFilter.h"
#include "vision/include/ColorFilter.h"

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
    m_maxTotalBlackCheckSize(0)
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
    m_maxTotalBlackCheckSize(0)
{
    init(core::ConfigNode::fromString("{}"));
}

void BuoyDetector::init(core::ConfigNode config)
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


    propSet->addProperty(config, false, "topRemovePercentage",
        "% of the screen from the top to be blacked out",
        0.0, &m_topRemovePercentage);

    propSet->addProperty(config, false, "bottomRemovePercentage",
        "% of the screen from the bottom to be blacked out",
        0.0, &m_bottomRemovePercentage);

    propSet->addProperty(config, false, "leftRemovePercentage",
        "% of the screen from the left to be blacked out",
        0.0, &m_leftRemovePercentage);

    propSet->addProperty(config, false, "rightRemovePercentage",
        "% of the screen from the right to be blacked out",
        0.0, &m_rightRemovePercentage);
    



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
                if ((totalSize / (640.0*480.0)) < m_maxTotalBlackCheckSize )
                {
                    double blackCount = countWhitePixels(blackFrame,         
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

int BuoyDetector::countWhitePixels(Image* source,
                                   int upperLeftX, int upperLeftY,
                                   int lowerRightX, int lowerRightY)
{
    unsigned char* sourceBuffer = source->getData();
    unsigned char* srcPtr = sourceBuffer;

    int width = lowerRightX - upperLeftX; //+ 1;
    int height = lowerRightY - upperLeftY; //+ 1;

    int yStart = upperLeftY;
    int yEnd = yStart + height;

    int whiteCount = 0;

    for (int y = yStart; y < yEnd; ++y)
    {
        // Get us to right row and column to start
        int offset = (y * source->asIplImage()->widthStep) + (upperLeftX * 3);
        srcPtr = sourceBuffer + offset;
        
        for (int x = 0; x < (width * 3); ++x)
        {
            // If white increment (note this is for subpixels)
            if (*srcPtr)
                whiteCount++;

            ++srcPtr;
        }
    }

    return whiteCount / 3;
}


void BuoyDetector::processImage(Image* input, Image* output)
{
    frame->copyFrom(input);


    // Filter for black if needed
    if (m_checkBlack)
    {
        blackFrame->copyFrom(frame);
        blackFrame->setPixelFormat(Image::PF_RGB_8);
        blackFrame->setPixelFormat(Image::PF_LCHUV_8);

        m_blackFilter->filterImage(blackFrame);
    }

    int totRowsRemoved = 0;
    int totColRemoved = 0;

    // Remove top chunk if desired
    if (m_topRemovePercentage != 0)
    {
        int linesToRemove = (int)(m_topRemovePercentage * frame->getHeight());
        totRowsRemoved += linesToRemove;
        size_t bytesToBlack = linesToRemove * frame->getWidth() * 3;
        memset(frame->getData(), 0, bytesToBlack);
    }
    

    if (m_bottomRemovePercentage != 0)
    {
//        printf("Removing \n");
        int linesToRemove = (int)(m_bottomRemovePercentage * frame->getHeight());
        totRowsRemoved += linesToRemove;
        size_t bytesToBlack = linesToRemove * frame->getWidth() * 3;
        memset(&(frame->getData()[frame->getWidth() * frame->getHeight()
                                  * 3 - bytesToBlack]), 0, bytesToBlack);
    }

    if (m_rightRemovePercentage != 0)
    {
        size_t lineSize = frame->getWidth() * 3;
        size_t bytesToBlack = (int)(m_rightRemovePercentage * lineSize);
        totColRemoved += bytesToBlack / 3;
        for (unsigned int i = 0; i < frame->getHeight(); ++i)
        {
            size_t offset = i * lineSize - bytesToBlack;
            memset(frame->getData() + offset, 0, bytesToBlack);
        }
    }

    if (m_leftRemovePercentage != 0)
    {
        size_t lineSize = frame->getWidth() * 3;
        size_t bytesToBlack = (int)(m_leftRemovePercentage * lineSize);
        totColRemoved += bytesToBlack / 3;
        for (unsigned int i = 0; i < frame->getHeight(); ++i)
        {
            size_t offset = i * lineSize;
            memset(frame->getData() + offset, 0, bytesToBlack);
        }
    }

    BlobDetector::Blob redBlob, greenBlob, yellowBlob;
    bool redFound = false, greenFound = false,
        yellowFound = false;

    int imPixels = (frame->getHeight() - totRowsRemoved) *
        (frame->getWidth() - totColRemoved);

    if ((redFound = processColor(frame, redFrame, *m_redFilter, redBlob))) {
        publishFoundEvent(redBlob, Color::RED);
        int blobPixels = redBlob.getSize();
        if(blobPixels > imPixels * m_almostHitPercentage)
            publish(EventType::BUOY_ALMOST_HIT, core::EventPtr(new core::Event()));

    } else {
        // Publish lost event if this was found previously
        if (m_redFound) {
            publishLostEvent(Color::RED);
        }
    }
    m_redFound = redFound;

    if ((greenFound = processColor(frame, greenFrame,
                                   *m_greenFilter, greenBlob))) {
        publishFoundEvent(greenBlob, Color::GREEN);
        int blobPixels = greenBlob.getSize();
        if(blobPixels > imPixels * m_almostHitPercentage)
            publish(EventType::BUOY_ALMOST_HIT, core::EventPtr(new core::Event()));

    } else {
        // Publish lost event if this was found previously
        if (m_greenFound) {
            publishLostEvent(Color::GREEN);
        }
    }
    m_greenFound = greenFound;

    if ((yellowFound = processColor(frame, yellowFrame,
                                    *m_yellowFilter, yellowBlob))) {
        publishFoundEvent(yellowBlob,  Color::YELLOW);
        int blobPixels = yellowBlob.getSize();
        if(blobPixels > imPixels * m_almostHitPercentage)
            publish(EventType::BUOY_ALMOST_HIT, core::EventPtr(new core::Event()));

    } else {
        // Publish lost event if this was found previously
        if (m_yellowFound) {
            publishLostEvent(Color::YELLOW);
        }
    }
    m_yellowFound = yellowFound;

    if(output)
    {
        if (m_debug == 0) {
            output->copyFrom(frame);
        } else {
            output->copyFrom(frame);

            // Color in the targets (as we find them)
            unsigned char* data = output->getData();
            unsigned char* redPtr = redFrame->getData();
            unsigned char* greenPtr = greenFrame->getData();
            unsigned char* yellowPtr = yellowFrame->getData();
            unsigned char* blackPtr = blackFrame->getData();

            unsigned char* end = output->getData() +
                (output->getWidth() * output->getHeight() * 3);
            
            for (; data != end; data += 3) {
                if (*redPtr) {
                    data[0] = 0;
                    data[1] = 0;
                    data[2] = 255;
                } else if (*greenPtr) {
                    data[0] = 0;
                    data[1] = 255;
                    data[2] = 0;
                } else if (*yellowPtr) {
                    data[0] = 0;
                    data[1] = 255;
                    data[2] = 255;
                } else if (*blackPtr) {
                    data[0] = 255;
                    data[1] = 20;
                    data[2] = 147;
                }

                // Advance all of the pointers
                redPtr += 3;
                greenPtr += 3;
                yellowPtr += 3;
                blackPtr += 3;
            }
            if (m_debug == 2) {
                if (redFound) {
                    CvPoint center;
                    center.x = redBlob.getCenterX();
                    center.y = redBlob.getCenterY();

                    redBlob.drawStats(output);
                    redBlob.draw(output);
                    // Red
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 0, 255), -1);
                }

                if (greenFound) {
                    CvPoint center;
                    center.x = greenBlob.getCenterX();
                    center.y = greenBlob.getCenterY();

                    greenBlob.drawStats(output);
                    greenBlob.draw(output);
                    // Green
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 255, 0), -1);
                }

                if (yellowFound) {
                    CvPoint center;
                    center.x = yellowBlob.getCenterX();
                    center.y = yellowBlob.getCenterY();

                    yellowBlob.drawStats(output);
                    yellowBlob.draw(output);
                    // Yellow (Red + Green)
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 255, 255), -1);
                }
            }
        }
    }
}


bool BuoyDetector::inrange(int min, int max, int value)
{
    if (min <= max) {
        return min <= value && value <= max;
    } else {
        return min <= value || value <= max;
    }
}

void BuoyDetector::show(char* window)
{
    Image *debug = new OpenCVImage(640, 480);

    // Reprocess the last frame with debug on
    // (We may be recalculating everything, but speed isn't an issue)
    processImage(frame, debug);

    cvNamedWindow(window);
    cvShowImage(window, debug->asIplImage());

    cvWaitKey(0);

    cvDestroyWindow(window);

    delete debug;
}

void BuoyDetector::publishFoundEvent(BlobDetector::Blob& blob, Color::ColorType color)
{
    BuoyEventPtr event(new BuoyEvent());

    double centerX, centerY;
    Detector::imageToAICoordinates(frame, blob.getCenterX(), blob.getCenterY(),
                                   centerX, centerY);

    event->x = centerX;
    event->y = centerY;
    event->color = color;

    event->azimuth = math::Degree(
        (78.0 / 2) * centerX * -1.0 *
        (double)frame->getHeight()/frame->getWidth());
    event->elevation = math::Degree((105.0 / 2) * centerY * 1);

    // Compute range (assume a sphere)
    double lightPixelRadius = sqrt((double)blob.getSize()/M_PI);
    double lightRadius = 0.25; // feet
    event->range = (lightRadius * frame->getHeight()) /
        (lightPixelRadius * tan(78.0/2 * (M_PI/180)));
        
    publish(EventType::BUOY_FOUND, event);
}

void BuoyDetector::publishLostEvent(Color::ColorType color)
{
    BuoyEventPtr event(new BuoyEvent());
    event->color = color;
    
    publish(EventType::BUOY_LOST, event);
}

} // namespace vision
} // namespace ram
