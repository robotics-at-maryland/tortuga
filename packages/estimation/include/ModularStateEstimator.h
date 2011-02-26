/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/ModularStateEstimator.h
 */

// STD Includes

// Library Includes

// Project Includes
#include "estimation/include/EstimationModule.h"
#include "estimation/include/StateEstimatorBase.h"
#include "estimation/include/modules/IncludeAllModules.h"

#include "core/include/Forward.h"
#include "core/include/EventHub.h"
#include "core/include/Event.h"
#include "core/include/EventPublisher.h"


#ifndef RAM_ESTIMATION_MODULARSTATEESTIMATOR_H
#define RAM_ESTIMATION_MODULARSTATEESTIMATOR_H

namespace ram {
namespace estimation {

class ModularStateEstimator : public StateEstimatorBase
{
public:

    ModularStateEstimator(core::ConfigNode config,
                          core::EventHubPtr eventHub = core::EventHubPtr());

    ModularStateEstimator(core::ConfigNode config,
                          core::SubsystemList deps = core::SubsystemList());
    
    virtual ~ModularStateEstimator();

protected:


private:
    /* These contain estimation routines that are config swappable */
    EstimationModulePtr dvlEstimationModule;
    EstimationModulePtr imuEstimationModule;
    EstimationModulePtr depthEstimationModule;
    EstimationModulePtr visionEstimationModule;
};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_MODULARSTATEESTIMATOR_H
