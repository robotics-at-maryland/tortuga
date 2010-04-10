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

// Library Includes
#include "highgui.h"
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
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

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);

    // State machine variables
    found=false;
    m_hedgeCenterX=0;
    m_hedgeCenterY=0;
    m_hedgeWidth=0;
    hedgeCenter.x=0;
    hedgeCenter.y=0;

    // Working images
    raw = new vision::OpenCVImage(640, 480);
    preprocess = new vision::OpenCVImage(640, 480);
    filtered = new vision::OpenCVImage(640, 480, vision::Image::PF_GRAY_8);
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

    m_lineDetector.processImage(filtered, analyzed);

    LineDetector::LineList lines = m_lineDetector.getLines();

    if (lines.size() > 0)
    {
        LineDetector::Line greenLine;

        // Attempt to find a valid blob
        if (processLines(lines, greenLine))
        {
            boundUR.x = greenLine.getMaxX();
            boundUR.y = greenLine.getMaxY();
            boundLL.x = greenLine.getMinX();
            boundLL.y = greenLine.getMinY();

            // Base the center off the bounding box
            hedgeCenter.x = (boundUR.x + boundLL.x) / 2;
            hedgeCenter.y = (boundUR.y + boundLL.y) / 2;

            // Width
            int pixelWidth = boundUR.x - boundLL.x;
            m_hedgeWidth = pixelWidth / 640.0;

            found=true;
            Detector::imageToAICoordinates(input, hedgeCenter.x, hedgeCenter.y,
                                           m_hedgeCenterX, m_hedgeCenterY);

            publishFoundEvent();
        }
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

            // clamp values
            //lightCenter.x = std::min(lightCenter.x, img->width-1);
            //lightCenter.x = std::max(lightCenter.x, 0);
            //lightCenter.y = std::min(lightCenter.y, img->height-1);
            //lightCenter.y = std::max(lightCenter.y, 0);
            //int radius = std::max((int)sqrt((double)redPixelCount/M_PI), 1);
         
            //cvCircle(img, hedgeCenter, radius, CV_RGB(0,255,0), 2, CV_AA, 0);
        }
    }
}

void HedgeDetector::publishFoundEvent()
{
    if (found)
    {
        HedgeEventPtr event(new HedgeEvent(0, 0, 0));

        event->x = m_hedgeCenterX;
        event->y = m_hedgeCenterY;
        event->width = m_hedgeWidth;

        publish(EventType::HEDGE_FOUND, event);
    }
}

bool HedgeDetector::processLines(const LineDetector::LineList& lines,
                                 LineDetector::Line& outLine)
{
    // TODO: Get aspect ratio working
    // Copy from RedLightDetector

    // TODO: Do stuff...

    return false;
}

} // namespace vision
} // namespace ram
