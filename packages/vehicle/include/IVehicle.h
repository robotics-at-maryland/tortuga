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

// STD Includes
#include <string>

// Boost Includes
#include <boost/array.hpp>

// Project Includes
#include "vehicle/include/Common.h"
#include "core/include/IUpdatable.h"
#include "math/include/Vector3.h"

namespace ram {
namespace vehicle {

class IVehicle : public core::IUpdatable
{
public:
    /** Gets the device associated with the given name */
    virtual device::IDevicePtr getDevice(std::string name) = 0;

    /** Return the current vehicle depth in feet */
    virtual double getDepth() = 0;

    /** Returns an array of temperature sensor names */
    virtual std::vector<std::string> getTemperatureNames() = 0;

    /** Returns the array of temperature sensor values */
    virtual std::vector<int> getTemperatures() = 0;

    virtual math::Vector3 getLinearAcceleration() = 0;

    virtual math::Vector3 getAngularRate() = 0;
    
    virtual math::Quaternion getOrientation() = 0;
    
    /** Truns <b>ON</b> the thruster safety */
    virtual void safeThrusters() = 0;

    /** Turns <b>OFF</b> the thruster safety */
    virtual void unsafeThrusters() = 0;
    
    /** Release Marker */
    virtual void dropMarker() = 0;

    /** Returns 1 if the start switch is down 0 if its up */
    virtual int startStatus() = 0;

    /** Prints a line to the vehicle LCD screen */
    virtual void printLine(int line, std::string text) = 0;

    /** Combines the given force and torque into motor forces the applies them

        @note   All force in <b>NEWTONS</b>
        
        @param  force   Translation force vector
        @param  torque  Rotational torque vector
     */
    virtual void applyForcesAndTorques(math::Vector3& force,
                                       math::Vector3& torque) = 0;
};
    
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_IVEHICLE_06_23_2006
