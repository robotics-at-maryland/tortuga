/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/SubsystemMaker.cpp
 */

// Project Includes
#include "core/include/SubsystemMaker.h"

namespace ram {
namespace core {

SubsystemPtr addressSpaceFix()
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
      "{ 'name' : 'Mock', 'type' : 'MockSubsystem' }"));
    ram::core::SubsystemList deps;
    deps.push_back(ram::core::SubsystemPtr());

    // It appears, that this call is the key
    return ram::core::SubsystemMaker::newObject(std::make_pair(config, deps));
}

} // namespace core
} // namespace ram
