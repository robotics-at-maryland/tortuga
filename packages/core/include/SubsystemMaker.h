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

// Must Be Included last
#include "core/include/Export.h"

// Need to disable this on windows, could cause hard to solve link errors
#ifdef RAM_WINDOWS
#pragma warning( disable : 4661 ) 
#endif

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

typedef ram::pattern::Maker<SubsystemPtr, // The type of object created by the maker
              SubsystemMakerParamType,  // The parameter used to create the object
              std::string,        // The type of key used to register makers
              SubsystemKeyExtractor> // Gets the key from the paramters
SubsystemMaker;
    
// Needed to keep the linker/compiler happy
#ifdef RAM_WINDOWS
template class RAM_EXPORT ram::pattern::Maker<SubsystemPtr, 
                                              SubsystemMakerParamType,  
                                              std::string,        
                                              SubsystemKeyExtractor>;
#endif


template<class SubsystemType>
struct SubsystemMakerTemplate : public SubsystemMaker
{
    SubsystemMakerTemplate(std::string subsystemType) : 
        SubsystemMaker(subsystemType) {};
    
    virtual SubsystemPtr makeObject(
            SubsystemMakerParamType params)
    {
        return boost::shared_ptr<SubsystemType>(
            new SubsystemType(params.first, params.second));
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

// Magic namespace to clean up names
namespace pyplusplus { 
namespace aliases {

typedef ram::core::SubsystemMaker SubsystemMaker;
typedef ram::core::SubsystemMakerParamType SubsystemMakerParamType;

}
}

// So GCCXML see's our typedef
namespace details {
inline int instantiateSubsystemMaker()
{
    int a = sizeof(ram::core::SubsystemMaker);
    a += sizeof(ram::core::SubsystemMakerParamType);
    return a;
}
}

#endif // RAM_CORE_SUBSYSTEMMAKER_09_29_2007
