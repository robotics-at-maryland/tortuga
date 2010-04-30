/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vehicle/estimator/include/BasicStateEstimator.h
 */

// Library Includes

// Project Includes
#include "core/include/Forward.h"
#include "core/include/EventHub.h"
#include "core/include/EventPublisher.h"
#include "vehicle/estimator/include/StateEstimatorBase.h"

#ifndef RAM_VEHICLE_ESTIMATOR_BASICSTATEESTIMATOR_H
#define RAM_VEHICLE_ESTIMATOR_BASICSTATEESTIMATOR_H

namespace ram {
namespace estimator {

class BasicStateEstimator : public StateEstimatorBase
{
public:

    BasicStateEstimator(core::ConfigNode config,
                        core::EventHubPtr eventHub = core::EventHubPtr(),
                        vehicle::IVehiclePtr vehicle = vehicle::IVehiclePtr());

    virtual ~BasicStateEstimator();

protected:

    virtual void rawUpdate_DVL(core::EventPtr event/*DVLRawUpdateEvent*/);
    virtual void rawUpdate_IMU(core::EventPtr event/*IMURawUpdateEvent*/);
    virtual void rawUpdate_DepthSensor(core::EventPtr event/*DepthSensorRawUpdateEvent*/);
    virtual void update_Vision(core::EventPtr event/*VisionUpdateEvent*/);
    virtual void update_Sonar(core::EventPtr event/*SonarUpdateEvent*/);

    core::EventConnectionPtr updateConnection_IMU;
    core::EventConnectionPtr updateConnection_DVL;
    core::EventConnectionPtr updateConnection_DepthSensor;
    core::EventConnectionPtr updateConnection_Sonar;
    core::EventConnectionPtr updateConnection_Vision;
    

private:

    vehicle::IVehiclePtr m_vehicle;
};

} // namespace estimator
} // namespace ram

#endif // RAM_VEHICLE_ESTIMATOR_BASICSTATEESTIMATOR_H
