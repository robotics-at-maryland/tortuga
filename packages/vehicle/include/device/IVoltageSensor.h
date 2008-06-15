/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IVoltageSensor.h
 */

#ifndef RAM_VEHICLE_DEVICE_IVOLTAGESENSOR_06_15_2008
#define RAM_VEHICLE_DEVICE_IVOLTAGESENSOR_06_15_2008

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {
    
class RAM_EXPORT IVoltageSensor 
{
public:
    /** Fired when the voltage updates */
    static const core::Event::EventType UPDATE;
    
    virtual ~IVoltageSensor();

    virtual double getVoltage() = 0;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IVOLTAGESENSOR_06_15_2008
