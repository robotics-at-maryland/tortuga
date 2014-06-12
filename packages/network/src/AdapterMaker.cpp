/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/src/AdapterMaker.cpp
 */

// Project Includes
#include "network/include/AdapterMaker.h"

namespace ram {
namespace pattern {

template<>
network::AdapterMaker::MakerMap* network::AdapterMaker::getRegistry()
{
    static network::AdapterMaker::MakerMap* reg =
        new network::AdapterMaker::MakerMap();
    return reg;
}

} // namespace pattern
} // namespace ram
