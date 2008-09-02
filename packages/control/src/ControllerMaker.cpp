/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/ControllerMakers.cpp
 */

// All registration are done in one file to keep the compilation dependency on 
// the factory system to a minimum

// Project Includes
#include "control/include/ControllerMaker.h"

/*namespace ram {
namespace control {

    
} // namespace control
} // namespace ram*/
    
// In this case the static variable is safe
#if RAM_COMPILER == RAM_COMPILER_MSVC
#  pragma warning( disable : 4640 )
#endif

namespace ram {
namespace pattern {

template<>
control::DepthControllerImpMaker::MakerMap*
control::DepthControllerImpMaker::getRegistry()
{
    // This line is run only once, avoids static initialization order issue
    static control::DepthControllerImpMaker::MakerMap* reg =
        new control::DepthControllerImpMaker::MakerMap();
    return reg;
}

template<>
control::TranslationalControllerImpMaker::MakerMap*
control::TranslationalControllerImpMaker::getRegistry()
{
    // This line is run only once, avoids static initialization order issue
    static control::TranslationalControllerImpMaker::MakerMap* reg =
        new control::TranslationalControllerImpMaker::MakerMap();
    return reg;
}

template<>
control::RotationalControllerImpMaker::MakerMap*
control::RotationalControllerImpMaker::getRegistry()
{
    // This line is run only once, avoids static initialization order issue
    static control::RotationalControllerImpMaker::MakerMap* reg =
        new control::RotationalControllerImpMaker::MakerMap();
    return reg;
}
    
} // namespace pattern
} // namespace ram
