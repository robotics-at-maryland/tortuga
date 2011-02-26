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
#include "math/include/AveragingFilter.h"

namespace ram {
namespace estimation {

static const int DEPTH_FILTER_SIZE = 10;
typedef math::AveragingFilter<double, DEPTH_FILTER_SIZE>  DepthAveragingFilter;

class DepthAveragingModule : public EstimationModule
{
public:
    DepthAveragingModule(core::ConfigNode config,
                         core::EventHubPtr eventHub,
                         EstimatedStatePtr estState);

    ~DepthAveragingModule(){};

    // the Depth Estimation routine goes here.  
    // it should store the new estimated state in estimatedState.
    virtual void update(core::EventPtr event);

private:
    std::string m_name;
    double m_previousDepth;

    DepthAveragingFilter m_filteredDepth;
    DepthAveragingFilter m_filteredDepthRate;
};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_DEPTHAVERAGINGMODULE_H
