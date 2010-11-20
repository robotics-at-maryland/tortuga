/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/ModularStateEstimator.h
 */

// Library Includes

// Project Includes
#include "core/include/Forward.h"
#include "core/include/EventHub.h"
#include "core/include/Event.h"
#include "core/include/EventPublisher.h"
#include "vehicle/include/estimator/EstimationModule.h"
#include "vehicle/include/estimator/StateEstimatorBase.h"
#include "vehicle/include/estimator/modules/IncludeAllModules.h"

#ifndef RAM_ESTIMATOR_MODULARSTATEESTIMATOR_H
#define RAM_ESTIMATOR_MODULARSTATEESTIMATOR_H

namespace ram {
namespace estimator {

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

    virtual void init_IMU(core::EventPtr event);
    virtual void init_DVL(core::EventPtr event);
    virtual void init_DepthSensor(core::EventPtr event);

private:
    /* These contain estimation routines that are config swappable */
    EstimationModulePtr dvlEstimationModule;
    EstimationModulePtr imuEstimationModule;
    EstimationModulePtr depthEstimationModule;

    /* this may be removed in the future*/
    vehicle::IVehiclePtr m_vehicle;
};

} // namespace estimator
} // namespace ram

#endif // RAM_ESTIMATOR_MODULARSTATEESTIMATOR_H
