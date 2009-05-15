/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/BinDetector.cpp
 */

// STD Includes
#include <sstream>

// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/TrackedBlob.h"
#include "vision/include/Detector.h"
#include "vision/include/Image.h"

#include "math/include/Vector2.h"

namespace ram {
namespace vision {
    
TrackedBlob::TrackedBlob() :
    Blob(0, 0, 0, 0, 0, 0, 0),
    m_normX(0),
    m_normY(0),
    m_angle(math::Degree(0)),
    m_id(0)
{
}
    
TrackedBlob::TrackedBlob(BlobDetector::Blob blob, double x, double y,
                         math::Degree angle, int id) :
    Blob(blob),
    m_normX(x),
    m_normY(y),
    m_angle(angle),
    m_id(id)
{
}

TrackedBlob::TrackedBlob(BlobDetector::Blob blob, double x, double y, int id) :
    Blob(blob),
    m_normX(x),
    m_normY(y),
    m_angle(math::Degree(0)),
    m_id(id)
{
}

TrackedBlob::TrackedBlob(BlobDetector::Blob blob, Image* sourceImage,
                         math::Degree angle, int id) :
    Blob(blob),
    m_normX(0),
    m_normY(0),
    m_angle(angle),
    m_id(id)
{
    Detector::imageToAICoordinates(sourceImage, getCenterX(), getCenterY(),
                                   m_normX, m_normY);
}

TrackedBlob::TrackedBlob(BlobDetector::Blob blob, Image* sourceImage, int id) :
    Blob(blob),
    m_normX(0),
    m_normY(0),
    m_angle(math::Degree(0)),
    m_id(id)
{
    Detector::imageToAICoordinates(sourceImage, getCenterX(), getCenterY(),
                                   m_normX, m_normY);
}

    
double TrackedBlob::distanceTo(TrackedBlob& otherTrackedBlob)
{
    return distanceTo(otherTrackedBlob.getX(), otherTrackedBlob.getY());
}

double TrackedBlob::distanceTo(double x, double y)
{
    math::Vector2 mCenter(getX(), getY());
    math::Vector2 otherCenter(x, y);
    return (mCenter-otherCenter).length();
}
    
void TrackedBlob::drawStats(Image* image)
{
    // Draw the normal stats
    BlobDetector::Blob::drawStats(image);
    
    // Draw green rectangle around the blob
    CvPoint tl,tr,bl,br;
    tl.x = bl.x = getMinX();
    tr.x = br.x = getMaxX();
    tl.y = tr.y = getMinY();
    bl.y = br.y = getMaxY();

    // Now draw my id
    std::stringstream ss;
    ss << "Id:" << getId();
    Image::writeText(image, ss.str(), tl.x, tl.y);
}

} // namespace vision
} // namespace ram
