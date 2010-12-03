/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/PIDDepthTrackingController.h
 */

#ifndef RAM_CONTROL_PIDDEPTHTRACKINGCONTROLLER_H
#define RAM_CONTROL_PIDDEPTHTRACKINGCONTROLLER_H

// Project Includes
#include "control/include/DepthControllerBase.h"

namespace ram {
namespace control {    

class PIDDepthTrackingController : public DepthControllerBase
{
public:
    PIDDepthTrackingController(ram::core::ConfigNode config);
    virtual ~PIDDepthTrackingController() {}

    virtual math::Vector3 depthUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState);

private:
    double m_iErr; // Integrated error
    double m_kp, m_kd, m_ki; // PID gains
    double m_dtMin, m_dtMax; // Timestep limits
};

} // namespace control
} // namespace ram
    
#endif // RAM_CONTROL_PIDDEPTHTRACKINGCONTROLLER_H
