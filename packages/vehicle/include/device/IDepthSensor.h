/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IDepthSensor.h
 */

#ifndef RAM_VEHICLE_DEVICE_IDEPTHSENSOR_06_05_2008
#define RAM_VEHICLE_DEVICE_IDEPTHSENSOR_06_05_2008

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {
    
class RAM_EXPORT IDepthSensor : public IDevice         // For getName
             // boost::noncopyable
{
public:
    virtual ~IDepthSensor();

    virtual double getDepth() = 0;
    
protected:
    IDepthSensor(core::EventHubPtr eventHub = core::EventHubPtr());  
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IDEPTHSENSOR_06_05_2008
