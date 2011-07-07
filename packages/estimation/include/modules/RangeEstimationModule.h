/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Eliot Rudnick-Cohen <erudnick@umd.edu>
 * All rights reserved.
 *
 * Author: Eliot Rudnick-Cohen <erudnick@umd.edu>
 * File:  packages/estimation/include/modules/RangeEstimation.h
 */

#ifndef RAM_ESTIMATION_RANGEMODULE
#define RAM_ESTIMATION_RANGEMODULE
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

namespace ram {
namespace estimation {


class RangeModule : public EstimationModule
{
public:
    RangeModule(core::ConfigNode config,core::EventHubPtr eventHub,
                         EstimatedStatePtr estimatedState);

    virtual ~RangeModule(){};
    
    virtual void update(core::EventPtr event);

};


}// end of namespace estimation

}// end of namespace ram

#endif
