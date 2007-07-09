/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Vehicle.h
 */

#ifndef RAM_VEHICLE_IVEHICLE_06_23_2006
#define RAM_VEHICLE_IVEHICLE_06_23_2006

// Project Includes
#include "vehicle/include/Common.h"
#include "core/include/IUpdatable.h"
#include "math/include/Math.h"

namespace ram {
namespace vehicle {

class IVehicle : public core::IUpdatable
{
public:
    /** Gets the device associated with the given name */
    virtual device::IDevicePtr getDevice(std::string name) = 0;

    virtual double getDepth() = 0;

    /** Truns <b>ON</b> the thruster safety */
    virtual void safeThruster() = 0;

    /** Turns <b>OFF</b> the thruster safety */
    virtual void unsafeThrusters() = 0;
};
    
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_IVEHICLE_06_23_2006
