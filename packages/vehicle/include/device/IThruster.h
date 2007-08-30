/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/device/Thruster.h
 */

#ifndef RAM_VEHICLE_DEVICE_THRUSTER_06_25_2007
#define RAM_VEHICLE_DEVICE_THRUSTER_06_25_2007

// STD Includes
#include <string>

// Project Includes
#include "vehicle/include/device/IDevice.h"
#include "pattern/include/Subject.h"

namespace ram {
namespace vehicle {
namespace device {

class IThruster : public IDevice, // boost::noncopyable
                  public pattern::Subject
{
public:
    enum UpdateEvents {
        FORCE_UPDATE
    };
    
    virtual ~IThruster() {};
    
    /** Sets the current thruster force of the thrusters */
    void setForce(double newtons) = 0;

    /** Return the current force the thrusters are outputing in newtons */
    double getForce() = 0;
};
    
} // namespace device
} // namespace vehicle
} // namespace ram

#endif // RAM_VEHICLE_DEVICE_THRUSTER_06_25_2007
