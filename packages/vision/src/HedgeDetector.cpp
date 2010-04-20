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
    m_lineDetector(config, eventHub),
    m_blobDetector(config, eventHub),
    m_filter(0)
{
    init(config);
}

HedgeDetector::HedgeDetector(Camera* camera)
{
    init(core::ConfigNode::fromString("{}"));
}

void HedgeDetector::init(core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "minGreenPixels",
                         "Minimum number of green pixels (Blob Only)",
                         400, &minGreenPixels);

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

    propSet->addProperty(config, false, "minAspectRatio",
                         "Min aspect ratio of the blob",
                         0.5, &m_minAspectRatio);

    propSet->addProperty(config, false, "maxAspectRatio",
                         "Max aspect ratio of the blob",
                         2.0, &m_maxAspectRatio);

    propSet->addProperty(config, false, "useLineDetection",
                         "Use Line Detector base",
                         false, &m_useLineDetection);

    propSet->addProperty(config, false, "debugLevel",
        "Image shown: 0=analyzed, 1=filtered, 2=preprocessed, 3=raw",
        0, &m_debug, 0, 3);

    m_filter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_filter->addPropertiesToSet(propSet, &config,
                                 "L", "L*",
                                 "U", "Blue Chrominance",
                                 "V", "Red Chrominance",
                                 0, 255,    // L defaults // 180, 255
                                 0, 200,    // U defaults // 76, 245
                                 200, 255); // V defaults // 200,255

    propSet->addPropertiesFromSet(m_lineDetector.getPropertySet());
    propSet->addPropertiesFromSet(m_blobDetector.getPropertySet());

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);

    // State machine variables
    found=false;
    m_hedgeCenterX=0;
    m_hedgeCenterY=0;
    hedgeCenter.x=0;
    hedgeCenter.y=0;

    // Working images
    frame = new vision::OpenCVImage(640, 48);
    raw = new vision::OpenCVImage(640, 480);
    preprocess = new vision::OpenCVImage(640, 480);
    if (m_useLineDetection)
        filtered = new vision::OpenCVImage(640, 480, vision::Image::PF_GRAY_8);
    else
        filtered = new vision::OpenCVImage(640, 480, vision::Image::PF_BGR_8);
    analyzed = new vision::OpenCVImage(640, 480, vision::Image::PF_GRAY_8);
}

HedgeDetector::~HedgeDetector()
{
    delete frame;
    delete raw;
    delete preprocess;
    delete filtered;
    delete analyzed;
}

double HedgeDetector::getX()
{
    return m_hedgeCenterX;
}

double HedgeDetector::getY()
{
    return m_hedgeCenterY;
}

void HedgeDetector::setTopRemovePercentage(double percent)
{
    m_topRemovePercentage = percent;
}

void HedgeDetector::setBottomRemovePercentage(double percent)
{
    m_bottomRemovePercentage = percent;
}

void HedgeDetector::show(char* window)
{
    cvShowImage(window,((IplImage*)(raw)));
}

Image* HedgeDetector::getAnalyzedImage()
{
    return analyzed;
}

void HedgeDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}

