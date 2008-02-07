/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/Detector.cpp
 */


// Project Includes
#include "vision/include/Detector.h"

namespace ram {
namespace vision {

Detector::Detector(core::EventHubPtr eventHub) :
    core::EventPublisher(eventHub)
{
}

} // namespace vision
} // namespace ram

