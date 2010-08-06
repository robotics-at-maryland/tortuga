/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/device/PosVelStateEstimator.h
 */

#ifndef RAM_VEHICLE_DEVICE_POSVELSTATEESTIMATOR_01_13_2010
#define RAM_VEHICLE_DEVICE_POSVELSTATEESTIMATOR_01_13_2010

// Project Includes
#include "vehicle/include/device/LoopStateEstimator.h"

namespace ram {
namespace vehicle {
namespace device {

class RAM_EXPORT PosVelStateEstimator : public LoopStateEstimator
{
public:
    virtual ~PosVelStateEstimator() {}

    virtual int velocityUpdate(math::Vector2 velocity,
			       double timeStamp);

    virtual int positionUpdate(math::Vector2 position,
			       double timeStamp);

protected:
    PosVelStateEstimator(core::ConfigNode config,
			 core::EventHubPtr eventHub = core::EventHubPtr(),
			 IVehiclePtr vehicle = IVehiclePtr());
};

} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_POSVELSTATEESTIMATOR_01_13_2010
