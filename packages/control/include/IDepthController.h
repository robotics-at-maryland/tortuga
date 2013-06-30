/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/IDepthController.h
 */

#ifndef RAM_CONTROL_IDEPTHCONTROLLER_08_07_2008
#define RAM_CONTROL_IDEPTHCONTROLLER_08_07_2008

// Project Includes
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "estimation/include/Common.h"

#include "control/include/DesiredState.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

/** Defines the interface for a depth controller */
class RAM_EXPORT IDepthController
{
public:
    virtual ~IDepthController() {}

    /** Returns true if the vehicle is at the desired depth */
    //virtual bool atDepth() = 0;

    virtual double getISum()
    {
        //this method is something of a hack
        return -1;
    }
};

/** Provides an interface for a implementation of a Depth Controller */
class RAM_EXPORT IDepthControllerImp : public IDepthController
{
  public:
    virtual ~IDepthControllerImp() {}

    /** Gets the needed vehicle force based on current vehicle state */
    virtual math::Vector3 depthUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState) = 0;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_IDEPTHCONTROLLER_08_07_2008
