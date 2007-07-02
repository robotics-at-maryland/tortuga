/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/Device.h
 */

#ifndef RAM_VEHICLE_DEVICE_DEVICE_06_25_2007
#define RAM_VEHICLE_DEVICE_DEVICE_06_25_2007

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"

namespace ram {
namespace vehicle {
namespace device {

class Device : public IDevice // boost::noncopyable
{
public:
    Device(VehiclePtr vehicle, std::string name);
    virtual ~Device() {};

    virtual std::string getName();

    virtual VehiclePtr getVehicle();
    
private:
    std::string m_name;
    VehiclePtr m_vehicle;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_DEVICE_06_25_2007
