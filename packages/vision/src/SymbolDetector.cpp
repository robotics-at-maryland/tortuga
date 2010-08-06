/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/SymbolDetector.cpp
 */


// Project Includes
#include "vision/include/SymbolDetector.h"

namespace ram {
namespace vision {

SymbolDetector::SymbolDetector(core::EventHubPtr eventHub) :
    Detector(eventHub)
{
}
    
} // namespace vision
} // namespace ram

