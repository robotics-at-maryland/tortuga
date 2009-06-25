/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IPositionSensor.h
 */

#ifndef RAM_VEHICLE_DEVICE_IPOSITIONSENSOR_06_24_2009
#define RAM_VEHICLE_DEVICE_IPOSITIONSENSOR_06_24_2009

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
    
/** Represents a sensor that returns the position in meters */
class RAM_EXPORT IPositionSensor : public IDevice // For getName
             // boost::noncopyable
{
public:
    virtual ~IPositionSensor();

    /** The current position the sensor is reading in meters */
    virtual math::Vector2 getPosition() = 0;

    /** The location of the position sensor on the vehicle */
    virtual math::Vector3 getLocation() = 0;
    
protected:
    IPositionSensor(core::EventHubPtr eventHub = core::EventHubPtr(),
                    std::string name = "UNNAMED");
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IPOSITIONSENSOR_06_24_2009
