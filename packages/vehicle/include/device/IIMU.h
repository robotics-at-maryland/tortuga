/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/include/device/IIMU.h
 */

#ifndef RAM_VEHICLE_DEVICE_IIMU_06_25_2007
#define RAM_VEHICLE_DEVICE_IIMU_06_25_2007

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "math/include/Quaternion.h"
#include "math/include/Vector3.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

/** Abstract interface for Intertial Measurement Units */    
class RAM_EXPORT IIMU : public IDevice         // For getName
             // boost::noncopyable
{
public:
    static const core::Event::EventType UPDATE;
    static const core::Event::EventType RAW_UPDATE;
    static const core::Event::EventType INIT;
    
    virtual ~IIMU();

    virtual math::Vector3 getLinearAcceleration() = 0;

    virtual math::Vector3 getMagnetometer() = 0;
    
    virtual math::Vector3 getAngularRate() = 0;

    /** The orientation derived from the current mag & accel data */
    virtual math::Quaternion getOrientation() = 0;
    
protected:
    IIMU(core::EventHubPtr eventHub = core::EventHubPtr(),
         std::string name = "UNNAMED");
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IIMU_06_25_2007
