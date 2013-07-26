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

#include "estimation/include/Common.h"
#include "estimation/include/IStateEstimator.h"

#include "vehicle/include/Common.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"

#include "core/include/ConfigNode.h"
#include "core/include/Updatable.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/EventConnection.h"
#include "core/include/EventHub.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

/* Default values for deciding if we are at the desired state */
/** Tolerance for at Depth (1 foot in meters) */
static const double DEPTH_THRESHOLD = 0.5;
/** About a 2.5 degree tolerance */
static const double ORIENTATION_THRESHOLD = 0.03;
static const double VELOCITY_THRESHOLD = 0.1;
static const double POSITION_THRESHOLD = 0.5;
    
/** Base for controllers, assists in properly implementing the IController*/
class RAM_EXPORT ControllerBase : public IController,
                                  public core::Updatable
{
public:
    ControllerBase(core::EventHubPtr eventHub,
                   vehicle::IVehiclePtr vehicle,
                   estimation::IStateEstimatorPtr estimator,
                   core::ConfigNode config);

    ControllerBase(core::ConfigNode config,
                   core::SubsystemList deps = core::SubsystemList());

    virtual ~ControllerBase();

    /** Does all the grunt work of the actual updating, see doUpdate
     *
     *  The actual work is done by subclasses who implement the doUpdate method
     */
    virtual void update(double timestep);



    /** Sets the desired position and velocity state variables */
    virtual void translate(math::Vector2 position,
                           math::Vector2 velocity = math::Vector2::ZERO,
                           math::Vector2 accel = math::Vector2::ZERO);

    /** Sets the desired depth and depth change rate state variables */
    virtual void changeDepth(double depth,
                             double depthRate = 0,
                             double depthAccel = 0);

    /** Sets the desired orientation and angular rate state variables */
    virtual void rotate(math::Quaternion orientation,
                        math::Vector3 angularRate = math::Vector3::ZERO,
                        math::Vector3 angularAccel = math::Vector3::ZERO);

    /** Yaws the desired vehicle state by the desired number of degrees */
    virtual void yawVehicle(double degrees, double rate);

    /** Pitches the desired vehicle state by the desired number of degrees */
    virtual void pitchVehicle(double degrees, double rate);

    /** Rolls the desired vehicle state by the desired number of degrees */
    virtual void rollVehicle(double degrees, double rate);



    /** Gets desired position */
    virtual math::Vector2 getDesiredPosition();

    /** Gets desired velocity */
    virtual math::Vector2 getDesiredVelocity();

    /** Gets the desired in plane acceleration */
    virtual math::Vector2 getDesiredAccel();

    /** Gets the current desired orientation */
    virtual math::Quaternion getDesiredOrientation();

    /** Gets the desired angular rate */
    virtual math::Vector3 getDesiredAngularRate();

    /** Gets the desired angular acceleration */
    virtual math::Vector3 getDesiredAngularAccel();

    /** Current desired depth of the sub (uncalibrated units)*/
    virtual double getDesiredDepth();

    /** Current desired depth rate change */
    virtual double getDesiredDepthRate();

    /** Current desired depth acceleration */
    virtual double getDesiredDepthAccel();


    /** Loads current orientation into desired (fixes offset in roll and pitch)
     *
     *  The desired state quaternion will be "level" in horizontal plane, this
     *  will reverse slight offsets in roll and pitch.
     *
     *  @warning
     *      The vehicle should be upright when using this function, otherwise
     *      the interpretation of yaw and upright will be nonsensical.
     */
    virtual void holdCurrentHeading();

    /** Loads current orientation into the desired orientation */
    virtual void holdCurrentOrientation();

    /** Loads current position into desired and stays in that position */
    virtual void holdCurrentPosition();

    /** Makes the current actual depth the desired depth */
    virtual void holdCurrentDepth();



    /** Returns true if the vehicle is at the desired orientation */
    virtual bool atOrientation();

    /** Returns true if the vehicle is at the desired position */
    virtual bool atPosition();
    
    /** Returns true if the vehicle is at the desired velocity */
    virtual bool atVelocity();

    /** Returns true if the vehicle is at the desired depth */
    virtual bool atDepth();



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

    virtual void moveVel(double x, double y, double z)
    {
        math::Vector2 vxy = m_desiredState->getDesiredVelocity();
        double vz = m_desiredState->getDesiredDepthRate();
        bool bx = false;
        bool by = false;
        bool bz = false;
        if(x != 0)
        {
            vxy.x = x;
            bx = true;
        }
        if(y != 0)
        {
            vxy.y = y;
            by = true;
        }
        if(z != 0)
        {
            vz = z;
            bz = true;
        }
        m_desiredState->setDesiredVelocity(vxy);
        m_desiredState->setDesiredDepthRate(vz);
        m_desiredState->setvCon(bx,by,bz);
    }


protected:

    /** Implemeted by subclasses, does actual controller work
     *
     *  @param timestep
     *      The time sice the last update
     *  @param translationalForceOut
     *      The new force to apply to the vehicle (newtons)
     *  @param rotationalTorqueOut
     *      The new torque to apply to the vehicle (newtons)
     */
    virtual void doUpdate(const double& timestep,
                          math::Vector3& translationalForceOut,
                          math::Vector3& rotationalTorqueOut) = 0;

    control::DesiredStatePtr m_desiredState;
    estimation::IStateEstimatorPtr m_stateEstimator;
    vehicle::IVehiclePtr m_vehicle;
    core::ReadWriteMutex m_mutex;

    /** determine if we initially hold our depth and heading */
    int m_initHoldDepth;
    int m_initHoldHeading;
    int m_initHoldPosition;

private:
    void init(core::ConfigNode config,
              core::EventHubPtr eventHub);

    /** Updates m_atDepth on a change to the desired or estimated depth */
    void atDepthUpdate(core::EventPtr event);

    /** Updates m_atDepth on a change to the desired or estimated position */
    void atPositionUpdate(core::EventPtr event);

    /** Updates m_atDepth on a change to the desired or estimated velocity */
    void atVelocityUpdate(core::EventPtr event);

    /** Updates m_atDepth on a change to the desired or estimated orientation */
    void atOrientationUpdate(core::EventPtr event);

    core::EventConnectionPtr conn_desired_atDepth;
    core::EventConnectionPtr conn_estimated_atDepth;

    core::EventConnectionPtr conn_desired_atPosition;
    core::EventConnectionPtr conn_estimated_atPosition;

    core::EventConnectionPtr conn_desired_atVelocity;
    core::EventConnectionPtr conn_estimated_atVelocity;

    core::EventConnectionPtr conn_desired_atOrientation;
    core::EventConnectionPtr conn_estimated_atOrientation;
    
    /** Publishes the AT_DEPTH event for the depth given */
    void publishAtDepth(const double& depth);

    /** Publishes the AT_ORIENTATION event for the orientation given */
    void publishAtOrientation(const math::Quaternion& orientation);

    /** Publishes the AT_VELOCITY event for the velocity given */
    void publishAtVelocity(const math::Vector2& velocity);

    /** Publishes the AT_POSITION event for the position given */
    void publishAtPosition(const math::Vector2& position);
    
    /** Used to maintain state so we don't issue continuous AT_WHATEVER events*/
    bool m_atDepth;
    bool m_atOrientation;
    bool m_atVelocity;
    bool m_atPosition;
    
    /** When we are within these limits we send off the AT_WHATEVER event */
    double m_depthThreshold;
    double m_orientationThreshold;
    double m_velocityThreshold;
    double m_positionThreshold;
    
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_BWDCONTROLER_07_07_2007

