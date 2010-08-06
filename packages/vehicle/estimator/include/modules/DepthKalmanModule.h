/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/modules/BasicDepthEstimationModule.h
 */

/* This module attempts to estimate depth and depth_dot via a Kalman filter */


#ifndef RAM_VEHICLE_ESTIMATOR_DEPTHKALMANMODULE_H
#define RAM_VEHICLE_ESTIMATOR_DEPTHKALMANMODULE_H

// Library Includes

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "vehicle/estimator/include/EstimatedState.h"
#include "vehicle/estimator/include/EstimationModule.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Matrix2.h"

namespace ram {
namespace estimator {

class DepthKalmanModule : public EstimationModule
{
public:
    DepthKalmanModule(core::ConfigNode config,
                      core::EventHubPtr eventHub);
    ~DepthKalmanModule(){};

    /* called when a depth sensor publishes calibration values */
    virtual void init(core::EventPtr event);

    /* The Depth Estimation routine goes here.  It should store the new estimated
       state in estimatedState. */
    virtual void update(core::EventPtr event, EstimatedStatePtr estimatedState);

private:
    /* any necessary persistent variables should be declared here */
    std::string m_name;
    math::Vector3 m_location;
    double m_calibSlope;
    double m_calibIntercept;

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

} // namespace estimator
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_DEPTHKALMANMODULE_H
