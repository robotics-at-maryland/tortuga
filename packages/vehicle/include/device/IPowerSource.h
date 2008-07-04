/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IPowerSource.h
 */

#ifndef RAM_VEHICLE_DEVICE_IPOWERSOURCE_06_15_2008
#define RAM_VEHICLE_DEVICE_IPOWERSOURCE_06_15_2008

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
    
class RAM_EXPORT IPowerSource :
        public IVoltageProvider,
        public ICurrentProvider,
        public IDevice
             // boost::noncopyable
{
public:
    /** Fired when the power source becomes enabled */
    static const core::Event::EventType ENABLED;
    /** Fired when the power source becomes disabled */
    static const core::Event::EventType DISABLED;
    /** Fired when the power source becomes inUse */
    static const core::Event::EventType USING;
    /** Fired when the power source becomes stops being used */
    static const core::Event::EventType NOT_USING;
    
    virtual ~IPowerSource();

    /** Whether or not the vehicle can draw power from this power source */
    virtual bool isEnabled() = 0;

    /** Whether the vehicle is drawing power from this power source */
    virtual bool inUse() = 0;

    /** Set whether or not the vehicle can draw power from this power source */
    virtual void setEnabled(bool state) = 0;
    
protected:
    IPowerSource(core::EventHubPtr eventHub = core::EventHubPtr());  
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IPOWERSOURCE_06_15_2008
