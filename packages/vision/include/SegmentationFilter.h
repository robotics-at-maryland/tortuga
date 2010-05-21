/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/SegmentationFilter.h
 */

#ifndef RAM_SEGMENTATION_FILTER_H_05_20_2010
#define RAM_SEGMENTATION_FILTER_H_05_20_2010

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/ImageFilter.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/**
 * Provides a frontend interface to work with image segmentation code
 */
class RAM_EXPORT SegmentationFilter : public ImageFilter
{
public:
    SegmentationFilter(double sigma, double k, int min);

    virtual ~SegmentationFilter();

    virtual void filterImage(Image* input, Image* output = 0);

private:
    float m_sigma;
    float m_k;
    int m_min;
    int m_num_ccs;
};

} // namespace vision
} // namespace ram

#endif // RAM_SEGMENTATION_FILTER_H_05_20_2010
