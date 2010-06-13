/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/WindowDetector.cpp
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
#include "vision/include/WindowDetector.h"

namespace ram {
namespace vision {

WindowDetector::WindowDetector(core::ConfigNode config,
                               core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_redFilter(0),
    m_greenFilter(0),
    m_yellowFilter(0),
    m_blueFilter(0),
    m_blobDetector(config, eventHub)
{
    init(config);
}

WindowDetector::WindowDetector(Camera* camera) :
    cam(camera),
    m_redFilter(0),
    m_greenFilter(0),
    m_yellowFilter(0),
    m_blueFilter(0)
{
    init(core::ConfigNode::fromString("{}"));
}

WindowDetector::~WindowDetector()
{
    delete m_redFilter;
    delete m_greenFilter;
    delete m_yellowFilter;
    delete m_blueFilter;

    delete frame;
    delete redFrame;
    delete greenFrame;
    delete yellowFrame;
    delete blueFrame;
}

void WindowDetector::init(core::ConfigNode config)
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

    propSet->addProperty(config, false, "maxDistance",
                         "Maximum distance between two blobs from different frames",
                         15.0, &m_maxDistance);

    m_redFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_redFilter->addPropertiesToSet(propSet, &config,
                                    "RedH", "Red Hue",
                                    "RedS", "Red Saturation",
                                    "RedV", "Red Value",
                                    0, 255, 0, 255, 0, 255);
    m_greenFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_greenFilter->addPropertiesToSet(propSet, &config,
                                      "GreenH", "Green Hue",
                                      "GreenS", "Green Saturation",
                                      "GreenV", "Green Value",
                                      0, 255, 0, 255, 0, 255);
    m_yellowFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_yellowFilter->addPropertiesToSet(propSet, &config,
                                       "YellowH", "Yellow Hue",
                                       "YellowS", "Yellow Saturation",
                                       "YellowV", "Yellow Value",
                                       0, 255, 0, 255, 0, 255);
    m_blueFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_blueFilter->addPropertiesToSet(propSet, &config,
                                     "BlueH", "Blue Hue",
                                     "BlueS", "Blue Saturation",
                                     "BlueV", "Blue Value",
                                     0, 255, 0, 255, 0, 255);

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);
    
    // Working images
    frame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    redFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    greenFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    yellowFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    blueFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
}

void WindowDetector::update()
{
    cam->getImage(frame);
    processImage(frame);
}

bool WindowDetector::processColor(Image* input, Image* output,
                                  ColorFilter& filter,
                                  BlobDetector::Blob& outBlob)
{
    output->copyFrom(input);
    output->setPixelFormat(Image::PF_HSV_8);
    filter.filterImage(output);

    OpenCVImage debug(output->getWidth(), output->getHeight(),
                              Image::PF_BGR_8);
    m_blobDetector.processImage(output, &debug);
    //Image::showImage(&debug);
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Sanity check blob
        if (blob.getAspectRatio() < m_maxAspectRatio)
        {
            outBlob = blob;
            return true;
        }
    }

    return false;
}

void WindowDetector::processImage(Image* input, Image* output)
{
    frame->copyFrom(input);

    BlobDetector::Blob redBlob, greenBlob, yellowBlob, blueBlob;
    bool redFound = false, greenFound = false,
        yellowFound = false, blueFound = false;

    if ((redFound = processColor(frame, redFrame, *m_redFilter, redBlob))) {
        publishFoundEvent(redBlob, Color::RED);
    } else {
        // Publish lost event if this was found previously
    }

    if ((greenFound = processColor(frame, greenFrame,
                                   *m_greenFilter, greenBlob))) {
        publishFoundEvent(greenBlob, Color::GREEN);
    } else {
        // Publish lost event if this was found previously
    }

    if ((yellowFound = processColor(frame, yellowFrame,
                                    *m_yellowFilter, yellowBlob))) {
        publishFoundEvent(yellowBlob, Color::YELLOW);
    } else {
        // Publish lost event if this was found previously
    }

    if ((blueFound = processColor(frame, blueFrame, *m_blueFilter, blueBlob))) {
        publishFoundEvent(blueBlob, Color::BLUE);
    } else {
        // Publish lost event if this was found previously
    }

    if (output)
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
            unsigned char* bluePtr = blueFrame->getData();
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
                } else if (*bluePtr) {
                    data[0] = 255;
                    data[1] = 0;
                    data[2] = 0;
                }

                // Advance all of the pointers
                redPtr += 3;
                greenPtr += 3;
                yellowPtr += 3;
                bluePtr += 3;
            }
            
            if (m_debug == 2) {
                if (redFound) {
                    CvPoint center;
                    center.x = redBlob.getCenterX();
                    center.y = redBlob.getCenterY();

                    redBlob.drawStats(output);
                    // Red
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 0, 255), -1);
                }

                if (greenFound) {
                    CvPoint center;
                    center.x = greenBlob.getCenterX();
                    center.y = greenBlob.getCenterY();

                    greenBlob.drawStats(output);
                    // Green
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 255, 0), -1);
                }

                if (yellowFound) {
                    CvPoint center;
                    center.x = yellowBlob.getCenterX();
                    center.y = yellowBlob.getCenterY();

                    yellowBlob.drawStats(output);
                    // Yellow (Red + Green)
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(0, 255, 255), -1);
                }

                if (blueFound) {
                    CvPoint center;
                    center.x = blueBlob.getCenterX();
                    center.y = blueBlob.getCenterY();

                    blueBlob.drawStats(output);
                    // Blue
                    cvCircle(output->asIplImage(), center, 3,
                             cvScalar(255, 0, 0), -1);
                }
            }
        }
    }
}

void WindowDetector::show(char* window)
{
    vision::Image::showImage(frame, "WindowDetector");
}

IplImage* WindowDetector::getAnalyzedImage()
{
    return frame->asIplImage();
}

void WindowDetector::publishFoundEvent(const BlobDetector::Blob& blob,
                                       Color::ColorType color)
{
    TargetEventPtr event(new TargetEvent());
    event->color = color;

    Detector::imageToAICoordinates(frame,
                                   blob.getCenterX(),
                                   blob.getCenterY(),
                                   event->x,
                                   event->y);

    // Determine range
    event->range = 1.0 - (((double)blob.getHeight()) /
                          ((double)frame->getHeight()));

    // Determine the squareness
    double aspectRatio = blob.getTrueAspectRatio();
    if (aspectRatio < 1)
        event->squareNess = 1.0;
    else
        event->squareNess = 1.0/aspectRatio;

    publish(EventType::TARGET_FOUND, event);
}

void WindowDetector::publishLostEvent(Color::ColorType color)
{
    TargetEventPtr event(new TargetEvent());
    event->color = color;
    
    publish(EventType::TARGET_LOST, event);
}

} // namespace vision
} // namespace ram
