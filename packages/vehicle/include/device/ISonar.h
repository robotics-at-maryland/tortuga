/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/ISONAR.h
 */

#ifndef RAM_VEHICLE_DEVICE_ISONAR_07_19_2008
#define RAM_VEHICLE_DEVICE_ISONAR_07_19_2008

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "math/include/Vector3.h"
#include "core/include/TimeVal.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {
    
class RAM_EXPORT ISonar : public IDevice  // For getName
             // boost::noncopyable
{
public:
    /** Generated when we get a new ping (ram::vehicle::SonarEvent) */
    static const core::Event::EventType UPDATE;
    
    virtual ~ISonar();

    /** The direction from the vehicle to the pinger */
    virtual math::Vector3 getDirection() = 0;

    /** NOT IMPLEMENTED */
    virtual double getRange() = 0;

    /** The time the ping was heard */
    virtual core::TimeVal getPingTime() = 0;
    
protected:
    ISonar(core::EventHubPtr eventHub = core::EventHubPtr());  
};

} // namespace device
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_DEVICE_ISONAR_07_19_2008
