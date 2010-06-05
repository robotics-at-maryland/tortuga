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
#include "vehicle/estimator/include/EstimationModule.h"
#include "vehicle/estimator/include/StateEstimatorBase.h"
#include "vehicle/estimator/include/modules/IncludeAllModules.h"

#ifndef RAM_VEHICLE_ESTIMATOR_MODULARSTATEESTIMATOR_H
#define RAM_VEHICLE_ESTIMATOR_MODULARSTATEESTIMATOR_H

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

private:
    /* These contain estimation routines that are config swappable */
    EstimationModulePtr dvlEstimationModule;
    EstimationModulePtr imuEstimationModule;
    EstimationModulePtr depthEstimationModule;

    /* These contain the most recent events passed to each update function */
    core::EventPtr rawDVLDataEvent;
    core::EventPtr rawIMUDataEvent;
    core::EventPtr rawBoomIMUDataEvent;
    core::EventPtr rawDepthDataEvent;

    vehicle::IVehiclePtr m_vehicle;
};

} // namespace estimator
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_MODULARSTATEESTIMATOR_H
