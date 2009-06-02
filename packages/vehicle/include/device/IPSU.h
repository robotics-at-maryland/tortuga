/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/IMU.h
 */

#ifndef RAM_VEHICLE_DEVICE_IPSU_07_06_2007
#define RAM_VEHICLE_DEVICE_IPSU_07_06_2007

// STD Includesb
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "vehicle/include/device/Common.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

class RAM_EXPORT IPSU : public IDevice          // For getName
            // boost::noncopyable
{
public:
    virtual ~IPSU();

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
    
protected:
    IPSU(core::EventHubPtr eventHub = core::EventHubPtr(),
         std::string name = "UNNAMED");
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_IPSU_07_06_2007
