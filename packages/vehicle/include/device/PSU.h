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

#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/ConfigNode.h"
#include "pattern/include/Subject.h"

// Forward declare structure from imuapi.h
struct ctxValues;
struct usb_dev_handle;
typedef struct usb_dev_handle usb_dev_handle;


namespace ram {
namespace vehicle {
namespace device {

class PSU : public Device,          // For getName
            public core::Updatable, // for update
            public pattern::Subject // so other objects can watch
            // boost::noncopyable
{
public:
    enum UpdateEvents {
        DataUpdate
    };
    
    /** Create an IMU with the given device file */
    PSU(IVehicle* vehicle, core::ConfigNode config);

    /** Creats a new object */
    static PSUPtr construct(IVehicle* vehicle, core::ConfigNode config);

    /** Preforms a cast to the desired type */
    static PSUPtr castTo(IDevicePtr ptr);
//    static PSU* castTo(IDevice* ptr);
    
    virtual ~PSU();

    /** This is called at the desired interval to read data from the IMU */
    virtual void update(double timestep);

    
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
