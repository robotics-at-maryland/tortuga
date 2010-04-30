/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/modules/BasicDVLEstimationModule.h
 */

/* This is currently a template for a DVL EstimationModule implementation */


#ifndef RAM_VEHICLE_ESTIMATOR_BASICDVLESTIMATIONMODULE_H
#define RAM_VEHICLE_ESTIMATOR_BASICDVLESTIMATIONMODULE_H

// Library Includes

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "vehicle/estimator/include/EstimatedState.h"
#include "vehicle/estimator/include/EstimationModule.h"

namespace ram {
namespace estimator {

class BasicDVLEstimationModule : public EstimationModule
{
public:
    BasicDVLEstimationModule(core::ConfigNode config);
    ~BasicDVLEstimationModule(){};

    /* The DVL Estimation routine goes here.  It should store the new estimated
       state in estimatedState. */
    virtual void update(core::EventPtr event, EstimatedStatePtr estimatedState);

private:
    /* any necessary persistent variables should be declared here */
};

} // namespace estimatior
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_BASICDVLESTIMATIONMODULE_H
