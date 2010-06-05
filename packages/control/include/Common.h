/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/Common.h
 */

#ifndef RAM_CONTROL_COMMON_07_07_2007
#define RAM_CONTROL_COMMON_07_07_2007

// Library Includes
#include <boost/shared_ptr.hpp>

namespace ram {
namespace control {

// Forward Declartions
struct DesiredState;
struct MeasuredState;
struct ControllerState;
struct EstimatedState;

class IController;
typedef boost::shared_ptr<IController> IControllerPtr;

class ITranslationalController;
typedef boost::shared_ptr<ITranslationalController> ITranslationalControllerPtr;

class ITranslationalControllerImp;
typedef boost::shared_ptr<ITranslationalControllerImp>
ITranslationalControllerImpPtr;

class IDepthController;
typedef boost::shared_ptr<IDepthController> IDepthControllerPtr;

class IDepthControllerImp;
typedef boost::shared_ptr<IDepthControllerImp> IDepthControllerImpPtr;
    
class IRotationalController;
typedef boost::shared_ptr<IRotationalController> IRotationalControllerPtr;

class IRotationalControllerImp;
typedef boost::shared_ptr<IRotationalControllerImp> IRotationalControllerImpPtr;



} // namespace control
} // namespace ram

#endif // RAM_CONTROL_COMMON_07_07_2007
