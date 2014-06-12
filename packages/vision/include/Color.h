/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/Color.h
 */

#ifndef RAM_COLOR_H_03_04_2010
#define RAM_COLOR_H_03_04_2010

// STD Includes
#include <vector>
#include <string>

namespace ram {
namespace vision {

/** Defines color types */
struct Color
{
    enum ColorType {
        RED, ORANGE, YELLOW, GREEN, BLUE, UNKNOWN
    };

    /** A list of all color names */
    static std::vector<std::string> getColorNames();

    /** Turns the given color into a string, SymbolType::RED -> "RED" */
    static std::string colorToText(ColorType color);
};

} // namespace vision
} // namespace ram

#endif // RAM_COLOR_H_03_04_2010
