/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/Color.h
 */

// Library Includes
#include "boost/assign/list_of.hpp"

// Project Includes
#include "vision/include/Color.h"

namespace ram {
namespace vision {

std::vector<std::string> Color::getColorNames()
{
    static const std::vector<std::string> colorNames =
        boost::assign::list_of("RED")("ORANGE")("YELLOW")
        ("GREEN")("BLUE")("UNKNOWN");
    return colorNames;
}

std::string Color::colorToText(ColorType color)
{
    static const std::vector<std::string> colorNames = getColorNames();
    std::string name("ERROR");
    if (((size_t)color) < colorNames.size())
        name = colorNames[(size_t)color];

    return name;
}

} // namespace vision
} // namespace ram
