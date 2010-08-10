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

namespace ram {
namespace estimation {

class DepthKalmanModule : public EstimationModule
{
public:
    DepthKalmanModule(core::ConfigNode config,
                      core::EventHubPtr eventHub);
    ~DepthKalmanModule(){};

    /* The Depth Estimation routine goes here.  It should store the new estimated
       state in estimatedState. */
    virtual void update(core::EventPtr event,
                        EstimatedStatePtr estimatedState);

private:
    /* any necessary persistent variables should be declared here */
    std::string m_name;

    // Kalman Filter Variables

    // vehicle mass
    double m_mass;

    // vehicle drag coefficient
    double m_drag;

    // vehicle buoyancy
    double m_buoyancy;

    // initial estimate [depth, depth_dot]'
    math::Vector2 m_x0;

    // initial estimate error covariance
    math::Matrix2 m_P0;

    // previous state estimate
    math::Vector2 m_xPrev;

    // previous control input
    double m_uPrev;
    
    // previous state transition model
    math::Matrix2 m_Ak_prev;

    // previous estimate covariance
    math::Matrix2 m_PPrev;

};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_DEPTHKALMANMODULE_H
