/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  wrappers/network/src/Workaround.cpp
 */

// Project Includes
#include "core/include/Subsystem.h"

// Needed because Py++ insists on trying to generate a wrapper for this class
// even though we are doing it manually
void register_SubsystemList_class()
{
}
