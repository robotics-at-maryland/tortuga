/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/DetectorMaker.h
 */

#ifndef RAM_VISION_DETECTORMAKER_02_23_2008
#define RAM_VISION_DETECTORMAKER_02_23_2008

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "pattern/include/Maker.h"
#include "core/include/ConfigNodeKeyExtractor.h"

// Must Be Included last
#include "vision/include/Export.h"

// Need to disable this on windows, could cause hard to solve link errors
#ifdef RAM_WINDOWS
#pragma warning( disable : 4661 ) 
#endif
        
namespace ram {
namespace vision {

typedef std::pair<core::ConfigNode, core::EventHubPtr> DetectorMakerParamType;

struct DetectorKeyExtractor
{
    static std::string extractKey(DetectorMakerParamType& params)
    {
        return core::ConfigNodeKeyExtractor::extractKey(params.first);
    }
};
    
typedef pattern::Maker<
    DetectorPtr, // The type of object created by the maker
    DetectorMakerParamType,  // The parameter used to create the object
    std::string,            // The type of key used to register makers
    DetectorKeyExtractor> // Gets the key from the paramters
DetectorMaker;

// Needed to keep the linker/compiler happy
#ifdef RAM_WINDOWS
template class RAM_EXPORT pattern::Maker<DetectorPtr,
    DetectorMakerParamType,
    std::string,
    DetectorKeyExtractor>;
#endif

template<class DetectorType>
struct DetectorMakerTemplate : public DetectorMaker
{
    DetectorMakerTemplate(std::string deviceType) :
        DetectorMaker(deviceType) {};
    
    virtual DetectorPtr makeObject(DetectorMakerParamType params)
    {
        return DetectorPtr(new DetectorType(params.first, params.second));
    }
};

} // namespace vision
} // namespace ram

#endif	// RAM_VISION_DETECTORMAKER_02_23_2008

