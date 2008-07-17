/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/OrangePipeDetector.cpp
 */

// STD Includes
#include <iostream>

// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"

#include "math/include/Vector2.h"

using namespace std;

namespace ram {
namespace vision {

OrangePipeDetector::OrangePipeDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_cam(0),
    m_centered(false)
{
    init(config);
}
    
OrangePipeDetector::OrangePipeDetector(Camera* camera) :
    m_cam(camera),
    m_centered(false)
{
    init(core::ConfigNode::fromString("{}"));
}

void OrangePipeDetector::init(core::ConfigNode config)
{
    m_angle = math::Degree(0);
    m_lineX = 0;
    m_lineY = 0;
    m_found = false;
    m_frame = new OpenCVImage(640, 480);
    m_rotated = cvCreateImage(cvSize(640,480),8,3);//480 by 640 if camera is on sideways, else 640 by 480.
    m_lineX=m_lineY=0;
    
    m_centeredLimit = config["centeredLimit"].asDouble(0.1);
    m_minBrightness = config["minBrightness"].asInt(100);
    m_erodeIterations = config["erodeIterations"].asInt(3);
}

bool OrangePipeDetector::found()
{
    return m_found;
}

double OrangePipeDetector::getX()
{
    return m_lineX;
}

double OrangePipeDetector::getY()
{
    return m_lineY;
}

math::Degree OrangePipeDetector::getAngle()
{
    return m_angle;
}

OrangePipeDetector::~OrangePipeDetector()
{
    delete m_frame;
    cvReleaseImage(&m_rotated);
}

void OrangePipeDetector::show(char* window)
{
    cvShowImage(window,m_rotated);
}

IplImage* OrangePipeDetector::getAnalyzedImage()
{
    return (IplImage*)(m_rotated);
}

void OrangePipeDetector::update()
{
    m_cam->getImage(m_frame);
    processImage(m_frame, 0);
}

void OrangePipeDetector::processImage(Image* input, Image* output)
{
    //Plan is:  Search out orange with a strict orange filter, as soon as we
    // see a good deal of orange use a less strict filter, and reduce the
    // amount we need to see. In theory this makes us follow the pipeline as
    // long as possible, but be reluctant to follow some arbitrary blob until
    // we know its of a large size and the right shade of orange.  If the
    // pipeline is found, the angle found by hough is reported.  
    
    // Mask orange takes frame, then alter image, then strictness (true=more
    // strict, false=more lenient)
    IplImage* image =(IplImage*)(*input);

//    rotate90Deg(image,m_rotated);//Only do this if the camera is attached sideways again.
    cvCopyImage(image,m_rotated);//Poorly named if the cameras not on sideways... oh well.
    image=m_rotated;
    

    bool pipeFound = m_found;
    if (pipeFound)
    {
        //int left_or_right=guess_line(image);
        // Left is negative, right is positive, magnitude is num pixels from
        // center line

        // Can see the pipe, find pixels with a strict filtering
        int orange_count = mask_orange(image,true, m_minBrightness, true);
        
        if (orange_count < 250)
            pipeFound = false;
    }
    else
    {
        // Can't current see the pipe, find pixels with non-strict filtering
        int orange_count = mask_orange(image,true, m_minBrightness, false);
        if (orange_count > 1000)//this number is in pixels.
            pipeFound = true;
    }
    
    // We use to see the pipe, but now can't
    if (!pipeFound && m_found)
    {
        publish(EventType::PIPE_LOST,
                core::EventPtr(new core::Event()));
    }
    
    m_found = pipeFound;

    
    int linex,liney;
    if (m_found)
    {
        // 3 x 3 default erosion element, default 3 iterations.
        cvErode(image, image, 0, m_erodeIterations);
        
        double angle = hough(image,&linex,&liney);
        if (angle==HOUGH_ERROR)
        {
            m_lineX=-1;
            m_lineY=-1;
        }
        else
        {
            int dummy;
            histogram(image, &linex,&liney,&dummy,&dummy,&dummy,&dummy);//uh oh.
            // Make angle between 90 and -90
            m_angle = math::Radian(angle);
            if (m_angle > math::Radian(math::Math::HALF_PI))
                m_angle = m_angle - math::Radian(math::Math::PI);

            // m_lineX and m_lineY are fields, both are doubles
            // Shift origin to the center
            m_lineX = -1 * ((image->width / 2) - linex);
            m_lineY = (image->height / 2) - liney;
    
            // Normalize     (-1 to 1)
            m_lineX = m_lineX / ((double)(image->width)) * 2.0;
            m_lineY = m_lineY / ((double)(image->height)) * 2.0;

            // Account for the aspect ratio difference
            // 640/480      
            m_lineX *= (double)image->width / image->height;

            // Fire off found event
            PipeEventPtr event(new PipeEvent(0, 0, 0));
            event->x = m_lineX;
            event->y = m_lineY;
            event->angle = m_angle;
            publish(EventType::PIPE_FOUND, event);

            // Determine Centered
            math::Vector2 toCenter(m_lineX, m_lineY);
            if (toCenter.normalise() < m_centeredLimit)
            {
                if(!m_centered)
                {
                    m_centered = true;
                    publish(EventType::PIPE_CENTERED, event);
                }
            }
            else
            {
                m_centered = false;
            }
        }
        
    }
    
    if (output)
    {
        CvPoint center;
        center.x = linex;
        center.y = liney;
        cvCircle(image, center, 5, CV_RGB(0, 255, 0), -1);
        OpenCVImage temp(image, false);
        output->copyFrom(&temp);
    }
}

} // namespace vision
} // namespace ram
