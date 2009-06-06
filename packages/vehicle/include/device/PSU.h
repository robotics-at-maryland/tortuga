/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IMU.h
 */

#ifndef RAM_VEHICLE_DEVICE_PSU_07_06_2007
#define RAM_VEHICLE_DEVICE_PSU_07_06_2007

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/Device.h"
#include "vehicle/include/device/IPSU.h"

#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/ConfigNode.h"

// Forward declare structure from imuapi.h
struct ctxValues;
struct usb_dev_handle;
typedef struct usb_dev_handle usb_dev_handle;


namespace ram {
namespace vehicle {
namespace device {

class PSU;
typedef boost::shared_ptr<PSU> PSUPtr;
    
class PSU : public IPSU,
            public Device,         // For getName
            public core::Updatable // for update
            // boost::noncopyable
{
public:
    enum UpdateEvents {
        DataUpdate
    };

    virtual std::string getName() { return Device::getName(); }
    
    /** Create an IMU with the given device file */
    PSU(core::ConfigNode config,
        core::EventHubPtr eventHub = core::EventHubPtr(),
        IVehiclePtr vehicle = IVehiclePtr());

    virtual ~PSU();

    /** This is called at the desired interval to read data from the IMU */
    virtual void update(double timestep);

    virtual void setPriority(core::IUpdatable::Priority priority) {
        Updatable::setPriority(priority);
    }
    
    virtual core::IUpdatable::Priority getPriority() {
        return Updatable::getPriority();
    }

    virtual void setAffinity(size_t affinity) {
        Updatable::setAffinity(affinity);
    }
    
    virtual int getAffinity() {
        return Updatable::getAffinity();
    }
    
    virtual void background(int interval) {
        Updatable::background(interval);
    };
    
    virtual void unbackground(bool join = false) {
        Updatable::unbackground(join);
    };
    virtual bool backgrounded() {
        return Updatable::backgrounded();
    };
    
    /** Gets the voltage from the main batteries*/
    double getBatteryVoltage();

    /** Gest the currenty from the main batteries */
    double getBatteryCurrent();

    /** Gets the watts being drawn from the batteries */
    double getTotalWattage();

    
    /** List of all voltages ouputs from the power supply */
    VoltageList getVoltages();

    /** Gets the current output for each output supply */
    CurrentList getCurrents();

    /** Gets the power draw of each output on the supply */
    WattageList getWattages();
    
    /** The names of all output supplies */
    StringList getSupplyNames();
    
    //void getRawState(RawIMUData& imuState);
    
private:
    /** Config for the vehicle */
    core::ConfigNode m_config;

    /** Protects access to raw state */
    core::ReadWriteMutex m_valuesMutex;
    
    /** The raw data read back from the IMU */
    ctxValues* m_ctxValues;

    /** Carnetix device handle */
    usb_dev_handle* m_deviceHandle;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_PSU_07_06_2007
