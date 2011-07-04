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
#include "vision/include/HeartWindowDetector.h"
#include "vision/include/VisionSystem.h"

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("HeartWindowDetectorLog"));

namespace ram {
namespace vision {

HeartWindowDetector::HeartWindowDetector(core::ConfigNode config,
                                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_redFilter(0),
    m_blueFilter(0),
    m_blobDetector(config, eventHub),
    m_physicalWidthMeters(0.61)
{
    init(config);
}

HeartWindowDetector::HeartWindowDetector(Camera* camera) :
    cam(camera),
    m_redFilter(0),
    m_blueFilter(0),
    m_physicalWidthMeters(0.61)
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

    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();

    BlobDetector::Blob redBlob, blueBlob;

    // process the image to find the red square
    bool redFound = processColor(frame, redFrame, *m_redFilter, redBlob);
    if(redFound)
    {
        publishFoundEvent(redBlob, Color::RED);

        int blobPixels = redBlob.getSize();
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
    else if(m_redFound)
    {
        publishLostEvent(Color::RED);
        LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
    }
    m_redFound = redFound;


    // process the image to find the blue square
    bool blueFound = processColor(frame, blueFrame, *m_blueFilter, blueBlob);
    if(blueFound)
    {
        publishFoundEvent(blueBlob, Color::BLUE);

        int blobPixels = blueBlob.getSize();
        double fracWidth = static_cast<double>(blueBlob.getWidth()) / xPixelWidth;
        double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
        int width = blueBlob.getWidth();
        int height = blueBlob.getHeight();

        LOGGER.infoStream() << "1" << " "
                            << blueBlob.getCenterX() << " "
                            << blueBlob.getCenterY() << " "
                            << range << " "
                            << width << " "
                            << height << " "
                            << blobPixels << " "
                            << blobPixels / (width * height);
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
        output->copyFrom(frame);
        if(m_debug > 0)
        {
            output->copyFrom(frame);

            Image::fillMask(output, redFrame, 255, 0, 0);
            Image::fillMask(output, blueFrame, 0, 0, 255);
            
            if(m_debug == 2)
            {
                if(redFound)
                    drawDebugCircle(redBlob, output);

                if(blueFound)
                    drawDebugCircle(blueBlob, output);

            } // debug == 2
        } // debug > 0
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

void HeartWindowDetector::drawDebugCircle(BlobDetector::Blob blob,
                                          Image* output)
{
    CvPoint center;
    center.x = blob.getCenterX();
    center.y = blob.getCenterY();
    
    blob.drawStats(output);

    cvCircle(output->asIplImage(), center, 3, cvScalar(150, 0, 0), -1);
}

IplImage* HeartWindowDetector::getAnalyzedImage()
{
    return frame->asIplImage();
}

void HeartWindowDetector::publishFoundEvent(const BlobDetector::Blob& blob,
                                            Color::ColorType color)
{
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();

    WindowEventPtr event(new WindowEvent());

    double centerX, centerY;
    Detector::imageToAICoordinates(frame, blob.getCenterX(), blob.getCenterY(),
                                   centerX, centerY);
    
    double fracWidth = static_cast<double>(blob.getWidth()) / xPixelWidth;
    double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));

    // Determine range
    event->x = centerX;
    event->y = centerY;
    event->range = range;
    event->color = color;

    // Determine the squareness
    event->squareNess = blob.getAspectRatio();

    publish(EventType::CUPID_SMALL_FOUND, event);
    publish(EventType::CUPID_LARGE_FOUND, event);
}

void HeartWindowDetector::publishLostEvent(Color::ColorType color)
{
    WindowEventPtr event(new WindowEvent());
    event->color = color;
    publish(EventType::WINDOW_LOST, event);
}

} // namespace vision
} // namespace ram
