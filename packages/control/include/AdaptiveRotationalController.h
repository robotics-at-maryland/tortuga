/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/AdaptiveRotationalController.h
 */

#ifndef RAM_CONTROL_ADAPTIVEROTATIONALCONTROLLER_H
#define RAM_CONTROL_ADAPTIVEROTATIONALCONTROLLER_H

#include "control/include/RotationalControllerBase.h"
#include "math/include/MatrixN.h"

namespace ram {
namespace control {

class AdaptiveRotationalController : public RotationalControllerBase
{
public:
    AdaptiveRotationalController(ram::core::ConfigNode config);
    virtual ~AdaptiveRotationalController() {}

    virtual math::Vector3 rotationalUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState);

private:
    /** constrain var between min and max.  enhance readability */
    void clip(double &var, double min, double max);

    double m_dtMin;
    double m_dtMax;

    double m_rotLambda;
    double m_rotGamma;
    double m_rotK;

    math::MatrixN m_params;

};

} // namespace control
} // namespace ram

#endif // RAM_CONTROL_ADAPTIVEROTATIONALCONTROLLER_H
