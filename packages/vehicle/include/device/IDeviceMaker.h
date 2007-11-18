/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/include/device/IDeviceMaker.h
 */
        
#ifndef RAM_VEHICLE_IDEVICEMAKER_10_29_2007
#define	RAM_VEHICLE_IDEVICEMAKER_10_29_2007

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "pattern/include/Maker.h"
#include "core/include/ConfigNodeKeyExtractor.h"
        
namespace ram {
namespace vehicle {
namespace device {

typedef ram::pattern::Maker<IDevice*, // The type of object created by the maker
              ram::core::ConfigNode,  // The parameter used to create the object
              std::string,            // The type of key used to register makers
              ram::core::ConfigNodeKeyExtractor> // Gets the key from the paramters
IDeviceMaker;

template<class DeviceType>
struct IDeviceMakerTemplate : public IDeviceMaker
{
    IDeviceMakerTemplate(std::string deviceType) : IDeviceMaker(deviceType) {};
    
    virtual ram::vehicle::device::IDevice* makeObject(ram::core::ConfigNode config)
    {
        return new DeviceType(config);
    }
};

} // namespace device 
} // namespace vehicle
} // namespace ram

// This macro generates a simple class which uses the above template
#define RAM_VEHILCE_REGISTER_IDEVICE_MAKER(TYPE, NAME) \
struct NAME ## IDeviceMaker : \
    public ram::vehicle::device::IDeviceMakerTemplate<TYPE> \
{ \
    static NAME ## IDeviceMaker registerThis; \
    NAME ## IDeviceMaker () : \
        ram::vehicle::device::IDeviceMakerTemplate<TYPE>(# NAME) {}; \
}; \
NAME ## IDeviceMaker NAME ## IDeviceMaker::registerThis;


#endif	// RAM_VEHICLE_IDEVICEMAKER_10_29_2007

