/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vehicle/include/device/IDVL.h
 */

#ifndef RAM_VEHICLE_DEVICE_IDVL_11_20_2009
#define RAM_VEHICLE_DEVICE_IDVL_11_20_2009

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "math/include/Vector2.h"

// Must be Included last
#include "vehicle/include/Export.h"

// Temporary until api is finished
typedef struct _RawDVLData
{
    int temp; // temporary value that doesn't do anything
} RawDVLData;

namespace ram {
namespace vehicle {
namespace device {

/** Abstract interface for Doppler Velocity Logger */
class RAM_EXPORT IDVL : public IDevice
{
public:
    static const core::Event::EventType UPDATE;

    virtual ~IDVL();

    // TODO: Learn what else the DVL can do

    virtual double getDepth() = 0;

    virtual math::Vector2 getVelocity() = 0;

protected:
    IDVL(core::EventHubPtr eventHub = core::EventHubPtr(),
         std::string name = "UNNAMED");
};

} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IDVL_11_20_2009
