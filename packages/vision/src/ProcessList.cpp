/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim
 * File:  packages/vision/src/ProcessList.cpp
 */

// Project Includes
#include "vision/include/ProcessList.h"

namespace ram {
namespace vision {

ProcessList::ProcessList()
{

}

void ProcessList::addStep(std::string s)
{
	toCall.push_back(s);
}

} // namespace vision
} // namespace ram
