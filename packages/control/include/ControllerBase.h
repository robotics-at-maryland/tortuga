/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/ControllerBase.h
 */

#ifndef RAM_CONTROL_BWDCONTROLER_07_07_2007
#define RAM_CONTROL_BWDCONTROLER_07_07_2007

// Project Includes
#include "control/include/Common.h"
#include "control/include/IController.h"
#include "control/include/DesiredState.h"
#include "control/include/Helpers.h"

#include "vehicle/include/Common.h"

#include "core/include/ConfigNode.h"
#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "control/include/Export.h"
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"

namespace ram {
namespace control {

/** Tolerance for at Depth (1 foot in meters) */
static const double DEPTH_TOLERANCE = 0.5;

/** About a 2.5 degree tolerance */
static const double ORIENTATION_THRESHOLD = 0.03;

static const double VELOCITY_THRESHOLD = 0.05;
static const double POSITION_THRESHOLD = 0.5;
    
/** Base for controllers, assists in properly implementing the IController*/
class RAM_EXPORT ControllerBase : public IController,
                                  public core::Updatable
{
public:
    ControllerBase(vehicle::IVehiclePtr vehicle, core::ConfigNode config);

    ControllerBase(core::ConfigNode config,
                   core::SubsystemList deps = core::SubsystemList());

    virtual ~ControllerBase();

    /** Does all the grunt work of the actual updating, see doUpdate
     *
     *  The actual work is done by subclasses who implement the doUpdate method
     */
    virtual void update(double timestep);


    virtual void setVelocity(math::Vector2 velocity);  

    /** Get the current desired velocity */
    virtual math::Vector2 getVelocity();
    
    /** Set the current speed, clamped between -5 and 5
     *
     *  Setting this turns off the velocity based control, and gives direct
     *  speed based control.
     */
    virtual void setSpeed(double speed);

    /** Set how fast the vehicle is going side to side (positive = right) */
    virtual void setSidewaysSpeed(double speed);

    /** Gets the current speed, a value between -5 and 5 */
    virtual double getSpeed();

    /** Gets the current sideways speed
     *
     *  @return
     *      A value between -5 (left) and 5 (right)
     */
    virtual double getSidewaysSpeed();

    /** Loads current position into desired and stays in that position */
    virtual void holdCurrentPosition();

    /** Sets desired velocity and velocity based control for new controllers */
    virtual void setDesiredVelocity(math::Vector2 velocity, Frame frame);
    
    /** Sets desired position and position based control for new controllers */
    virtual void setDesiredPosition(math::Vector2 position, Frame frame);
 
    /** Sets a desired position and velocity for controling of both simultaneously */
    virtual void setDesiredPositionAndVelocity(math::Vector2 position,
					       math::Vector2 velocity);

    /** Gets desired velocity */
    virtual math::Vector2 getDesiredVelocity(Frame frame);

    /** Gets desired position */
    virtual math::Vector2 getDesiredPosition(Frame frame);

    virtual bool atPosition();
    
    virtual bool atVelocity();

    /** Yaws the desired vehicle state by the desired number of degrees */
    virtual void yawVehicle(double degrees);

    /** Pitches the desired vehicle state by the desired number of degrees */
    virtual void pitchVehicle(double degrees);

    /** Rolls the desired vehicle state by the desired number of degrees */
    virtual void rollVehicle(double degrees);

    /** Gets the current desired orientation */
    virtual math::Quaternion getDesiredOrientation();
    
    /** Sets the current desired orientation */
    virtual void setDesiredOrientation(math::Quaternion);
    
    /** Returns true if the vehicle is at the desired orientation */
    virtual bool atOrientation();

    /** Sets the desired depth of the sub in meters */
    virtual void setDepth(double depth);

    /** Current desired depth of the sub in meters */
    virtual double getDepth();
    
    /** Grab current estimated depth*/
    virtual double getEstimatedDepth();
    
    /** Grab current estimated depth velocity (depthDot)*/
    virtual double getEstimatedDepthDot();
    
    /** Returns true if the vehicle is at the desired depth */
    virtual bool atDepth();

    /** Makes the current actual depth the desired depth */
    virtual void holdCurrentDepth();

    /** Loads current orientation into desired (fixes offset in roll and pitch)
     *
     *  The desired state quaternion will be "level" in horizontal plane, this
     *  will reverse slight offsets in roll and pitch.
     *
     *  @warning
     *      The vehicle should be upright when using this function, otherwise
     *      the interpretation of yaw and upright will be nonsensical.
     */
    virtual void holdCurrentOrientation();
    virtual void holdCurrentHeading();

    virtual void setPriority(core::IUpdatable::Priority priority) {
        Updatable::setPriority(priority);
    }
    
    virtual core::IUpdatable::Priority getPriority() {
        return Updatable::getPriority();
    }

    virtual void setAffinity(size_t affinity) {
        Updatable::setAffinity(affinity);
    }
    
    virtual int getAffinity() {
        return Updatable::getAffinity();
    }
    
    virtual void background(int interval) {
        Updatable::background(interval);
    }
    
    virtual void unbackground(bool join = false) {
        Updatable::unbackground(join);
    }

    virtual bool backgrounded() {
        return Updatable::backgrounded();
    }

protected:
    /** Implemeted by subclasses, does actual controller work
     *
     *  @param timestep
     *      The time sice the last update
     *  @param linearAcceleration
     *      The current linear acceleration from the vehicle (m/s).
     *  @param orientation
     *      The current orientation of the vehicle (from north).
     *  @param angularRate
     *      The current angularRate of the vehicle (radians/sec)
     *  @param depth
     *      The current depth of the vehicle (feet)
     *  @param position
     *      The current position of the vehicle
     *  @param velocity
     *      The current velocity of the vehicle
     *  @param translationalForceOut
     *      The new force to apply to the vehicle (newtons)
     *  @param rotationalTorqueOut
     *      The new torque to apply to the vehicle (newtons)
     */
    virtual void doUpdate(const double& timestep,
                          const math::Vector3& linearAcceleration,
                          const math::Quaternion& orientation,
                          const math::Vector3& angularRate,
                          const double& depth,
                          const math::Vector2& position,
                          const math::Vector2& velocity,
                          math::Vector3& translationalForceOut,
                          math::Vector3& rotationalTorqueOut) = 0;

    /** Called whenever a new depth is set, handles events as needed
     *
     *  This will always publish the DESIRED_DEPTH_UPDATE event and also
     *  publish the AT_DEPTH event if needed. This should be called inside the
     *  IController::setDepth method.
     */
    void newDepthSet(const double& newDepth);

    /** Called whenever the desired orientation changes, handles events
     *
     *  This will always publish the DESIRED_ORIENTATION_UPDATE event and also
     *  publish the AT_ORIENTATION event if needed. This should be called
     *  inside the IController::setDesiredOrientation,
     *  IController::rollVehicle, IController::pitchVehicle, and
     *  IController::yawVehicle methods.
     */
    void newDesiredOrientationSet(const math::Quaternion& newOrientation);
   
    /** Called whenever the desired velocity changes, handles events
     *
     * This publishes the DESIRED_VELOCITY_UPDATE event and will
     * publish the AT_VELOCITY event if needed.  This should be called
     * inside the IController::setDesiredVelocity and the
     * IController::setDesiredPositionAndVelocity function
     */
    void newDesiredVelocitySet(const math::Vector2& newVelocity);

    /** Called whenever the desired velocity changes, handles events
     *
     * This publishes the DESIRED_POSITION_UPDATE event and will
     * publish the AT_POSITION event if needed.  This should be called
     * inside the IController::setDesiredVelocity and the
     * IController::setDesiredPositionAndVelocity function
     */
    void newDesiredPositionSet(const math::Vector2& newPosition);

    controltest::DesiredStatePtr desiredState;

private:
    void init(core::ConfigNode config);

    void publishAtDepth(const double& depth);
    void publishAtOrientation(const math::Quaternion& orientation);
    void publishAtVelocity(const math::Vector2& velocity);
    void publishAtPosition(const math::Vector2& position);
    
    /** Used to maintain state so we don't issue continuous AT_*** at depth updates */
    bool m_atDepth;
    bool m_atOrientation;
    bool m_atVelocity;
    bool m_atPosition;
    
    /** When we are within this limit we send off the AT_*** event */
    double m_depthThreshold;
    double m_orientationThreshold;
    double m_velocityThreshold;
    double m_positionThreshold;
    
    /** Out Vehicle */
    vehicle::IVehiclePtr m_vehicle;    
    
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_BWDCONTROLER_07_07_2007

