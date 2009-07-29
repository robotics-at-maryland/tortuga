/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/CameraMaker.h
 */

#ifndef RAM_VISION_CAMERAMAKER_02_23_2008
#define RAM_VISION_CAMERAMAKER_02_23_2008

// Library Includes
#include <boost/tuple/tuple.hpp>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Camera.h"
#include "pattern/include/Maker.h"
#include "core/include/ConfigNodeKeyExtractor.h"
#include "core/include/Common.h"

// Must Be Included last
#include "vision/include/Export.h"

// Need to disable this on windows, could cause hard to solve link errors
#ifdef RAM_WINDOWS
#pragma warning( disable : 4661 ) 
#endif
        
namespace ram {
namespace vision {

typedef boost::tuple<const std::string, core::ConfigNode, std::string&,
                     core::EventHubPtr>
CameraMakerParamType;

/** Extracts the key (type) of the camera from the creation input */    
struct CameraKeyExtractor
{
    static std::string extractKey(CameraMakerParamType& params);
};
    
typedef pattern::Maker<
    CameraPtr, // The type of object created by the maker
    CameraMakerParamType,  // The parameter used to create the object
    std::string,            // The type of key used to register makers
    CameraKeyExtractor> // Gets the key from the paramters
CameraMaker;

// Needed to keep the linker/compiler happy
#ifdef RAM_WINDOWS
template class RAM_EXPORT pattern::Maker<CameraPtr,
    CameraMakerParamType,
    std::string,
    CameraKeyExtractor>;
#endif
    
template<class CameraType>
struct CameraMakerTemplate : public CameraMaker
{
    CameraMakerTemplate(std::string deviceType) :
        CameraMaker(deviceType) {};
    
    virtual CameraPtr makeObject(CameraMakerParamType params);
};

} // namespace vision
} // namespace ram

#endif	// RAM_VISION_CAMERAMAKER_02_23_2008

// STD Includes
#include <sstream>

// Library Includes
#include <boost/regex.hpp>
