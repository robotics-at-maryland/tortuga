/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IPowerBus.h
 */

#ifndef RAM_VEHICLE_DEVICE_IPOWERBUS_10_04_2008
#define RAM_VEHICLE_DEVICE_IPOWERBUS_10_04_2008

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/IVoltageProvider.h"
#include "vehicle/include/device/ICurrentProvider.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {
    
class RAM_EXPORT IPowerBus :
        public IVoltageProvider,
        public ICurrentProvider,
        public IDevice
             // boost::noncopyable
{
public:
    virtual ~IPowerBus();

protected:
    IPowerBus(core::EventHubPtr eventHub = core::EventHubPtr());  
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IPOWERBUS_10_04_2008
