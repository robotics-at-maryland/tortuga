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
                        core::EventHubPtr eventHub = core::EventHubPtr(),
                        vehicle::IVehiclePtr vehicle = vehicle::IVehiclePtr());

    virtual ~ModularStateEstimator();

protected:

    /* These functions are bound to certain event types and are called when
       an event of the correct type is published to the event hub */
    virtual void rawUpdate_DVL(core::EventPtr event);
    virtual void rawUpdate_IMU(core::EventPtr event);
    virtual void rawUpdate_DepthSensor(core::EventPtr event);
    virtual void update_Vision(core::EventPtr event);
    virtual void update_Sonar(core::EventPtr event);

private:
    /* These contain estimation routines that are config swappable */
    EstimationModulePtr dvlEstimationModule;
    EstimationModulePtr imuEstimationModule;
    EstimationModulePtr depthEstimationModule;

    /* this may be removed in the future*/
    vehicle::IVehiclePtr m_vehicle;
};

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_MODULARSTATEESTIMATOR_H
