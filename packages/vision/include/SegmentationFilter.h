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

// STD Includes
#include <string>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/PropertySet.h"
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

    void addPropertiesToSet(core::PropertySetPtr propSet,
                            core::ConfigNode* config);

    float getSigma() { return m_sigma; }
    void setSigma(float sigma) { m_sigma = sigma; }

    float getK() { return m_k; }
    void setK(float k) { m_k = k; }

    int getMin() { return m_min; }
    void setMin(int min) { m_min = min; }

    int getNumColors() { return m_num_ccs; }

private:
    float m_sigma;
    float m_k;
    int m_min;
    int m_num_ccs;
};

} // namespace vision
} // namespace ram

#endif // RAM_SEGMENTATION_FILTER_H_05_20_2010
