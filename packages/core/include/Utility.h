/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/PythonConfigNodeImp.cpp
 */


// Library Includes
#include <boost/filesystem.hpp>

namespace ram {
namespace core {

struct Utility
{
    boost::filesystem::path relativePath(boost::filesystem::path path,
                                         boost::filesystem::path start);
};
    
} // namespace core
} // namespace ram
