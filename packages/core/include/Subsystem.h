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
#include <vector>

// Project Includes
#include "core/include/IUpdatable.h"

namespace ram {
namespace core {

class Subsystem;
typedef std::vector<Subsystem*> SubsystemList;

/** Reprsents a major part of the overall system

    This is a class mainly a marker class, used to provide a uniform start up
    framework.
 */
class Subsystem : public IUpdatable
{
public:
    virtual ~Subsystem() {};
    
    std::string getName() { return m_name; }

    template<typename T>
    static Subsystem* getSubsystemOfType(SubsystemList list) 
    {
    	Subsystem* result = 0;
    	for (size_t i = 0; i < list.size(); ++i) 
    	{
    		result = dynamic_cast<T*>(list[i]);
    		if (0 != result)
    			return result;
    	}
    	
    	return result;
    }
    
protected:
    Subsystem(std::string name) : m_name(name) {}
	
private:
    std::string m_name;
};
    
} // namespace core
} // namespace ram

#endif // RAM_CORE_SUBSYSTEM_09_29_2007
