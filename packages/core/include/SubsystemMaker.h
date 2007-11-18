/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/SubSystemMaker.h
 */

#ifndef RAM_CORE_SUBSYSTEMMAKER_09_29_2007
#define RAM_CORE_SUBSYSTEMMAKER_09_29_2007

// STD Includes
#include <utility>

// Project Includes
#include "pattern/include/Maker.h"
#include "core/include/Subsystem.h"
#include "core/include/ConfigNodeKeyExtractor.h"

namespace ram {
namespace core {

typedef std::pair<ConfigNode, SubsystemList> SubsystemMakerParamType;

struct SubsystemKeyExtractor
{
	static std::string extractKey(SubsystemMakerParamType& params)
	{
		return ConfigNodeKeyExtractor::extractKey(params.first);
	}
};

typedef ram::pattern::Maker<Subsystem*, // The type of object created by the maker
			  SubsystemMakerParamType,  // The parameter used to create the object
              std::string,        // The type of key used to register makers
              SubsystemKeyExtractor> // Gets the key from the paramters
SubsystemMaker;

template<class SubsystemType>
struct SubsystemMakerTemplate : public SubsystemMaker
{
    SubsystemMakerTemplate(std::string subsystemType) : 
    	SubsystemMaker(subsystemType) {};
    
    virtual Subsystem* makeObject(
    		SubsystemMakerParamType params)
    {
        return new SubsystemType(params.first, params.second);
    }
};

} // namespace core
} // namespace ram

// arguments are (type name, registered name)
#define RAM_CORE_REGISTER_SUBSYSTEM_MAKER(TYPE, NAME) \
struct NAME ## SubsytsemMaker : \
    public ram::core::SubsystemMakerTemplate<TYPE> \
{ \
    static NAME ## SubsytsemMaker registerThis; \
    NAME ## SubsytsemMaker () : \
        ram::core::SubsystemMakerTemplate<TYPE>(# NAME) {}; \
}; \
NAME ## SubsytsemMaker NAME ## SubsytsemMaker::registerThis

#endif // RAM_CORE_SUBSYSTEMMAKER_09_29_2007
