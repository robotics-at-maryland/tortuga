/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/SegmentationFilter.cpp
 */

// STD Includes
#include <string>
#include <sstream>

// Library Includes
#include <boost/bind.hpp>
#include "cv.h"
#include "segment-image.h" // Graph Based Segmentation

// Project Includes
#include "vision/include/SegmentationFilter.h"

namespace ram {
namespace vision {

SegmentationFilter::SegmentationFilter(double sigma, double k, int min) :
    m_sigma(sigma),
    m_k(k),
    m_min(min),
    m_num_ccs(0)
{
}

SegmentationFilter::~SegmentationFilter()
{
}

void SegmentationFilter::filterImage(Image* input, Image* output)
{
    // Pack image into their data format
    image<rgb> segment_image(input->getWidth(), input->getHeight(), false);
    int size = segment_image.width() * segment_image.height();
    unsigned char* dataPtr = (unsigned char*) input->asIplImage()->imageData;
    for (int i = 0; i < size; i++) {
        // Copy image points from the original image into their image format
        rgb* data = segment_image.data+i;
        data->r = dataPtr[2];
        data->g = dataPtr[1];
        data->b = dataPtr[0];

        // Advance image pointer for input
        dataPtr += 3;
    }
    

    // Run their algorithm on this image
    image<rgb> *seg = ::segment_image(&segment_image, m_sigma,
                                      m_k, m_min, &m_num_ccs);

    // Extract received image and place it back into input
    dataPtr = (unsigned char*) input->asIplImage()->imageData;
    for (int i = 0; i < size; i++) {
        rgb* data = seg->data+i;
        dataPtr[2] = data->r;
        dataPtr[1] = data->g;
        dataPtr[0] = data->b;

        // Advance data pointer
        dataPtr += 3;
    }

    // Clean up
    delete seg;
}

void SegmentationFilter::addPropertiesToSet(
    core::PropertySetPtr propSet, core::ConfigNode* config)
{
    propSet->addProperty(*config, false,
                         "segmentSigma",
                         "The sigma value in Graph-Based Segmentation",
                         0.5,
                         boost::bind(&SegmentationFilter::getSigma, this),
                         boost::bind(&SegmentationFilter::setSigma, this, _1));

    propSet->addProperty(*config, false,
                         "segmentK",
                         "The k value in Graph-Based Segmentation",
                         500.0,
                         boost::bind(&SegmentationFilter::getK, this),
                         boost::bind(&SegmentationFilter::setK, this, _1));
    
    propSet->addProperty(*config, false,
                         "segmentMin",
                         "The min value in Graph-Based Segmentation",
                         50,
                         boost::bind(&SegmentationFilter::getMin, this),
                         boost::bind(&SegmentationFilter::setMin, this, _1));
}

} // namespace vision
} // namespace ram
