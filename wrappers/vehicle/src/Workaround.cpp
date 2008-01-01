/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/control/src/Workaround.cpp
 */

// Needed because Py++ insists on trying to generate a wrapper for this class
// even though we are doing it manually
void register_SubsystemList_class()
{
}
