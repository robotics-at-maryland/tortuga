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
#include "vision/include/SegmentationFilter.h"
#include "vision/include/WindowDetector.h"

namespace ram {
namespace vision {

WindowDetector::WindowDetector(core::ConfigNode config,
                               core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_filter(0),
    m_blobDetector(config, eventHub)
{
    init(config);
}

WindowDetector::WindowDetector(Camera* camera) :
    cam(camera),
    m_filter(0)
{
    init(core::ConfigNode::fromString("{}"));
}

WindowDetector::~WindowDetector()
{
    delete m_filter;

    delete frame;
    delete filtered;
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

    m_filter = new SegmentationFilter(0.5, 500, 50);
    m_filter->addPropertiesToSet(propSet, &config);

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);
    
    // State machine variables
    m_topLeftFound = false;
    m_bottomLeftFound = false;
    m_topRightFound = false;
    m_bottomRightFound = false;

    // Working images
    frame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    filtered = new OpenCVImage(640, 480, Image::PF_BGR_8);

    // Binary image needs compatibility with BlobDetector
    binary = new OpenCVImage(640, 480, Image::PF_BGR_8);
}

void WindowDetector::update()
{
    cam->getImage(frame);
    processImage(frame);
}

static bool scalarCompare (CvScalar s1, CvScalar s2)
{
    if (s1.val[0] == s2.val[0]) {
        if (s1.val[1] == s2.val[1]) {
            if (s1.val[2] == s2.val[2]) {
                return s1.val[3] < s2.val[3];
            } else {
                return s1.val[2] < s2.val[2];
            }
        } else {
            return s1.val[1] < s2.val[1];
        }
    } else {
        return s1.val[0] < s2.val[0];
    }
};

void WindowDetector::filterBlobs(WindowDetector::ScalarMap& blobMap,
                                 BlobDetector::BlobList& blobs)
{
    BOOST_FOREACH(const WindowDetector::ScalarMap::value_type& p, blobMap)
    {
        int centerX = (int) (p.second.maxX - p.second.minX) / 2 + p.second.minX;
        int centerY = (int) (p.second.maxY - p.second.minY) / 2 + p.second.minY;

        BlobDetector::Blob blob(p.second.pixels, centerX, centerY,
                                p.second.maxX, p.second.minX,
                                p.second.maxY, p.second.minY);

        // Checks for aspect ratio, width, height, and that the middle pixel
        // is not the same as the image color (filters out middle box)
        double aspectRatio = 1.0 / blob.getTrueAspectRatio();
        unsigned char* data = filtered->getData() +
            (centerX + centerY * filtered->getWidth()) * 3;
        if (blob.getFillPercentage() > m_minPixelPercentage &&
            aspectRatio <= m_maxAspectRatio &&
            aspectRatio >= m_minAspectRatio &&
            blob.getWidth() >= m_minWidth &&
            blob.getHeight() >= m_minHeight &&
            data[0] != p.first.val[0] &&
            data[1] != p.first.val[1] &&
            data[2] != p.first.val[2])
        {
            blobs.push_back(blob);
        }
    }    
}

void WindowDetector::closestBlob(BlobDetector::BlobList& blobs,
                                 const BlobDetector::Blob& oldBlob,
                                 BlobDetector::Blob **outBlob)
{
    int index = -1;
    double min_distance = HUGE_VAL;
    for (size_t i = 0; i < blobs.size(); i++) {
        int x1 = blobs[i].getCenterX();
        int y1 = blobs[i].getCenterY();
        int x2 = oldBlob.getCenterX();
        int y2 = oldBlob.getCenterY();

        double dist = (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
        if (dist < min_distance && dist < m_maxDistance) {
            min_distance = dist;
            index = i;
        }
    }

    if (index != -1) {
        *outBlob = &blobs[index];
    }
}

void WindowDetector::extrapolate3(BlobDetector::BlobList& blobs,
                                  BlobDetector::Blob *topLeft,
                                  BlobDetector::Blob *topRight,
                                  BlobDetector::Blob *bottomLeft,
                                  BlobDetector::Blob *bottomRight)
{
    // Base the method on the missing blob
    if (!topLeft) {
        // Fake blob with x position of bottomLeft, y position of topRight
        BlobDetector::Blob fakeBlob(0, bottomLeft->getCenterX(),
                                    topRight->getCenterY(),
                                    0, 0, 0, 0);
        closestBlob(blobs, fakeBlob, &topLeft);
    } else if (!topRight) {
        // Fake blob with x position of bottomRight, y position of topLeft
        BlobDetector::Blob fakeBlob(0, bottomRight->getCenterX(),
                                    topLeft->getCenterY(),
                                    0, 0, 0, 0);
        closestBlob(blobs, fakeBlob, &topRight);
    } else if (!bottomLeft) {
        // Fake blob with x position of topLeft, y position of bottomRight
        BlobDetector::Blob fakeBlob(0, topLeft->getCenterX(),
                                    bottomRight->getCenterY(),
                                    0, 0, 0, 0);
        closestBlob(blobs, fakeBlob, &bottomLeft);
    } else if (!bottomRight) {
        // Fake blob with x position of topRight, y position of bottomLeft
        BlobDetector::Blob fakeBlob(0, topRight->getCenterX(),
                                    bottomLeft->getCenterY(),
                                    0, 0, 0, 0);
        closestBlob(blobs, fakeBlob, &bottomRight);
    } else {
        assert(false && "Could not find a missing blob");
    }
}

void WindowDetector::extrapolatePositions(BlobDetector::BlobList& blobs,
                                          BlobDetector::Blob *topLeft,
                                          BlobDetector::Blob *topRight,
                                          BlobDetector::Blob *bottomLeft,
                                          BlobDetector::Blob *bottomRight)
{
    int foundBlobs = !!topLeft + !!topRight + !!bottomLeft + !!bottomRight;

    switch (foundBlobs) {
    case 3:
        extrapolate3(blobs, topLeft, topRight, bottomLeft, bottomRight);
        break;
    case 2:
        break;
    case 1:
        break;
    case 0:
        // Cannot extrapolate any positions
        break;
    }
}

void WindowDetector::processImage(Image* input, Image* output)
{
    frame->copyFrom(input);
    filtered->copyFrom(frame);
    m_filter->filterImage(filtered);

    WindowDetector::ScalarMap colorMap (scalarCompare);
    size_t size = filtered->getWidth() * filtered->getHeight() * 3;
    CvScalar current = cvScalar(0, 0, 0, 0);
    unsigned char* data = filtered->getData();
    size_t x = 0, y = 0;
    
    for (size_t i=0; i < size; i += 3) {
        size_t vsize = colorMap.size();
        current.val[0] = data[i];
        current.val[1] = data[i+1];
        current.val[2] = data[i+2];

        Area* area = &colorMap[current];
        area->pixels++;

        if (vsize != colorMap.size()) {
            // New color
            area->minX = x;
            area->maxX = x;
            area->minY = y;
            area->maxY = y;
            area->pixels = 1;
        } else {
            // Find the new min/max
            if (x < area->minX)
                area->minX = x;
            else if (x > area->maxX)
                area->maxX = x;

            if (y < area->minY)
                area->minY = y;
            else if (y > area->maxY)
                area->maxY = y;

            area->pixels++;
        }

        // Move one pixel right
        x++;
        if (x >= filtered->getWidth()) {
            // Move down one line when reaching the end of line
            y++;
            x = 0;
        }
    }

    // Filter out blobs without enough pixels and incorrect aspect ratios
    BlobDetector::BlobList blobList;
    filterBlobs(colorMap, blobList);

    std::sort(blobList.begin(), blobList.end(),
              BlobDetector::BlobComparer::compare);

    // Check if we can find a left, top, right, or left blob
    BlobDetector::Blob *TLBlob = 0, *TRBlob = 0, *BLBlob = 0, *BRBlob = 0;

    // If one of the windows was found previously, find them based on that
    if (m_topLeftFound || m_bottomLeftFound ||
        m_topRightFound || m_bottomRightFound)
    {
        // Find the closest blob to every previously found point
        if (m_topLeftFound) {
            closestBlob(blobList, m_topLeftWindow, &TLBlob);
        }

        if (m_topRightFound) {
            closestBlob(blobList, m_topRightWindow, &TRBlob);
        }

        if (m_bottomLeftFound) {
            closestBlob(blobList, m_bottomLeftWindow, &BLBlob);
        }

        if (m_bottomRightFound) {
            closestBlob(blobList, m_bottomRightWindow, &BRBlob);
        }

        extrapolatePositions(blobList, TLBlob, TRBlob, BLBlob, BRBlob);
    }
    else
    {    
        // Search for windows
        // Only look at the 4 biggest blobs
        for (size_t i = 0; i < std::min(4u, blobList.size()); i++)
        {
            // Find the two blobs with minimum x's
            if (!TLBlob) {
                TLBlob = &blobList[i];
            } else if (!BLBlob) {
                // Check if this new blob should be the top blob
                if (blobList[i].getCenterY() < TLBlob->getCenterY()) {
                    BLBlob = TLBlob;
                    TLBlob = &blobList[i];
                } else {
                    BLBlob = &blobList[i];
                }
            } else {
                // Check if this x value is less than either of the above
                if (blobList[i].getCenterX() < std::max(TLBlob->getCenterX(),
                                                        BLBlob->getCenterX())) {
                    // Find the one to replace
                    if (TLBlob->getCenterX() < BLBlob->getCenterX()) {
                        // Replace BLBlob
                        BLBlob = &blobList[i];
                    } else {
                        TLBlob = &blobList[i];
                    }

                    // Check Y values and reorder if necessary
                    if (BLBlob->getCenterY() < TLBlob->getCenterY()) {
                        BlobDetector::Blob *tmp = BLBlob;
                        BLBlob = TLBlob;
                        TLBlob = tmp;
                    }
                }
            }
        }

        // TODO: Check if the X values are similar (false positive otherwise)
        // Find the two remaining blobs and assign them as TR and BR
        for (size_t i = 0; i < std::min(4u, blobList.size()); i++)
        {
            // Check pointer values
            if (&blobList[i] != TLBlob && &blobList[i] != BLBlob) {
                if (!TRBlob) {
                    TRBlob = &blobList[i];
                } else if (!BRBlob) {
                    if (blobList[i].getCenterY() < TRBlob->getCenterY()) {
                        BRBlob = TRBlob;
                        TRBlob = &blobList[i];
                    } else {
                        BRBlob = &blobList[i];
                    }
                } else {
                    // Should never reach here...
                    assert(false && "Too many blobs");
                }
            }
        }
    }

    // Publish events
    // TODO: Change colors to be based on configuration
    if (TLBlob) {
        publishFoundEvent(*TLBlob, Color::RED);
    } else {
        if (m_topLeftFound) {
            publishLostEvent(Color::RED);
        }
    }

    if (TRBlob) {
        publishFoundEvent(*TRBlob, Color::GREEN);
    } else {
        if (m_topRightFound) {
            publishLostEvent(Color::GREEN);
        }
    }

    if (BLBlob) {
        publishFoundEvent(*BLBlob, Color::YELLOW);
    } else {
        if (m_topLeftFound) {
            publishLostEvent(Color::YELLOW);
        }
    }

    if (BRBlob) {
        publishFoundEvent(*BRBlob, Color::BLUE);
    } else {
        if (m_topRightFound) {
            publishLostEvent(Color::BLUE);
        }
    }

    if (output)
    {
        if (m_debug == 0) {
            output->copyFrom(frame);
        } else {
            output->copyFrom(filtered);
            
            if (m_debug == 2) {
                BOOST_FOREACH(BlobDetector::Blob blob, blobList)
                {
                    blob.draw(output);
                    blob.drawStats(output);
                }

                if (TLBlob) {
                    CvPoint center;
                    center.x = TLBlob->getCenterX();
                    center.y = TLBlob->getCenterY();

                    cvCircle(output->asIplImage(), center, 3, cvScalar(0, 0, 255), -1);
                }

                if (TRBlob) {
                    CvPoint center;
                    center.x = TRBlob->getCenterX();
                    center.y = TRBlob->getCenterY();

                    cvCircle(output->asIplImage(), center, 3, cvScalar(255, 0, 0), -1);
                }

                if (BLBlob) {
                    CvPoint center;
                    center.x = BLBlob->getCenterX();
                    center.y = BLBlob->getCenterY();

                    // Yellow (Red + Green)
                    cvCircle(output->asIplImage(), center, 3, cvScalar(0, 255, 255), -1);
                }

                if (BRBlob) {
                    CvPoint center;
                    center.x = BRBlob->getCenterX();
                    center.y = BRBlob->getCenterY();

                    // Turquoise (Blue + Green)
                    cvCircle(output->asIplImage(), center, 3, cvScalar(255, 255, 0), -1);
                }
            }
        }
    }
}

void WindowDetector::show(char* window)
{
    vision::Image::showImage(filtered, "WindowDetector");
}

IplImage* WindowDetector::getAnalyzedImage()
{
    return filtered->asIplImage();
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
