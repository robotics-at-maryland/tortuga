/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/ITranslationalController.h
 */

#ifndef RAM_CONTROL_ITRANSLATIONALCONTROLLER_08_31_2008
#define RAM_CONTROL_ITRANSLATIONALCONTROLLER_08_31_2008

// Project Includes
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "estimation/include/Common.h"

#include "control/include/DesiredState.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

/** Defines the interface for controler which controls in plane motion */
class RAM_EXPORT ITranslationalController 
{
public:

    /** The exact type of translation control we are undergoing */

    virtual ~ITranslationalController() {}

};

/** Provides an interface for a implementation of a Translational Controller */
class RAM_EXPORT ITranslationalControllerImp : public ITranslationalController
{
  public:    
    virtual ~ITranslationalControllerImp() {}

    /** Gets the needed vehicle force based on current vehicle state */
    virtual math::Vector3 translationalUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState) = 0;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_ITRANSLATIONALCONTROLLER_08_31_2008
