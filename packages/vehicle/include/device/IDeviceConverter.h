/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/vehicle/include/device/IDeviceConverter.h
 */

#ifndef RAM_CORE_IDEVICECONVERTER_H_01_13_2008
#define RAM_CORE_IDEVICECONVERTER_H_01_13_2008

// Project Includes
#include "core/include/ToPythonConverter.h"
#include "vehicle/include/device/IDevice.h"

namespace ram {
namespace vehicle {
namespace device {

typedef core::ToPythonConverter<IDevice> IDeviceConverter;
    
#ifdef RAM_WINDOWS
template class RAM_EXPORT core::ToPythonConverter<IDevice>;
#endif
    
template<class T>
class SpecificIDeviceConverter :
        public core::SpecificToPythonConverter<T, IDevice>
{
};

} // namespace device
} // namespace vehicle
} // namespace ram
    
#endif // RAM_CORE_IDEVICECONVERTER_H_01_13_2008
