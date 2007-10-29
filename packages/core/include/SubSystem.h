/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/SubSystem.h
 */

#ifndef RAM_CORE_SUBSYSTEM_09_29_2007
#define RAM_CORE_SUBSYSTEM_09_29_2007

// STD Includes
#include <string>

namespace ram {
namespace core {

/** Reprsents a major part of the overall system

    This is a class mainly a marker class, used to provide a uniform start up
    framework.
 */
class SubSystem
{
public:
    SubSystem(std::string name) : m_name(name) {}

    virtual ~SubSystem() {};
    
    std::string getName() { return m_name; }

private:
    std::string m_name;
};
    
} // namespace core
} // namespace ram

#endif // RAM_CORE_SUBSYSTEM_09_29_2007
