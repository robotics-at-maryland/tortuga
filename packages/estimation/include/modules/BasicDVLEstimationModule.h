/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/modules/BasicDVLEstimationModule.h
 */

/* This is currently a template for a DVL EstimationModule implementation */


#ifndef RAM_ESTIMATION_BASICDVLESTIMATIONMODULE_H
#define RAM_ESTIMATION_BASICDVLESTIMATIONMODULE_H

// STD Includes

// Library Includes

// Project Includes
#include "estimation/include/EstimatedState.h"
#include "estimation/include/EstimationModule.h"

#include "core/include/ConfigNode.h"
#include "core/include/Event.h"

namespace ram {
namespace estimation {

class BasicDVLEstimationModule : public EstimationModule
{
public:
    BasicDVLEstimationModule(core::ConfigNode config,
                             core::EventHubPtr eventHub,
                             EstimatedStatePtr estState);

    virtual ~BasicDVLEstimationModule(){};

    // the DVL Estimation routine goes here. 
    // it should store the new estimated state in estimatedState.
    virtual void update(core::EventPtr event);

private:
    // any necessary persistent variables should be declared here
};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_BASICDVLESTIMATIONMODULE_H
