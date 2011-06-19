/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/src/TableColorFilter.cpp
 */

// STD Includes
#include <iostream>

// Library Includes

// Project Includes
#include "vision/include/TableColorFilter.h"

namespace ram {
namespace vision {

TableColorFilter::TableColorFilter(core::ConfigNode config)
{

    init(config);
}

void TableColorFilter::init(core::ConfigNode config)
{
}

bool TableColorFilter::saveLookupTable(std::string filePath, bool relativePath)
{
    return true;
}

bool TableColorFilter::loadLookupTable(std::string filePath, bool relativePath)
{
    return true;
}

void TableColorFilter::createLookupTable()
{

}

void TableColorFilter::filterImage(Image* input, Image* output)
{

}

void TableColorFilter::inverseFilterImage(Image* input, Image* output)
{

}


} // namespace vision
} // namespace ram
