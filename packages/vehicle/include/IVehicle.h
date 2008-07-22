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
//#include "core/include/IUpdatable.h"
#include "core/include/Subsystem.h"
#include "core/include/Event.h"
#include "math/include/Vector3.h"

// Must Be Included last
#include "vehicle/include/Export.h"

namespace ram {
namespace vehicle {
    
/** The Generic Vehicle Subsystem interface
 * 
 *  This provides the minimum amount of information need to control the 
 *  vehicle. Additional information for monitoring of vehicle health and 
 *  additional control functions are avaible by querying its devices.
 */
class RAM_EXPORT IVehicle : public core::Subsystem
{
public:
    /**
     * \defgroup EventsOut IVehicle Generated Events
     */
    /* @{ */
    
    /** When the vehicles orientation changes (ram::math:;OrientationEvent)*/
    static const core::Event::EventType ORIENTATION_UPDATE;
    
    /** When the vehicles linear accel. changes (ram::math::Vector3Event) */
    //static const core::Event::EventType LINEAR_ACCEL_UPDATE;
    
    /** When the vehicles angular rate changes (ram::math::Vector3Event) */
    //static const core::Event::EventType ANGULAR_RATE_UPDATE;
    
    /** When the vehicles depth changes changes (ram::math::NumericEvent) */
    static const core::Event::EventType DEPTH_UPDATE;
    
    /* @{ */

    
    virtual ~IVehicle();

    /** Gets the device associated with the given name */
    virtual device::IDevicePtr getDevice(std::string name);

    /** The name of all current devices of the vehicle */
    virtual std::vector<std::string> getDeviceNames() = 0;
    
    /** Return the current vehicle depth in feet */
    virtual double getDepth() = 0;
    
    /** The linear of acceleration (w/gravity) in the vehicle's local frame */
    virtual math::Vector3 getLinearAcceleration() = 0;

    virtual math::Vector3 getAngularRate() = 0;
    
    /** The orientation of the vehicle relative to North with zero roll */
    virtual math::Quaternion getOrientation() = 0;

    /** Combines the given force and torque into motor forces the applies them

        @note   All force in <b>NEWTONS</b> and applied in Vehicle's local
                coordinate frame.  This means, a force of [1,0,0] will cause
                produce positive thrust on port and starboard thrusters. This
                makes the vehicle go forward.
        
        @param  force   Translation force vector
        @param  torque  Rotational torque vector
     */
    virtual void applyForcesAndTorques(const math::Vector3& force,
                                       const math::Vector3& torque) = 0;


    // These should not be here, but since the property interface is not done
    // yet, they will have to stay
    /** Truns <b>ON</b> the thruster safety */
    virtual void safeThrusters() = 0;
 
    /** Turns <b>OFF</b> the thruster safety */
    virtual void unsafeThrusters() = 0;

    /** Drop a marker */
    virtual void dropMarker() = 0;
protected:
    IVehicle(std::string name,
             core::EventHubPtr eventHub = core::EventHubPtr());
};
    
} // namespace vehicle
} // namespace ram
    
#endif // RAM_VEHICLE_IVEHICLE_06_23_2006
