/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/SBPowerSource.h
 */

#ifndef RAM_VEHICLE_DEVICE_SBPOWERSOURCE_06_15_2008
#define RAM_VEHICLE_DEVICE_SBPOWERSOURCE_06_15_2008

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IPowerSource.h"

// Must Be Included last
#include "vehicle/include/Export.h"
#include "vehicle/include/Common.h"
#include "vehicle/include/device/Device.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace vehicle {
namespace device {

/** Implements the IPowerSource with the SensorBoard class */
class RAM_EXPORT SBPowerSource :
        public Device, // for getName
        public IPowerSource
        // boost::noncopyable
{
public:
    SBPowerSource(core::ConfigNode config,
                  core::EventHubPtr eventHub = core::EventHubPtr(),
                  IVehiclePtr vehicle = IVehiclePtr());
    
    virtual ~SBPowerSource();

    virtual double getCurrent();
    
    virtual double getVoltage();
    
    virtual bool isEnabled();

    //virtual bool inUse() = 0;

    virtual std::string getName() { return Device::getName(); }
    
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
    /** SensorBoard::POWERSOURCE_UPDATE event handler */
    void onPowerSourceUpdate(core::EventPtr event);
    
    /** ID used to identify self in power source update messages */
    int m_id;

    core::ReadWriteMutex m_mutex;
    bool m_enabled;
    double m_voltage;
    double m_current;

    /** Sensor Board from which to access the hardware */
    SensorBoardPtr m_sensorBoard;

    /** POWERSOURCE_UPDATE event connection */
    core::EventConnectionPtr m_connection;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_SBPOWERSOURCE_06_15_2008
