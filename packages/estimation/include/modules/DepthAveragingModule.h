/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/modules/DepthAveragingModule.h
 */

/* This is currently a template for a Depth EstimationModule implementation */


#ifndef RAM_ESTIMATION_DEPTHAVERAGINGMODULE_H
#define RAM_ESTIMATION_DEPTHAVERAGINGMODULE_H

// STD Includes

// Library Includes

// Project Includes
#include "estimation/include/EstimatedState.h"
#include "estimation/include/EstimationModule.h"

#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "core/include/AveragingFilter.h"

namespace ram {
namespace estimation {

const static int FILTER_SIZE = 10;

class DepthAveragingModule : public EstimationModule
{
public:
    DepthAveragingModule(core::ConfigNode config,
                               core::EventHubPtr eventHub);
    ~DepthAveragingModule(){};

    /* The Depth Estimation routine goes here.  It should store the new estimated
       state in estimatedState. */
    virtual void update(core::EventPtr event, EstimatedStatePtr estimatedState);

private:
    /* any necessary persistent variables should be declared here */
    std::string m_name;
    double m_previousDepth;

    core::AveragingFilter<double, FILTER_SIZE> m_filteredDepth;
    core::AveragingFilter<double, FILTER_SIZE> m_filteredDepthRate;
};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_DEPTHAVERAGINGMODULE_H
