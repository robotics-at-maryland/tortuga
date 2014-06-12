/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IVelocitySensor.h
 */

#ifndef RAM_VEHICLE_DEVICE_IVELOCITYSENSOR_06_24_2009
#define RAM_VEHICLE_DEVICE_IVELOCITYSENSOR_06_24_2009

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {
    
/** Represents a sensor that returns the velocity in feet */
class RAM_EXPORT IVelocitySensor : public IDevice // For getName
             // boost::noncopyable
{
public:
    static const core::Event::EventType UPDATE;
    static const core::Event::EventType RAW_UPDATE;
    static const core::Event::EventType INIT;
    static const core::Event::EventType RAW_RANGE_UPDATE;

    virtual ~IVelocitySensor();

    /** The location of the velocity sensor on the vehicle */
    virtual math::Vector3 getLocation() = 0;
    
protected:
    IVelocitySensor(core::EventHubPtr eventHub = core::EventHubPtr(),
                    std::string name = "UNNAMED");
};

} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IVELOCITYSENSOR_06_24_2009
