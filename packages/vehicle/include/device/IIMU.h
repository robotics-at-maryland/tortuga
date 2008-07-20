/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IIMU.h
 */

#ifndef RAM_VEHICLE_DEVICE_IIMU_06_25_2007
#define RAM_VEHICLE_DEVICE_IIMU_06_25_2007

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "math/include/Quaternion.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {
    
class RAM_EXPORT IIMU : public IDevice         // For getName
             // boost::noncopyable
{
public:
    virtual ~IIMU();

    virtual math::Vector3 getLinearAcceleration() = 0;

    virtual math::Vector3 getAngularRate() = 0;
    
    virtual math::Quaternion getOrientation() = 0;
    
protected:
    IIMU(core::EventHubPtr eventHub = core::EventHubPtr());  
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IIMU_06_25_2007