void HedgeDetector::processImage(Image* input, Image* output)
{
    // Resize images if needed
    if ((raw->getWidth() != input->getWidth()) &&
        (raw->getHeight() != input->getHeight()))
    {
        int width = (int)input->getWidth();
        int height = (int)input->getHeight();

        filtered->setSize(width, height);
        analyzed->setSize(width, height);
    }

    raw->copyFrom(input);
    preprocess->copyFrom(raw);

    // TODO: Add a pixel format for LUV and use setPixelFormat
    // Convert to LUV color space
    cvCvtColor(preprocess->asIplImage(), preprocess->asIplImage(), CV_BGR2Luv);
    
    // Remove top chunk if desired
    if (m_topRemovePercentage != 0)
    {
        int linesToRemove = (int)(m_topRemovePercentage * raw->getHeight());
        size_t bytesToBlack = linesToRemove * raw->getWidth()
            * raw->getNumChannels();
        memset(preprocess->asIplImage()->imageData, 0, bytesToBlack);
    }
    

    if (m_bottomRemovePercentage != 0)
    {
        int linesToRemove = (int)(m_topRemovePercentage * raw->getHeight());
        size_t bytesToBlack = linesToRemove * raw->getWidth()
            * raw->getNumChannels();
        memset(&(preprocess->asIplImage()->imageData[
                     preprocess->getWidth() * preprocess->getHeight() *
                     preprocess->getNumChannels() - bytesToBlack]),
               0, bytesToBlack);
    }

    if (m_rightRemovePercentage != 0)
    {
        size_t lineSize = raw->getWidth() * raw->getNumChannels();
        size_t bytesToBlack = (int)(m_rightRemovePercentage * lineSize);;
        for (size_t i = 0; i < raw->getHeight(); ++i)
        {
            size_t offset = i * lineSize - bytesToBlack;
            memset(preprocess->asIplImage()->imageData + offset,
                   0, bytesToBlack);
        }
    }

    if (m_leftRemovePercentage != 0)
    {
        size_t lineSize = raw->getWidth() * raw->getNumChannels();
        size_t bytesToBlack = (int)(m_leftRemovePercentage * lineSize);;
        for (size_t i = 0; i < raw->getHeight(); ++i)
        {
            size_t offset = i * lineSize;
            memset(preprocess->asIplImage()->imageData + offset,
                   0, bytesToBlack);
        }
    }
        
    // Process Image
    CvPoint boundUR = {0};
    CvPoint boundLL = {0};
    boundUR.x = 0;
    boundUR.y = 0;
    boundLL.x = 0;
    boundLL.y = 0;

    m_filter->filterImage(preprocess, filtered);

    BlobDetector::Blob greenBlob;
    bool foundBlob;
    if (m_useLineDetection) {
        m_lineDetector.processImage(filtered, analyzed);
        LineDetector::LineList lines = m_lineDetector.getLines();
        foundBlob = processLines(lines, greenBlob);
    } else {
        m_blobDetector.setMinimumBlobSize(minGreenPixels);
        m_blobDetector.processImage(filtered, analyzed);
        BlobDetector::BlobList blobs = m_blobDetector.getBlobs();
        foundBlob = processBlobs(blobs, greenBlob);
    }

    if (foundBlob)
    {
        /**
         * Constant for the camera to convert from 1 - width pixels
         * to a range in feet
         *
         * The second constant is the range of the hedge when seen at its
         * maximum size. Aka, how far away the hedge is when it's width spans
         * the entire screen.
         *
         * Both numbers are currently chosen to match the output of the
         * IdealSimVision until I can get real numbers to match it to feet.
         *
         * The hedge was width 0.65 when the range was 8.21. It was 3.18 when
         * the base was approximately 1. So these values can be found by
         *     8.21 = 0.65 * RMULT + 3.18
         */
        static const double RMULT = 7.73846;
        static const double RBASE = 3.18;
        boundUR.x = greenBlob.getMaxX();
        boundUR.y = greenBlob.getMaxY();
        boundLL.x = greenBlob.getMinX();
        boundLL.y = greenBlob.getMinY();
        
        // Base the center off the bounding box
        hedgeCenter.x = (boundUR.x + boundLL.x) / 2;
        hedgeCenter.y = (boundUR.y + boundLL.y) / 2;
        
        found=true;
        Detector::imageToAICoordinates(input, hedgeCenter.x, hedgeCenter.y,
                                       m_hedgeCenterX, m_hedgeCenterY);
        m_squareNess = greenBlob.getTrueAspectRatio();
        m_range = (1.0 - (((double)greenBlob.getWidth()) /
                          ((double)raw->getWidth()))) * RMULT + RBASE;
        
        publishFoundEvent();
    } else {
        if (found)
            publish(EventType::HEDGE_LOST, core::EventPtr(new core::Event()));

        found=false;
    }


    if (output)
    {
        switch (m_debug) {
        case 0:
            // Full information
            output->copyFrom(analyzed);
            output->setPixelFormat(vision::Image::PF_BGR_8);
            break;
        case 1:
            // Filtered image and preprocessed
            output->copyFrom(filtered);
            output->setPixelFormat(vision::Image::PF_BGR_8);
            break;
        case 2:
            // Preprocessed image only
            output->copyFrom(preprocess);
            break;
        case 3:
            // Raw image
            output->copyFrom(raw);
            break;
        }
        
        if (found)
        {
            // For convenience
            IplImage* img = output->asIplImage();
            // Debugging info
            CvPoint tl,tr,bl,br;
            tl.x = bl.x = std::max(hedgeCenter.x-4,0);
            tr.x = br.x = std::min(hedgeCenter.x+4,img->width-1);
            tl.y = tr.y = std::min(hedgeCenter.y+4,img->height-1);
            br.y = bl.y = std::max(hedgeCenter.y-4,0);
                        
            cvLine(img, tl, tr, CV_RGB(0,0,255), 3, CV_AA, 0 );
            cvLine(img, tl, bl, CV_RGB(0,0,255), 3, CV_AA, 0 );
            cvLine(img, tr, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
            cvLine(img, bl, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
            
            cvRectangle(img, boundUR, boundLL, CV_RGB(0,255,0), 2, CV_AA, 0);
        }
    }
}

void HedgeDetector::publishFoundEvent()
{
    if (found)
    {
        HedgeEventPtr event(new HedgeEvent());

        event->x = m_hedgeCenterX;
        event->y = m_hedgeCenterY;
        event->squareNess = m_squareNess;
        event->range = m_range;

        publish(EventType::HEDGE_FOUND, event);
    }
}

bool HedgeDetector::processLines(const LineDetector::LineList& lines,
                                 BlobDetector::Blob& outBlob)
{
    LineDetector::LineList hLines;
    LineDetector::LineList vLines;

    BlobDetector::BlobList highPriorityBlobs;
    BlobDetector::BlobList lowPriorityBlobs;

    // TODO: Move this to a configuration value
    double m_maxTilt = 15;
    int tolerance = 5; // 5 pixel tolerance
    math::Degree tilt(m_maxTilt);

    // Categorize every line as horizontal or vertical
    BOOST_FOREACH(LineDetector::Line line, lines)
    {
        math::Radian theta = line.theta();

        // Vertical line
        if (math::Radian(0) - tilt <= theta &&
            math::Radian(0) + tilt >= theta) {
            vLines.push_back(line);
        } else if (math::Radian(math::Math::PI/2) - tilt <= theta ||
                   math::Radian(-math::Math::PI/2) + tilt >= theta) {
            hLines.push_back(line);
        }
    }

    // Exit if there are no horizontal or vertical lines
    if (hLines.size() == 0 || vLines.size() == 0)
        return false;

    // Sort based on rho value
    std::sort(hLines.begin(), hLines.end(),
              LineDetector::RhoComparer::compare);
    std::sort(vLines.begin(), vLines.end(),
              LineDetector::RhoComparer::compare);

    for (size_t i=0; i<hLines.size(); i++) {
        // Treat this as the base line
        // Choose an arbitrary vertical line
        LineDetector::Line base = hLines[i];
        for (size_t j=0; j<vLines.size()-1; j++) {
            LineDetector::Line pipe1 = vLines[j];

            // Find a potential other pipe
            for (size_t k=j; k<vLines.size(); k++) {
                // If deemed invalid, we use a continue statement
                LineDetector::Line pipe2 = vLines[j];
                // Lines should be at roughly the same height
                int differenceY = abs(pipe2.getCenterY() - pipe1.getCenterY());
                if (differenceY > tolerance)
                    continue;

                // Center of the base should be roughly in
                // the center of these two pipes
                int centerX = (pipe2.getCenterX() + pipe1.getCenterX())/2;
                if (centerX < base.getCenterX() - tolerance ||
                    centerX > base.getCenterX() + tolerance)
                    continue;

                int centerY = (pipe2.getCenterY() + pipe1.getCenterY())/2;
                if (centerY >= base.getCenterY())
                    continue;

                // Passed, create a blob out of the three lines
                int minY = std::min(pipe1.getMinY(), pipe2.getMinY());
                int maxY = std::max(pipe1.getMaxY(), pipe2.getMaxY());
                maxY = std::max(maxY, base.getMaxY());
                int minX = std::min(pipe1.getMinX(), pipe2.getMinX());
                minX = std::min(minX, base.getMinX());
                int maxX = std::max(pipe1.getMaxX(), pipe2.getMaxX());
                maxX = std::max(maxX, base.getMaxX());

                BlobDetector::Blob blob(0, centerX, centerY, maxX,
                                        minX, maxY, minY);
                lowPriorityBlobs.push_back(blob);
            }
        }
    }

    if (highPriorityBlobs.size() > 0) {
        // Place the first high priority blob
        outBlob = highPriorityBlobs[0];
        return true;
    } else if (lowPriorityBlobs.size() > 0) {
        outBlob = lowPriorityBlobs[0];
        return true;
    } else {
        return false;
    }
}

bool HedgeDetector::processBlobs(const BlobDetector::BlobList& blobs,
                                 BlobDetector::Blob& outBlob)
{
    // Assumed sorted biggest to smallest by BlobDetector.
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Determine if we have actual found the light
        if (blob.getTrueAspectRatio() < m_maxAspectRatio &&
            blob.getTrueAspectRatio() > m_minAspectRatio)
        {
            // Found our blob record and leave
            outBlob = blob;
            return true;
        }
    }
    
    return false;
}

} // namespace vision
} // namespace ram
