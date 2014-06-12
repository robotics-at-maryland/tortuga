/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vehicle/include/device/IDevice.h
 */

#ifndef RAM_VEHICLE_DEVICE_IDEVICE_06_22_2007
#define RAM_VEHICLE_DEVICE_IDEVICE_06_22_2007

// STD Includes
#include <cassert>
#include <string>

// Library Includes
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/EventPublisher.h"
#include "core/include/IUpdatable.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
namespace device {

class RAM_EXPORT IDevice : public core::IUpdatable,
                           public core::EventPublisher,
                           boost::noncopyable
{
public:
    virtual ~IDevice();

    virtual std::string getName() = 0;

    template<class T>
    static boost::shared_ptr<T> castTo(boost::shared_ptr<IDevice> device)
    {
        boost::shared_ptr<T> ptr = boost::dynamic_pointer_cast<T>(device);
        assert(ptr && "Failed to cast IDevice pointer");
        return ptr;
    }
    
    template<class T>
    static T* castTo(IDevice* device)
    {
        T* ptr = dynamic_cast<T*>(device);
        assert(ptr && "Failed to cast IDevice pointer");
        return ptr;
    }
    void setcor(bool c)
    {
        corflg = c;
    }


protected:
    IDevice(core::EventHubPtr eventHub, std::string name = "UNNAMED");
    bool corflg;//this flag controls if an imu is corrupt

};
    
} // namespace device
} // namespace vehicle
} // namespace ram

//sizeof(boost::shared_ptr<ram::vehicle::device::IDevice>);

#endif // RAM_VEHICLE_DEVICE_IDEVICE_06_22_2007
