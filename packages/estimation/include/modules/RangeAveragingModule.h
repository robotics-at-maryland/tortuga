/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Eliot Rudnick-Cohen <erudnick@umd.edu>
 * All rights reserved.
 *
 * Author: Eliot Rudnick-Cohen <erudnick@umd.edu>
 * File:  packages/estimation/include/modules/RangeAveraging.h
 */

#ifndef RAM_ESTIMATION_RANGEAVERAGINGMODULE
#define RAM_ESTIMATION_RANGEAVERAGINGMODULE
// STD Includes
#include <iostream>
// Library Includes
#include <log4cpp/Category.hh>

//Project Includes
#include "vehicle/include/Events.h"
#include "estimation/include/EstimatedState.h"
#include "estimation/include/EstimationModule.h"
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "math/include/AveragingFilter.h"

namespace ram {
namespace estimation {

static const int RANGE_FILTER_SIZE = 10;

class RangeAveragingModule : public EstimationModule
{
public:
    RangeAveragingModule(core::ConfigNode config,core::EventHubPtr eventHub,
                EstimatedStatePtr estimatedState);

    virtual ~RangeAveragingModule(){};
    
    virtual void update(core::EventPtr event);
private:
    math::AveragingFilter<double,RANGE_FILTER_SIZE> filter;

};


}// end of namespace estimation

}// end of namespace ram

#endif
