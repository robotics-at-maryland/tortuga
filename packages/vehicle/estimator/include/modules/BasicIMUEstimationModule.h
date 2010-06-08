/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/modules/BasicIMUEstimationModule.h
 */

/* This is currently a template for a IMU EstimationModule implementation */


#ifndef RAM_VEHICLE_ESTIMATOR_BASICIMUESTIMATIONMODULE_H
#define RAM_VEHICLE_ESTIMATOR_BASICIMUESTIMATIONMODULE_H

// Library Includes

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "vehicle/estimator/include/EstimatedState.h"
#include "vehicle/estimator/include/EstimationModule.h"

namespace ram {
namespace estimator {

class BasicIMUEstimationModule : public EstimationModule
{
public:
    BasicIMUEstimationModule(core::ConfigNode config);
    ~BasicIMUEstimationModule(){};


    /* This is called when an IMU publishes its calibration values */
    virtual void init(core::EventPtr event);

    /* The IMU Estimation routine goes here.  It should store the new estimated
       state in estimatedState. */
    virtual void update(core::EventPtr event, EstimatedStatePtr estimatedState);

private:
    /* any necessary persistent variables should be declared here */
};

} // namespace estimatior
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_BASICIMUESTIMATIONMODULE_H
