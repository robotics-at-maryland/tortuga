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
#include <set>

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/IUpdatable.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

class Subsystem;
typedef boost::shared_ptr<Subsystem> SubsystemPtr;
typedef std::set<SubsystemPtr> SubsystemList;
typedef SubsystemList::iterator SubsystemListIter;

    
/** Reprsents a major part of the overall system

    This is a class mainly a marker class, used to provide a uniform start up
    framework.
 */
class RAM_EXPORT Subsystem : public IUpdatable
{
public:
    virtual ~Subsystem() {};
    
    std::string getName() { return m_name; }

    template<typename T>
    static boost::shared_ptr<T> getSubsystemOfType(SubsystemList list) 
    {
        boost::shared_ptr<T> result = boost::shared_ptr<T>();
        SubsystemListIter iter = list.begin();
        SubsystemListIter end = list.end();
    	for (; iter != end; ++iter) 
    	{
                result = boost::dynamic_pointer_cast<T>(*iter);
    		if (result)
                    return result;
    	}
    	
    	return result;
    }
    
protected:
    Subsystem(std::string name) : m_name(name) {}
	
private:
    std::string m_name;
};


namespace details {
inline int instantiateA()
{
    int a = sizeof(SubsystemList);
    a += sizeof(SubsystemPtr);
    return a;
}
}
    
} // namespace core
} // namespace ram

#endif // RAM_CORE_SUBSYSTEM_09_29_2007
