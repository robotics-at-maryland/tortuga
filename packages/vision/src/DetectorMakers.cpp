/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/DetectorMakers.cpp
 */

// All registration are done in one file to keep the compilation dependency on 
// the factory system to a minimum

// Project Includes
#include "vision/include/DetectorMaker.h"

#include "vision/include/RedLightDetector.h"
#include "vision/include/BinDetector.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/GateDetector.h"
#include "vision/include/FeatureDetector.h"
#include "vision/include/SuitDetector.h"
#include "vision/include/DuctDetector.h"
#include "vision/include/DownwardDuctDetector.h"
#include "vision/include/SafeDetector.h"

namespace ram {
namespace vision {

static DetectorMakerTemplate<RedLightDetector>
registerRedLightDetector("RedLightDetector");

static DetectorMakerTemplate<BinDetector>
registerBinDetector("BinDetector");

static DetectorMakerTemplate<OrangePipeDetector>
registerOrangePipeDetector("OrangePipeDetector");
    
static DetectorMakerTemplate<GateDetector>
registerGateDetector("GateDetector");

static DetectorMakerTemplate<FeatureDetector>
registerFeatureDetector("FeatureDetector");

static DetectorMakerTemplate<SuitDetector>
registerSuitDetector("SuitDetector");

static DetectorMakerTemplate<DuctDetector>
registerDuctDetector("DuctDetector");

static DetectorMakerTemplate<DownwardDuctDetector>
registerDownwardDuctDetector("DownwardDuctDetector");

static DetectorMakerTemplate<SafeDetector>
registerSafeDetector("SafeDetector");
    
} // namespace vision
} // namespace ram
    
// In this case the static variable is safe
#if RAM_COMPILER == RAM_COMPILER_MSVC
#  pragma warning( disable : 4640 )
#endif

namespace ram {
namespace pattern {

template<>
vision::DetectorMaker::MakerMap*
vision::DetectorMaker::getRegistry()
{
    // This line is run only once, avoids static initialization order issue
    static vision::DetectorMaker::MakerMap* reg =
        new vision::DetectorMaker::MakerMap();
    return reg;
}

} // namespace pattern
} // namespace ram
