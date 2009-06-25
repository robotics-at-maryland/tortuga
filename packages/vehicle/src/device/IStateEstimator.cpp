/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/device/IStateEstimator.cpp
 */

// Project Includes
#include "vehicle/include/device/IStateEstimator.h"

namespace ram {
namespace vehicle {
namespace device {

IStateEstimator::IStateEstimator(core::EventHubPtr eventHub,
                                 std::string name) :
    IDevice(eventHub, name)
{
}

IStateEstimator::~IStateEstimator()
{
}
    
} // namespace device
} // namespace vehicle
} // namespace ram
