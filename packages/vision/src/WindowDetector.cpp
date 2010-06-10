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
                         1.0, &m_maxAspectRatio);

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

    m_filter = new SegmentationFilter(0.5, 500, 50);
    m_filter->addPropertiesToSet(propSet, &config);

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);
    
    // State machine variables
    found = false;

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

struct Area {
    size_t minX;
    size_t maxX;
    size_t minY;
    size_t maxY;
    size_t pixels;

    Area() :
        minX(0),
        maxX(0),
        minY(0),
        maxY(0),
        pixels(0)
    {
    }
};

void WindowDetector::processImage(Image* input, Image* output)
{
    typedef std::map<CvScalar, Area, bool(*)(CvScalar,CvScalar)> map_type;

    frame->copyFrom(input);
    filtered->copyFrom(frame);
    m_filter->filterImage(filtered);

    std::map<CvScalar,Area,bool(*)(CvScalar,CvScalar)> colorMap (scalarCompare);
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
    std::vector<BlobDetector::Blob> blobList;
    BOOST_FOREACH(const map_type::value_type& p, colorMap)
    {
        int centerX = (int) (p.second.maxX - p.second.minX) / 2 + p.second.minX;
        int centerY = (int) (p.second.maxY - p.second.minY) / 2 + p.second.minY;

        BlobDetector::Blob blob(p.second.pixels, centerX, centerY,
                                p.second.maxX, p.second.minX,
                                p.second.maxY, p.second.minY);

        double aspectRatio = 1.0 / blob.getTrueAspectRatio();
        if (blob.getFillPercentage() > m_minPixelPercentage &&
            aspectRatio < m_maxAspectRatio &&
            aspectRatio > m_minAspectRatio &&
            blob.getWidth() > m_minWidth &&
            blob.getHeight() > m_minHeight)
        {
            blobList.push_back(blob);
        }
    }

    std::sort(blobList.begin(), blobList.end(),
              BlobDetector::BlobComparer::compare);

    // Check if we can find a left, top, right, or left blob
    BlobDetector::Blob *TLBlob = 0, *TRBlob = 0, *BLBlob = 0, *BRBlob = 0;
    
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

void WindowDetector::publishFoundEvent(int x, int y, Color::ColorType color)
{
}

} // namespace vision
} // namespace ram
