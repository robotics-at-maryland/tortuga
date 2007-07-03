/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/IDevice.h
 */

#ifndef RAM_VEHICLE_DEVICE_IDEVICE_06_22_2007
#define RAM_VEHICLE_DEVICE_IDEVICE_06_22_2007

// Library Includes
#include <boost/utility.hpp>

// Project Includes
#include "core/include/IUpdatable.h"
#include "vehicle/include/Common.h"

namespace ram {
namespace vehicle {
namespace device {

class IDevice : public core::IUpdatable, boost::noncopyable
{
public:
    virtual ~IDevice() {};

    virtual std::string getName() = 0;

    virtual Vehicle* getVehicle() = 0;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

//sizeof(boost::shared_ptr<ram::vehicle::device::IDevice>);

#endif // RAM_VEHICLE_DEVICE_IDEVICE_06_22_2007
