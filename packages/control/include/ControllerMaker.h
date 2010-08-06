/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/ControllerMaker.h
 */

#ifndef RAM_CONTROL_CONTROLLERMAKER_09_01_2008
#define RAM_CONTROL_CONTROLLERMAKER_09_01_2008

#include<utility>

// Project Includes
#include "control/include/IDepthController.h"
#include "control/include/IRotationalController.h"
#include "control/include/ITranslationalController.h"
#include "control/include/Common.h"

#include "pattern/include/Maker.h"

#include "core/include/ConfigNodeKeyExtractor.h"

// Must Be Included last
#include "control/include/Export.h"

// Need to disable this on windows, could cause hard to solve link errors
#ifdef RAM_WINDOWS
#pragma warning( disable : 4661 ) 
#endif
        
namespace ram {
namespace control {

typedef core::ConfigNode ControllerMakerParamType;
//typedef std::pair<core::ConfigNode, core::EventHubPtr> ControllerMakerParamType;

struct ControllerKeyExtractor
{
    static std::string extractKey(ControllerMakerParamType& params)
    {
        return core::ConfigNodeKeyExtractor::extractKey(params);
//        return core::ConfigNodeKeyExtractor::extractKey(params.first);
    }
};

/** A template which makes it easier to do the controller specific template */
template<class ControllerType, class MakerType>
struct ControllerImpMakerTemplate : public MakerType
{
    ControllerImpMakerTemplate(std::string controllerType) :
        MakerType(controllerType) {};
    
    virtual typename MakerType::ObjectType makeObject(
        ControllerMakerParamType params)
    {
        return typename MakerType::ObjectType(new ControllerType(params));
//        return typename Maker::ObjectType(new ControllerType(params.first,
//                                                             params.second));
    }
};

// ------------------------------------------------------------------------- //
//              D E P T H   C O N T R O L L E R   M A K E R                  //
// ------------------------------------------------------------------------- //
    
typedef pattern::Maker<
    IDepthControllerImpPtr,   // The type of object created by the maker
    ControllerMakerParamType, // The parameter used to create the object
    std::string,              // The type of key used to register makers
    ControllerKeyExtractor>   // Gets the key from the paramters
DepthControllerImpMaker;

// Needed to keep the linker/compiler happy
#ifdef RAM_WINDOWS
template class RAM_EXPORT
pattern::Maker< IDepthControllerImpPtr, ControllerMakerParamType, std::string,
                ControllerKeyExtractor>   
#endif

template<class ControllerType>
struct DepthControllerImpMakerTemplate :
    public ControllerImpMakerTemplate<ControllerType, DepthControllerImpMaker>
{
    DepthControllerImpMakerTemplate(std::string controllerType) :
        ControllerImpMakerTemplate<ControllerType, DepthControllerImpMaker>(
            controllerType) {};
};

// ------------------------------------------------------------------------- //
//       T R A N S L A T I O N A L   C O N T R O L L E R   M A K E R         //
// ------------------------------------------------------------------------- //

typedef pattern::Maker<
    ITranslationalControllerImpPtr, // The type of object created by the maker
    ControllerMakerParamType,       // The parameter used to create the object
    std::string,                    // The type of key used to register makers
    ControllerKeyExtractor>         // Gets the key from the paramters
TranslationalControllerImpMaker;

// Needed to keep the linker/compiler happy
#ifdef RAM_WINDOWS
template class RAM_EXPORT
pattern::Maker< ITranslationalControllerImpPtr,
                ControllerMakerParamType, std::string, ControllerKeyExtractor>
#endif

template<class ControllerType>
struct TranslationalControllerImpMakerTemplate :
    public ControllerImpMakerTemplate<ControllerType,
                                      TranslationalControllerImpMaker>
{
    TranslationalControllerImpMakerTemplate(std::string controllerType) :
        ControllerImpMakerTemplate<ControllerType,
                                   TranslationalControllerImpMaker>
        (controllerType) {};
};

// ------------------------------------------------------------------------- //
//          R O T A T I O N A L   C O N T R O L L E R   M A K E R            //
// ------------------------------------------------------------------------- //

typedef pattern::Maker<
    IRotationalControllerImpPtr, // The type of object created by the maker
    ControllerMakerParamType,    // The parameter used to create the object
    std::string,                 // The type of key used to register makers
    ControllerKeyExtractor>      // Gets the key from the paramters
RotationalControllerImpMaker;

// Needed to keep the linker/compiler happy
#ifdef RAM_WINDOWS
template class RAM_EXPORT
pattern::Maker< IRotationalControllerImpPtr,
                ControllerMakerParamType, std::string, ControllerKeyExtractor>
#endif

template<class ControllerType>
struct RotationalControllerImpMakerTemplate :
    public ControllerImpMakerTemplate<ControllerType,
                                      RotationalControllerImpMaker>
{
    RotationalControllerImpMakerTemplate(std::string controllerType) :
        ControllerImpMakerTemplate<ControllerType,
                                   RotationalControllerImpMaker>
        (controllerType) {};
};

} // namespace control
} // namespace ram

#endif	// RAM_CONTROL_CONTROLLERMAKER_09_01_2008

