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

// STD Includes
#include <vector>
#include <string>

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
        SHIP, AIRCRAFT, TANK, FACTORY,
        AXE, CLIPPERS, HAMMER, MACHETE,
        LARGE_X, SMALL_X, LARGE_O, SMALL_O,
        NET, SHIELD, SWORD, TRIDENT,
        UNKNOWN, NONEFOUND, BIN37,BIN10,BIN16,BIN98,BINOUTLINE
    };

    /** A list of all symbol names */
    static std::vector<std::string> getSymbolNames();
    
    /** Turns the given symbol into a string, SymbolType::CLUB -> "CLUB" */
    static std::string symbolToText(SymbolType symbol);
};

} // namespace vision
} // namespace ram
    
#endif // RAM_SYMBOL_H_07_08_2008
