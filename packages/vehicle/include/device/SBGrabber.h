/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/device/SBGrabber.h
 */

#ifndef RAM_VEHICLE_DEVICE_SBGRABBER_06_30_2010
#define RAM_VEHICLE_DEVICE_SBGRABBER_06_30_2010

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/Common.h"
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IPayloadSet.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

/** Implements the IPayloadSet with the SensorBoard class to fire torpedos */
class RAM_EXPORT SBGrabber :
        public Device, // for getName
        public IPayloadSet
        // boost::noncopyable
{
public:
    SBGrabber(core::ConfigNode config,
              core::EventHubPtr eventHub = core::EventHubPtr(),
              IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SBGrabber();

    // IPayloadSet methods
    virtual void releaseObject();
    virtual int objectCount();
    virtual int initialObjectCount();

    // IDevice methods
    virtual std::string getName() { return Device::getName(); }

    // IUpdatable methods
    virtual void update(double timestep) {}

    virtual void setPriority(core::IUpdatable::Priority) {}

    virtual core::IUpdatable::Priority getPriority() {
        return IUpdatable::NORMAL_PRIORITY;
    }

    virtual void setAffinity(size_t) {};
    
    virtual int getAffinity() {
        return -1;
    };
    
    virtual void background(int interval) {
        //Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        //Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return true;
        //return Updatable::backgrounded();
    };
    
private:
    /** Sensor Board from which to access the hardware */
    SensorBoardPtr m_sensorBoard;

    /** Whether the grabber has released or not, updated by releaseObject */
    int m_released;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_SBGRABBER_06_30_2010
