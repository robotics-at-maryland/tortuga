/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/Symbol.h
 */

#ifndef RAM_SYMBOL_H_07_08_2008
#define RAM_SYMBOL_H_07_08_2008

namespace ram {
namespace vision {

/** Defines normal and rotated symbol types */
struct Symbol
{
    enum SymbolType {
        CLUB, CLUBR90, CLUBR180, CLUBR270, 
        SPADE, SPADER90, SPADER180, SPADER270, 
        HEART, HEARTR90, HEARTR180, HEARTR270, 
        DIAMOND, DIAMONDR90, DIAMONDR180, DIAMONDR270, 
        UNKNOWN, NONEFOUND
    };
};

} // namespace vision
} // namespace ram
    
#endif // RAM_SYMBOL_H_07_08_2008
