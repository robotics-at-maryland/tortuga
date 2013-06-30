/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/IController.h
 */

#ifndef RAM_CONTROL_ICONTROLLER_07_03_2007
#define RAM_CONTROL_ICONTROLLER_07_03_2007

// STD Includes
#include <string>

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/Event.h"
#include "core/include/Subsystem.h"

#include "math/include/Quaternion.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"

#include "control/include/Common.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

class IController;
typedef boost::shared_ptr<IController> IControllerPtr;

/** A single interface from which to interact with the low level controller
 *
 *  This is a facade over a set of controllers which handle difference parts
 *  of the vehicles motion.
 */
class RAM_EXPORT IController : public core::Subsystem
{
public:

    /** Sets the desired position and velocity state variables */
    virtual void translate(math::Vector2 position,
                           math::Vector2 velocity = math::Vector2::ZERO,
                           math::Vector2 accel = math::Vector2::ZERO) = 0;

    /** Sets the desired depth and depth change rate state variables */
    virtual void changeDepth(double depth,
                             double depthRate = 0,
                             double depthAccel = 0) = 0;

    /** Sets the desired orientation and angular rate state variables */
    virtual void rotate(math::Quaternion orientation,
                        math::Vector3 angularRate = math::Vector3::ZERO,
                        math::Vector3 angularAccel = math::Vector3::ZERO) = 0;

    /** Yaws the desired vehicle state by the desired number of degrees */
    virtual void yawVehicle(double degrees, double rate) = 0;

    /** Pitches the desired vehicle state by the desired number of degrees */
    virtual void pitchVehicle(double degrees, double rate) = 0;

    /** Rolls the desired vehicle state by the desired number of degrees */
    virtual void rollVehicle(double degrees, double rate) = 0;



    /** Gets desired position in the inertial frame*/
    virtual math::Vector2 getDesiredPosition() = 0;

    /** Gets desired velocity in the inertial frame*/
    virtual math::Vector2 getDesiredVelocity() = 0;

    /** Gets the desired in plane acceleration */
    virtual math::Vector2 getDesiredAccel() = 0;

    /** Gets the current desired orientation */
    virtual math::Quaternion getDesiredOrientation() = 0;

    /** Gets the desired angular rate */
    virtual math::Vector3 getDesiredAngularRate() = 0;

    /** Gets the desired angular acceleration */
    virtual math::Vector3 getDesiredAngularAccel() = 0;

    /** Current desired depth of the sub (uncalibrated units)*/
    virtual double getDesiredDepth() = 0;

    /** Current desired depth rate change */
    virtual double getDesiredDepthRate() = 0;

    /** Current desired depth acceleration */
    virtual double getDesiredDepthAccel() = 0;


    /** Loads current orientation into desired (fixes offset in roll and pitch)
     *
     *  The desired state quaternion will be "level" in horizontal plane, this
     *  will reverse slight offsets in roll and pitch.
     *
     *  @warning
     *      The vehicle should be upright when using this function, otherwise
     *      the interpretation of yaw and upright will be nonsensical.
     */
    virtual void holdCurrentHeading() = 0;

    /** Loads current orientation into the desired orientation */
    virtual void holdCurrentOrientation() = 0;

    /** Loads current position into desired and stays in that position */
    virtual void holdCurrentPosition() = 0;

    /** Makes the current actual depth the desired depth */
    virtual void holdCurrentDepth() = 0;



    /** Returns true if the vehicle is at the desired orientation */
    virtual bool atOrientation() = 0;

    /** Returns true if the vehicle is at the desired position */
    virtual bool atPosition() = 0;
    
    /** Returns true if the vehicle is at the desired velocity */
    virtual bool atVelocity() = 0;

    /** Returns true if the vehicle is at the desired depth */
    virtual bool atDepth() = 0;

    virtual void moveVel(double x, double y, double z)
    {

    }

    /**
     * \defgroup Events IController Events
     */
    /* @{ */
    
    /** When the controller desired depth changes
     *
     *  Type is ram::math::NumbericEvent;
     */
    static const core::Event::EventType DESIRED_DEPTH_UPDATE;

    /** When the desired depth rate changese */
    static const core::Event::EventType DESIRED_DEPTHRATE_UPDATE;

    /** When the desired depth accel changese */
    static const core::Event::EventType DESIRED_DEPTHACCEL_UPDATE;


    /** When the desired orientation changes */
    static const core::Event::EventType DESIRED_ORIENTATION_UPDATE;

    /** When the desired angular rate changes */
    static const core::Event::EventType DESIRED_ANGULARRATE_UPDATE;

    /** When the desired angular acceleration changes */
    static const core::Event::EventType DESIRED_ANGULARACCEL_UPDATE;


    /** When the desired position changes */
    static const core::Event::EventType DESIRED_POSITION_UPDATE;

    /** When the desired velocity changes */
    static const core::Event::EventType DESIRED_VELOCITY_UPDATE;

    /** When the desired linear acceleration changes */
    static const core::Event::EventType DESIRED_LINEARACCEL_UPDATE;


    /** When the vehicle reaches the depth set by the controller
     *
     *  Type is ram::math::NumbericEvent;
     */
    static const core::Event::EventType AT_DEPTH;

    /** When the vehicle reaches the orientation set by the controller
     *
     *  Type is ram::math::OrientationEvent
     */
    static const core::Event::EventType AT_ORIENTATION;

    /** When the vehicle reaches the position set by the controller
     *
     *  Type is ram::math::Vector2Event
     */

    static const core::Event::EventType AT_POSITION;

    /** When the vehicle reaches the velocity set by the controller
     *
     *  Type is ram::math::Vector2Event
     */

    static const core::Event::EventType AT_VELOCITY;

 
    static const core::Event::EventType DEPTH_CONTROL_SIGNAL_UPDATE;
    static const core::Event::EventType TRANSLATION_CONTROL_SIGNAL_UPDATE;
    static const core::Event::EventType ORIENTATION_CONTROL_SIGNAL_UPDATE;

protected:
    IController(std::string name,
                core::EventHubPtr eventHub = core::EventHubPtr());
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_ICONTROLLER_07_03_2007
