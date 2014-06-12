/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/ConfigNodeKeyExtractor.cxx
 */

// STD Includes
#include <string>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "core/include/ConfigNodeKeyExtractor.h"

namespace ram {
namespace core {

const std::string CFG = "{'type' : 'TestName'}";
    
TEST(TestConfigNodeKeyExtractor)
{
    ConfigNode node(ConfigNode::fromString(CFG));
    
    std::string name =
        ConfigNodeKeyExtractor::extractKey(node);

    CHECK_EQUAL("TestName", name);
    
}
  
} // namespace core
} // namespace ram
