/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/src/device/DeviceMakers.cpp
 */

// All registration are done in one file to keep the compilation dependency on 
// the factory system to a minimum

// Project Includes
#include "core/include/Feature.h"

#include "vehicle/include/device/IDeviceMaker.h"
#include "vehicle/include/device/Thruster.h"


#ifdef RAM_WITH_DRIVERS
#include "vehicle/include/device/IMU.h"
#include "vehicle/include/device/PSU.h"
#include "vehicle/include/device/SensorBoard.h"
#include "vehicle/include/device/SBPowerSource.h"
#endif

// Register each device with the factor system
#ifdef RAM_WITH_DRIVERS
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(ram::vehicle::device::IMU, IMU);
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(ram::vehicle::device::PSU, PSU);

RAM_VEHILCE_REGISTER_IDEVICE_MAKER(ram::vehicle::device::SensorBoard, SensorBoard);
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(ram::vehicle::device::SBPowerSource, SBPowerSource);
#endif
RAM_VEHILCE_REGISTER_IDEVICE_MAKER(ram::vehicle::device::Thruster, Thruster);



// In this case the static variable is safe
#if RAM_COMPILER == RAM_COMPILER_MSVC
#  pragma warning( disable : 4640 )
#endif

namespace ram {
namespace pattern {

template<>
vehicle::device::IDeviceMaker::MakerMap*
vehicle::device::IDeviceMaker::getRegistry()
{
    // This line is run only once, avoids static initialization order issue
    static vehicle::device::IDeviceMaker::MakerMap* reg =
        new vehicle::device::IDeviceMaker::MakerMap();
    return reg;
}

} // namespace pattern
} // namespace ram
