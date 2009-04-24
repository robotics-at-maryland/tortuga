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
#include <algorithm>

// Library Includes
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"

#include "math/include/Vector2.h"

#include "core/include/PropertySet.h"

using namespace std;

namespace ram {
namespace vision {

OrangePipeDetector::OrangePipeDetector(core::ConfigNode config,
                                       core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_cam(0),
    m_centered(false),
    m_noHough(false)
{
    init(config);
}
    
OrangePipeDetector::OrangePipeDetector(Camera* camera) :
    m_cam(camera),
    m_centered(false),
    m_noHough(false)
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
    
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "centeredLimit",
        "Max distance from the center for the pipe to be considered centered",
        0.1, &m_centeredLimit);

    propSet->addProperty(config, false, "minBrightness",
        "Minimum brighness for orange",
        100, &m_minBrightness);

    propSet->addProperty(config, false, "erodeIterations",
        "How many times to erode the filtered image",
        3, &m_erodeIterations);

    propSet->addProperty(config, false, "rOverGMin",
        "Red/Green minimum ratio", 1.0, &m_rOverGMin);
    propSet->addProperty(config, false, "rOverGMax",
        "Red/Green maximum ratio", 2.0, &m_rOverGMax);
    propSet->addProperty(config, false, "bOverRMax",
        "Blue/Red maximum ratio",  0.4, &m_bOverRMax);

    propSet->addProperty(config, false, "minPixels",
        "Minimum pixels for a blob to be considered a pipe",
        3000, &m_minPixels);
    propSet->addProperty(config, false, "minPixelsFound",
        "Minimum pixels for a blob, if we found the blob last frame",
        250, &m_minPixelsFound);

    propSet->addProperty(config, false, "noHough",
        "Use custom angle finder instead of hough",
        false, &m_noHough);
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

void OrangePipeDetector::setHough(bool value)
{
    m_noHough = value;
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
        int orange_count = mask_orange(image,true, m_minBrightness, true,
                                       m_rOverGMin, 
                                       m_rOverGMax, 
                                       m_bOverRMax);
        
        if (orange_count < m_minPixelsFound)
            pipeFound = false;
    }
    else
    {
        // Can't current see the pipe, find pixels with non-strict filtering
        int orange_count = mask_orange(image,true, m_minBrightness, false,
                                       m_rOverGMin, 
                                       m_rOverGMax, 
                                       m_bOverRMax);
         if (orange_count > m_minPixels)//this number is in pixels.
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

        // Don't run hough if we aren't using it
        double angle = HOUGH_ERROR;
        if (!m_noHough)
        {
            int dummy;
            angle = hough(image,&dummy,&dummy);
        }

        // Determine the centroid of the pipe
        int dummy; 
        histogram(image, &linex,&liney,&dummy,&dummy,&dummy,&dummy);

        if (angle==HOUGH_ERROR)
        {
            // Hough failed lets do this manually, first find all blobs
            m_blobDetector.setMinimumBlobSize(200);
            OpenCVImage temp(image, false);
            m_blobDetector.processImage(&temp);
            BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

            if (blobs.size() > 0) 
            {
                bool debug = output != 0;
                angle = findPipeAngle(blobs[0], image, debug);
            }
        }

        // If we have an error, just the angle to "zero", note that is really
        // -90 degrees because we add 90 degrees in the next step
        if (angle == HOUGH_ERROR)
            angle = -1.57079633;

        // Make angle between 90 and -90
        m_angle = math::Radian(angle) + math::Degree(90);
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
    
    if (output)
    {
        CvPoint center;
        center.x = linex;
        center.y = liney;
        cvCircle(image, center, 5, CV_RGB(0, 0, 255), -1);
        OpenCVImage temp(image, false);
        output->copyFrom(&temp);
    }
}

double OrangePipeDetector::findPipeAngle(BlobDetector::Blob pipeBlob, 
                                         IplImage* image, bool debug)
{
    // Determine the size of the square we are going to draw and our blob 
    // finding thresholds
    int width = pipeBlob.getMaxX() - pipeBlob.getMinX();
    int height = pipeBlob.getMaxY() - pipeBlob.getMinY();
    int squareSize = width;
    if (height > squareSize)
      squareSize = height;

    int drawSize = squareSize / 2;
    int minBlobSize = ((squareSize / 4) * (squareSize / 4)) / 4;

    // Draw square to eliminate the central area of the pipe
    math::Vector2 upperLeft(-drawSize/2, drawSize/2);
    math::Vector2 upperRight(drawSize/2, drawSize/2);
    math::Vector2 lowerLeft(-drawSize/2, -drawSize/2);
    math::Vector2 lowerRight(drawSize/2, -drawSize/2);

    CvPoint pts[4];
    pts[0].x = (int)upperLeft.x + pipeBlob.getCenterX();
    pts[0].y = (int)upperLeft.y + pipeBlob.getCenterY();
    pts[1].x = (int)upperRight.x + pipeBlob.getCenterX();
    pts[1].y = (int)upperRight.y + pipeBlob.getCenterY();
    pts[2].x = (int)lowerRight.x + pipeBlob.getCenterX();
    pts[2].y = (int)lowerRight.y + pipeBlob.getCenterY();
    pts[3].x = (int)lowerLeft.x + pipeBlob.getCenterX();
    pts[3].y = (int)lowerLeft.y + pipeBlob.getCenterY();

    // Finally lets draw the image
    cvFillConvexPoly(image, pts, 4, CV_RGB(0,0,0));

    // Find all blobs inside the pipe blob
    m_blobDetector.setMinimumBlobSize(minBlobSize);
    OpenCVImage temp(image, false);
    m_blobDetector.processImage(&temp);
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

    BlobDetector::BlobList internalBlobs;
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        if (pipeBlob.containsInclusive(blob))
            internalBlobs.push_back(blob);
    }

    if (internalBlobs.size() >= 2)
    {
        // Get the start and end coordinates from the blob centers      
        math::Vector2 start(internalBlobs[0].getCenterX(), 
                            internalBlobs[0].getCenterY());
        math::Vector2 end(internalBlobs[1].getCenterX(), 
                          internalBlobs[1].getCenterY());
        if (start.y > end.y)
            std::swap(start, end);

        // Determine the angle of the pipe
        int xdiff = (int)(end.x - start.x);    //deal with y's being flipped
        int ydiff = (int)(-1*(end.y - start.y));
        double angle = atan2((double)(ydiff/(double)image->height),
                             (double)(xdiff/(double)image->height));

        // If we are debugging draw up some debug data
        if (debug)
        {
            // Center of are start and end points
            CvPoint startPt;
            startPt.x = (int)start.x;
            startPt.y = (int)start.y;
            cvCircle(image, startPt, 5, CV_RGB(0, 255, 0), -1);
            CvPoint endPt;
            endPt.x = (int)end.x;
            endPt.y = (int)end.y;
            cvCircle(image, endPt, 5, CV_RGB(255, 0, 0), -1);
            
            // Line which connects the centroids
            cvLine(image, startPt, endPt, CV_RGB(255,0,255), 3, CV_AA, 0);
        
            // Bounds of blobs
            OpenCVImage temp(image, false);
            pipeBlob.draw(&temp, false);
            internalBlobs[0].draw(&temp, false);
            internalBlobs[1].draw(&temp, false);
        }
    
        return angle;
    }
    
    return HOUGH_ERROR;
}

} // namespace vision
} // namespace ram
