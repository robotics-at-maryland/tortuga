/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/device/IStateEstimatorDevice.h
 */

#ifndef RAM_VEHICLE_DEVICE_ISTATEESTIMATORDEVICE_12_17_2009
#define RAM_VEHICLE_DEVICE_ISTATEESTIMATORDEVICE_12_17_2009

// STD Includes
#include <string>

// Project Includes
#include "core/include/EventHub.h"
#include "vehicle/include/Common.h"
#include "vehicle/include/device/Common.h"
#include "vehicle/include/device/IDevice.h"

// Must be included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

class RAM_EXPORT IStateEstimatorDevice : public IDevice // for getName
{
public:
    virtual ~IStateEstimatorDevice() {};

    /** sets the devices state estimator */
    void setStateEstimator(IStateEstimatorPtr stateEstimator);

    /** sets the state estimator based on the vehicle */
    void setVehicle(IVehiclePtr vehicle);

    IStateEstimatorPtr getStateEstimator();

protected:
    IStateEstimatorDevice(core::EventHubPtr eventHub,
			  std::string name = "UNNAMED");

    IStateEstimatorPtr m_stateEstimator;
};

} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_ISTATEESTIMATORDEVICE_12_17_2009
