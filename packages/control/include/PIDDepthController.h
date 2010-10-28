/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/PIDDepthController.h
 */


#ifndef RAM_CONTROL_PIDDEPTHCONTROLLER_H
#define RAM_CONTROL_PIDDEPTHCONTROLLER_H

#include "control/include/DepthControllerBase.h"

namespace ram {
namespace control {    

class PIDDepthController : public DepthControllerBase
{
public:
    PIDDepthController(ram::core::ConfigNode config);
    virtual ~PIDDepthController() {}

    virtual math::Vector3 depthUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState);

private:

  double m_depthSumError;
  double m_prevDepth;

  double m_kp;
  double m_kd;
  double m_ki;

  double dt_min;
  double dt_max;
};

} // namespace control
} // namespace ram
    
#endif // RAM_CONTROL_PIDDEPTHCONTROLLER_H
