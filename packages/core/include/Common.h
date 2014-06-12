/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/ConfigNode.h
 */

#ifndef RAM_CORE_COMMON_06_30_2007
#define RAM_CORE_COMMON_06_30_2007

// Library Includes
#include <boost/shared_ptr.hpp>

namespace ram {
namespace core {
    
class ConfigNodeImp;
typedef boost::shared_ptr<ConfigNodeImp> ConfigNodeImpPtr;
    
} // namespace core
} // namespace ram

#endif // RAM_CORE_COMMON_06_30_2007
