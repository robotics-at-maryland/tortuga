/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/SBMarkerDropper.h
 */

#ifndef RAM_VEHICLE_DEVICE_SBMARKERDROPPER_07_21_2008
#define RAM_VEHICLE_DEVICE_SBMARKERDROPPER_07_21_2008

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

/** Implements the IPayloadSet with the SensorBoard class to drop markers */
class RAM_EXPORT SBMarkerDropper :
        public Device, // for getName
        public IPayloadSet
        // boost::noncopyable
{
public:
    SBMarkerDropper(core::ConfigNode config,
                    core::EventHubPtr eventHub = core::EventHubPtr(),
                    IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SBMarkerDropper();

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

    /** MARKERDROPPER_UPDATE event connection */
    //core::EventConnectionPtr m_connection;

    /** Number of markers dropped */
    int m_markersDropped;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_SBMARKERDROPPER_07_21_2008
