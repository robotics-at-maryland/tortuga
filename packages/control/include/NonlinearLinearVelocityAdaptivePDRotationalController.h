/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/NonlinearPDRotationalController.h
 */

#ifndef RAM_CONTROL_NONLINEARLINEARVELOCITYADAPTIVEPDROTATIONALCONTROLLER_H
#define RAM_CONTROL_NONLINEARLINEARVELOCITYADAPTIVEPDROTATIONALCONTROLLER_H

#include "control/include/RotationalControllerBase.h"
#include "math/include/Matrix3.h"

namespace ram {
namespace control {    

class NonlinearLinearVelocityAdaptivePDRotationalController : public RotationalControllerBase
{
public:
    NonlinearLinearVelocityAdaptivePDRotationalController(ram::core::ConfigNode config);
    virtual ~NonlinearLinearVelocityAdaptivePDRotationalController() {}

    virtual math::Vector3 rotationalUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState);

private:

    double angularPGain;
    double angularDGain;
    math::Matrix3 inertiaEstimate;
    math::Vector3 adaptTerms;

};

} // namespace control
} // namespace ram

#endif // RAM_CONTROL_NONLINEARPDROTATIONALCONTROLLER_H
