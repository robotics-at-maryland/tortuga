/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/modules/BasicDepthEstimationModule.h
 */

/* This module attempts to estimate depth and depth_dot via a Kalman filter */


#ifndef RAM_ESTIMATION_DEPTHKALMANMODULE_H
#define RAM_ESTIMATION_DEPTHKALMANMODULE_H

// STD Includes

// Library Includes

// Project Includes
#include "estimation/include/EstimatedState.h"
#include "estimation/include/EstimationModule.h"

#include "core/include/ConfigNode.h"
#include "core/include/Event.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix2.h"

#include "math/include/SGolaySmoothingFilter.h"

namespace ram {
namespace estimation {

class DepthKalmanModule : public EstimationModule
{
public:
    DepthKalmanModule(core::ConfigNode config,
                      core::EventHubPtr eventHub,
                      EstimatedStatePtr estState);

    virtual ~DepthKalmanModule(){};

    virtual void update(core::EventPtr event);

private:
    // Kalman filter variables
    math::Vector2 m_xHat;  // previous state estimate
    math::Matrix2 m_Pk;    // process covariance

    math::SGolaySmoothingFilterPtr m_filteredDepth;
    math::SGolaySmoothingFilterPtr m_filteredDepthDot;
};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_DEPTHKALMANMODULE_H
