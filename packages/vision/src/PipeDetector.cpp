/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/PipeDetector.cpp
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
#include "vision/include/PipeDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"

#include "math/include/Vector2.h"

#include "core/include/PropertySet.h"

using namespace std;

namespace ram {
namespace vision {

PipeDetector::Pipe::Pipe() :
    TrackedBlob()
{
}
    
PipeDetector::Pipe::Pipe(BlobDetector::Blob blob, Image* sourceImage,
                         math::Degree angle, int id) :
    TrackedBlob(blob, sourceImage, angle, id)
{
}
        
/*void PipeDetector::Pipe::draw(Image* image)
{
    IplImage* out = image->asIplImage();
    // Draw green rectangle around the blob
    CvPoint tl,tr,bl,br;
    tl.x = bl.x = getMinX();
    tr.x = br.x = getMaxX();
    tl.y = tr.y = getMinY();
    bl.y = br.y = getMaxY();
    cvLine(out, tl, tr, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tl, bl, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tr, br, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, bl, br, CV_RGB(0,255,0), 3, CV_AA, 0);

    // Now draw my id
    std::stringstream ss;
    ss << getId();
    Image::writeText(image, ss.str(), tl.x, tl.y);

    std::stringstream ss2;
    ss2 << getAngle().valueDegrees();
    Image::writeText(image, ss2.str(), br.x-30, br.y-15);

    // Now do the suit
    if (Suit::NONEFOUND == m_suit)
        Image::writeText(image, "None", bl.x, bl.y - 15);
    else if (Suit::UNKNOWN == m_suit)
        Image::writeText(image, "Unknown", bl.x, bl.y - 15);
    else if (Suit::HEART == m_suit)
        Image::writeText(image, "Heart", bl.x, bl.y - 15);
    else if (Suit::DIAMOND == m_suit)
        Image::writeText(image, "Diamond", bl.x, bl.y - 15);
    else if (Suit::SPADE == m_suit)
        Image::writeText(image, "Spade", bl.x, bl.y - 15);
    else if (Suit::CLUB == m_suit)
        Image::writeText(image, "Club", bl.x, bl.y - 15);
        }*/

    
PipeDetector::PipeDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub,
                           int minPixels, int foundMinPixels) :
    Detector(eventHub),
    m_noHough(false)
{
    init(config, minPixels, foundMinPixels);
}
    
void PipeDetector::init(core::ConfigNode config, int minPixels,
                        int foundMinPixels)
{    
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "minPixels",
        "Minimum pixels for a blob to be considered a pipe",
        minPixels, &m_minPixels);
    propSet->addProperty(config, false, "minPixelsFound",
        "Minimum pixels for a blob, if we found the blob last frame",
        foundMinPixels, &m_minPixelsFound);

    propSet->addProperty(config, false, "noHough",
        "Use custom angle finder instead of hough",
        false, &m_noHough);
}

bool PipeDetector::found()
{
    return m_pipes.size() > 0;
}

PipeDetector::PipeList PipeDetector::getPipes()
{
    return m_pipes;
}

PipeDetector::~PipeDetector()
{
}

void PipeDetector::setHough(bool value)
{
    m_noHough = value;
}

void PipeDetector::processImage(Image* input, Image* output)
{
    // Find all blobs that could be pipes
    if (found())
        m_blobDetector.setMinimumBlobSize(m_minPixelsFound);
    else
        m_blobDetector.setMinimumBlobSize(m_minPixels);
    m_blobDetector.processImage(input);
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

    //if (output)
    //    output->copyFrom(input);
    
    // Determine the angle of the blobs
    PipeList candidatePipes;
    BOOST_FOREACH(BlobDetector::Blob pipeBlob, blobs)
    {
        math::Degree angle;
        if (findPipeAngle(pipeBlob, angle, input, output))
        {
            candidatePipes.push_back(Pipe(pipeBlob, input, angle, 0));
        }
        else
        {
            candidatePipes.push_back(Pipe(pipeBlob, input, math::Degree(0), 0));
            if (output)
                pipeBlob.draw(output, false);
        }
    }
    
    // Filter blobs for correct "pipeness"
    /// TODO: Implement this

    // Pack up all the blob information
    m_pipes = candidatePipes;
    
    // Draw debug information if requested
    if (output)
    {
        
        BOOST_FOREACH(Pipe pipe, m_pipes)
        {
            // Draw centroid
            CvPoint center;
            center.x = pipe.getCenterX();
            center.y = pipe.getCenterY();
            cvCircle(output->asIplImage(), center, 5, CV_RGB(0, 0, 255), -1);
            
            // Draw bounds (without centroid) and stats
            pipe.draw(output, false);
            pipe.drawStats(output);
        }
    }
}
    
bool PipeDetector::findPipeAngle(BlobDetector::Blob pipeBlob,
                                 math::Degree& outAngle,
                                 Image* input, Image* output)
{
    // Determine the size of the square we are going to draw and our blob 
    // finding thresholds
    int width = pipeBlob.getWidth();
    int height = pipeBlob.getHeight();
    int squareSize = width;
    if (height > squareSize)
        squareSize = height;

    // The two is added to deal with a little bit of round off which can make
    // the blacking out box, just a little bit too small.  This result in one
    // long blob instead of two seperate ones
    int drawWidth = std::min(squareSize / 2, width) + 2;
    int drawHeight = std::min(squareSize / 2, height) + 2;
    int minBlobSize = pipeBlob.getSize() / 8;
    
    // Draw square to eliminate the central area of the pipe
    math::Vector2 upperLeft(-drawWidth/2, drawHeight/2);
    math::Vector2 upperRight(drawWidth/2, drawHeight/2);
    math::Vector2 lowerLeft(-drawWidth/2, -drawHeight/2);
    math::Vector2 lowerRight(drawWidth/2, -drawHeight/2);
    
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
    cvFillConvexPoly(input->asIplImage(), pts, 4, CV_RGB(0,0,0));
    //if (output)
    //    cvFillConvexPoly(output->asIplImage(), pts, 4, CV_RGB(0,0,0));

    
    // Find all blobs inside the pipe blob
    m_blobDetector.setMinimumBlobSize(minBlobSize);
    m_blobDetector.processImage(input);
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
        double angle = atan2((double)(ydiff/(double)input->getHeight()),
                             (double)(xdiff/(double)input->getHeight()));
        
        // Make angle between 90 and -90
        math::Degree resultAngle(math::Radian(angle) + math::Degree(90));
        if (resultAngle > math::Radian(math::Math::HALF_PI))
            resultAngle -= math::Radian(math::Math::PI);
        outAngle = resultAngle;
        
        // If we are debugging draw up some debug data
        if (output)
        {
            IplImage* outImage = output->asIplImage();
            
            // Center of are start and end points
            CvPoint startPt;
            startPt.x = (int)start.x;
            startPt.y = (int)start.y;
            cvCircle(outImage, startPt, 5, CV_RGB(0, 255, 0), -1);
            CvPoint endPt;
            endPt.x = (int)end.x;
            endPt.y = (int)end.y;
            cvCircle(outImage, endPt, 5, CV_RGB(255, 0, 0), -1);
            
            // Line which connects the centroids
            cvLine(outImage, startPt, endPt, CV_RGB(255,0,255), 3, CV_AA, 0);
            
            // Bounds of blobs
            internalBlobs[0].draw(output, false);
            internalBlobs[1].draw(output, false);
        }
        
        return true;
    }
    
    return false;
}
    
} // namespace vision
} // namespace ram
