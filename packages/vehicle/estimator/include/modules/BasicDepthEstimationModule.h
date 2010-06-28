/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/modules/BasicDepthEstimationModule.h
 */

/* This is currently a template for a Depth EstimationModule implementation */


#ifndef RAM_VEHICLE_ESTIMATOR_BASICDEPTHESTIMATIONMODULE_H
#define RAM_VEHICLE_ESTIMATOR_BASICDEPTHESTIMATIONMODULE_H

// Library Includes

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "vehicle/estimator/include/EstimatedState.h"
#include "vehicle/estimator/include/EstimationModule.h"

namespace ram {
namespace estimator {

class BasicDepthEstimationModule : public EstimationModule
{
public:
    BasicDepthEstimationModule(core::ConfigNode config);
    ~BasicDepthEstimationModule(){};

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
};

} // namespace estimatior
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_BASICDEPTHESTIMATIONMODULE_H
