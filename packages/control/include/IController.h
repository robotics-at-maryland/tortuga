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

#include "control/include/Common.h"
#include "control/include/IDepthController.h"
#include "control/include/ITranslationalController.h"
#include "control/include/IRotationalController.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

class IController;
typedef boost::shared_ptr<IController> IControllerPtr;

/** A single interface from which to control a vehicle
 *
 *  This is a facade over a set of controllers which handle difference parts
 *  of the vehicles motion.
 */
class RAM_EXPORT IController : public core::Subsystem
{
public:
    /** Set the current speed, clamped between -5 and 5
     *
     *  Setting this turns off the velocity based control, and gives direct
     *  speed based control.
     */
    virtual void setSpeed(double speed) = 0;

    /** Set how fast the vehicle is going side to side (positive = right) */
    virtual void setSidewaysSpeed(double speed) = 0;

    /** Gets the current speed, a value between -5 and 5 */
    virtual double getSpeed() = 0;

    /** Gets the current sideways speed
     *
     *  @return
     *      A value between -5 (left) and 5 (right)
     */
    virtual double getSidewaysSpeed() = 0;

    /** Loads current position into desired and stays in that position */
    virtual void holdCurrentPosition() = 0;

    /** Sets desired velocity and velocity based control for new controllers */
    virtual void setDesiredVelocity(math::Vector2 velocity, int frame) = 0;
    
    /** Sets desired position and position based control for new controllers */
    virtual void setDesiredPosition(math::Vector2 position, int frame) = 0;
 
    /** Sets a desired position and velocity for controling of both simultaneously */
    virtual void setDesiredPositionAndVelocity(math::Vector2 position,
					       math::Vector2 velocity) = 0;

    /** Gets desired velocity */
    virtual math::Vector2 getDesiredVelocity(int frame) = 0;

    /** Gets desired position */
    virtual math::Vector2 getDesiredPosition(int frame) = 0;

    /** Returns true if the vehicle is at the desired position */
    virtual bool atPosition() = 0;
    
    /** Returns true if the vehicle is at the desired velocity */
    virtual bool atVelocity() = 0;

    /** Yaws the desired vehicle state by the desired number of degrees */
    virtual void yawVehicle(double degrees) = 0;

    /** Pitches the desired vehicle state by the desired number of degrees */
    virtual void pitchVehicle(double degrees) = 0;

    /** Rolls the desired vehicle state by the desired number of degrees */
    virtual void rollVehicle(double degrees) = 0;

    /** Gets the current desired orientation */
    virtual math::Quaternion getDesiredOrientation() = 0;
    
    /** Sets the current desired orientation */
    virtual void setDesiredOrientation(math::Quaternion) = 0;
    
    /** Returns true if the vehicle is at the desired orientation */
    virtual bool atOrientation() = 0;

    /** Sets the desired depth of the sub in meters */
    virtual void setDepth(double depth) = 0;

    /** Current desired depth of the sub in meters */
    virtual double getDepth() = 0;
    
    /** Returns true if the vehicle is at the desired depth */
    virtual bool atDepth() = 0;

    /** Makes the current actual depth the desired depth */
    virtual void holdCurrentDepth() = 0;

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

    /**
     * \defgroup Events IController Events
     */
    /* @{ */
    
    /** When the controller desired depth changes
     *
     *  Type is ram::math::NumbericEvent;
     */
    static const core::Event::EventType DESIRED_DEPTH_UPDATE;

    /** When the desired orientation changes (ram::math::OrientationEvent) */
    static const core::Event::EventType DESIRED_ORIENTATION_UPDATE;

    /** When the desired velocity changes */
    static const core::Event::EventType DESIRED_VELOCITY_UPDATE;

    /** When the desired position changes */
    static const core::Event::EventType DESIRED_POSITION_UPDATE;

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

    /** When the vehicle reaches the velocity set by the controller
     *
     *  Type is ram::math::Vector2Event
     */

    static const core::Event::EventType AT_VELOCITY;

    /** When the vehicle reaches the positionx set by the controller
     *
     *  Type is ram::math::Vector2Event
     */

    static const core::Event::EventType AT_POSITION;


    /** Sent to adjust the display parameter name and count
     *
     *  Type is ram::control::ParamSetupEvent
     */
    static const core::Event::EventType PARAM_SETUP;

    /** Sent to update the value of the displayed parameters
     *
     *  Type is ram::control::ParamUpdateEvent
     */
    static const core::Event::EventType PARAM_UPDATE;

    
    const static int BODY_FRAME;
    const static int INERTIAL_FRAME;


    /* @{ */
    
protected:
    IController(std::string name,
                core::EventHubPtr eventHub = core::EventHubPtr());
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_ICONTROLLER_07_03_2007
