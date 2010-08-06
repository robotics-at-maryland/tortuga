/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/ImageFilter.h
 */

#ifndef RAM_IMAGE_FILTER_H_05_20_2010
#define RAM_IMAGE_FILTER_H_05_20_2010

// Project Includes
#include "vision/include/Image.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {

/**
 * Interface for filtering an image
 */
class RAM_EXPORT ImageFilter
{
public:
    virtual void filterImage(Image* input, Image* output = 0) = 0;
};

} // namespace vision
} // namespace ram

#endif // RAM_IMAGE_FILTER_H_05_20_2010
