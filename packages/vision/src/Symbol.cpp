/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Symbol.cpp
 */

// Library Includes
#include "boost/assign/list_of.hpp"

// Project Includes
#include "vision/include/Symbol.h"

namespace ram {
namespace vision {

std::vector<std::string> Symbol::getSymbolNames()
{
    static const std::vector<std::string> symbolNames =
        boost::assign::list_of("CLUB")("CLUBR90")("CLUBR180")("CLUBR270")
        ("SPADE")("SPADER90")("SPADER180")("SPADER270")
        ("HEART")("HEARTR90")("HEARTR180")("HEARTR270")
        ("DIAMOND")("DIAMONDR90")("DIAMONDR180")("DIAMONDR270")
        ("SHIP")("AIRCRAFT")("TANK")("FACTORY")
        ("AXE")("CLIPPERS")("HAMMER")("MACHETE")("LARGE_X")("SMALL_X")
        ("LARGE_O")("SMALL_O")
        ("NET")("SHIELD")("SWORD")("TRIDENT")
        ("UNKNOWN")("NONEFOUND")("BIN37")("BIN10")("BIN16")("BIN98")("BINOUTLINE");
    return symbolNames;
}
    
std::string Symbol::symbolToText(SymbolType symbol)
{
    static const std::vector<std::string> symbolNames = getSymbolNames();
    std::string name("ERROR");
    if (((size_t)symbol) < symbolNames.size())
        name = symbolNames[(size_t)symbol];
            
    return name;
}

} // namespace vision
} // namespace ram
