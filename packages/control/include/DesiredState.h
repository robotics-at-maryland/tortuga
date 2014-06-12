/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/DesiredState.h
 */

#ifndef RAM_CONTROL_DESIREDSTATE_H
#define RAM_CONTROL_DESIREDSTATE_H

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "core/include/ConfigNode.h"

#include "core/include/ReadWriteMutex.h"
#include "core/include/EventPublisher.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

class DesiredState;
typedef boost::shared_ptr<DesiredState> DesiredStatePtr;

class DesiredState : public core::EventPublisher
{
public:
    DesiredState(core::ConfigNode config, 
                 core::EventHubPtr eventHub = core::EventHubPtr());
    virtual ~DesiredState();


    /** returns the desired position in the inertial frame */
    math::Vector2 getDesiredPosition();

    /** returns the desired velocity in the inertial frame */
    math::Vector2 getDesiredVelocity();

    /** returns the desired acceleration in the inertial frame */
    math::Vector2 getDesiredAccel();


    /** returns the desired orientation */
    math::Quaternion getDesiredOrientation();

    /** returns the desired angular rate */
    math::Vector3 getDesiredAngularRate();

    /** returns the desired angular acceleration */
    math::Vector3 getDesiredAngularAccel();


    /** returns the desired depth */
    double getDesiredDepth();

    /** returns the desired depth change rate */
    double getDesiredDepthRate();

    /** returns the desired depth change accel */
    double getDesiredDepthAccel();




    /** set the desired position in the inertial frame
     *
     * @param position - the desired velocity 2-vector
     */
    void setDesiredPosition(math::Vector2 position);

    /** set the desired velocity in the inertial frame
     *
     * @param velocity - the desired velocity 2-vector
     */
    void setDesiredVelocity(math::Vector2 velocity);

    /** set the desired acceleration in the inertial frame
     *
     * @param velocity - the desired velocity 2-vector
     */
    void setDesiredAccel(math::Vector2 accel);



    /** set the desired depth
     *
     * @param depth - the desired depth
     */
    void setDesiredDepth(double depth);

    /** set the desired depth change rate
     *
     * @param depthRate - the desired depth change rate
     */
    void setDesiredDepthRate(double depthRate);

    /** set the desired depth change accel
     *
     * @param depthRate - the desired depth change rate
     */
    void setDesiredDepthAccel(double depthAccel);



    /** set the desired orientation
     *
     * @param orientation - the desired quaternion
     */
    void setDesiredOrientation(math::Quaternion orientation);
    
    /** set the desired angular rate
     *
     * @param angularRate - a 3-vector representing the desired angular rate
     */
    void setDesiredAngularRate(math::Vector3 angularRate);

    /** set the desired angular acceleration
     *
     * @param angularRate - a 3-vector representing the desired angular rate
     */
    void setDesiredAngularAccel(math::Vector3 angularAccel);

    void setvCon(bool x, bool y, bool z);


    //yes these shouldn't be public, but they're only really used from python(which doesn't care) anyway
    bool vx;
    bool vy;
    bool vz;
private:
    void init(core::ConfigNode config);




    /** called whenever a new depth is set, handles events as needed
     *
     *  This will always publish the DESIRED_DEPTH_UPDATE event
     */
    void newDesiredDepthSet(const double& newDepth);

    /** called whenever a new depth change rate is set, handles events as needed
     *
     *  This will always publish the DESIRED_DEPTHRATE_UPDATE event
     */
    void newDesiredDepthRateSet(const double& newDepthRate);

    /** called whenever a new depth change rate is set, handles events as needed
     *
     *  This will always publish the DESIRED_DEPTHACCEL_UPDATE event
     */
    void newDesiredDepthAccelSet(const double& newDepthAccel);




    /** called whenever the desired orientation changes, handles events
     *
     *  This will always publish the DESIRED_ORIENTATION_UPDATE event
     */
    void newDesiredOrientationSet(const math::Quaternion& newOrientation);

    /** called whenever the desired orientation changes, handles events
     *
     *  This will always publish the DESIRED_ANGULARVELOCITY_UPDATE event
     */
    void newDesiredAngularRateSet(const math::Vector3& newAngularRate);

    /** called whenever the desired orientation changes, handles events
     *
     *  This will always publish the DESIRED_ANGULARACCEL_UPDATE event
     */
    void newDesiredAngularAccelSet(const math::Vector3& newAngularAccel);




    /** called whenever the desired position changes, handles events
     *
     * This publishes the DESIRED_POSITION_UPDATE event
     */
    void newDesiredPositionSet(const math::Vector2& newPosition);

    /** called whenever the desired velocity changes, handles events
     *
     * This publishes the DESIRED_VELOCITY_UPDATE event
     */
    void newDesiredVelocitySet(const math::Vector2& newVelocity);

    /** called whenever the desired velocity changes, handles events
     *
     * This publishes the DESIRED_LINEARACCEL_UPDATE event
     */
    void newDesiredLinearAccelSet(const math::Vector2& newAccel);



   
    math::Vector2 m_desiredVelocity; // stored in inertial frame
    math::Vector2 m_desiredPosition; // stored in inertial frame
    math::Vector2 m_desiredAccel;    // stored in inertial frame
      
    double m_desiredDepth;
    double m_desiredDepthRate;
    double m_desiredDepthAccel;
      
    math::Quaternion m_desiredOrientation;
    math::Vector3 m_desiredAngularRate;
    math::Vector3 m_desiredAngularAccel;


    core::ReadWriteMutex m_stateMutex;
}; // class

} //namespace control
} //namespace ram

#endif // RAM_CONTROL_DESIREDSTATE_H
