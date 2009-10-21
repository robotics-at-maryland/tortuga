/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/slam/include/Common.h
 */

#ifndef RAM_SLAM_COMMON_10_17_2009
#define RAM_SLAM_COMMON_10_17_2009

// Library Includes
#include <boost/shared_ptr.hpp>

namespace ram {
namespace slam {

// Forward Declartions
class ISlam;
typedef boost::shared_ptr<ISlam> ISlamPtr;
    
} // namespace slam
} // namespace ram

#endif // RAM_SLAM_COMMON_10_17_2009
