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

#include <boost/shared_ptr.hpp>
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "core/include/ConfigNode.h"

#include "core/include/ReadWriteMutex.h"
#include "core/include/EventPublisher.h"

#include "vehicle/include/IVehicle.h"

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

    // returned in inertial frame

    /** Returns the desired velocity in the inertial frame */
    math::Vector2 getDesiredVelocity();

    /** Returns the desired position in the inertial frame */
    math::Vector2 getDesiredPosition();

    /** Returns the desired orientation */
    math::Quaternion getDesiredOrientation();

    /** returns the desired angular rate */
    math::Vector3 getDesiredAngularRate();

    /** returns the desired depth */
    double getDesiredDepth();

    /** returns the desired depth change rate */
    double getDesiredDepthRate();

    /** Set the desired velocity in the inertial frame
     *
     * @param velocity - the desired velocity 2-vector
     */
    void setDesiredVelocity(math::Vector2 velocity);

    /** Set the desired position in the inertial frame
     *
     * @param position - the desired velocity 2-vector
     */
    void setDesiredPosition(math::Vector2 position);

    /** Set the desired depth
     *
     * @param depth - the desired depth
     */
    void setDesiredDepth(double depth);

    /** Set the desired depth change rate
     *
     * @param depthRate - the desired depth change rate
     */
    void setDesiredDepthRate(double depthRate);

    /** Set the desired orientation
     *
     * @param orientation - the desired quaternion
     */
    void setDesiredOrientation(math::Quaternion orientation);
    
    /** Set the desired angular rate
     *
     * @param angularRate - a 3-vector representing the desired angular rate
     */
    void setDesiredAngularRate(math::Vector3 angularRate);

private:
    void init(core::ConfigNode config);

    /** Called whenever a new depth is set, handles events as needed
     *
     *  This will always publish the DESIRED_DEPTH_UPDATE event and also
     *  publish the AT_DEPTH event if needed.
     */
    void newDepthSet(const double& newDepth);

    /** Called whenever a new depth change rate is set, handles events as needed
     *
     *  This will always publish the DESIRED_DEPTHRATE_UPDATE event and also
     *  publish the AT_DEPTHRATE event if needed.
     */
    void newDepthRateSet(const double& newDepthRate);

    /** Called whenever the desired orientation changes, handles events
     *
     *  This will always publish the DESIRED_ORIENTATION_UPDATE event and also
     *  publish the AT_ORIENTATION event if needed.
     */
    void newDesiredOrientationSet(const math::Quaternion& newOrientation);
   
    /** Called whenever the desired velocity changes, handles events
     *
     * This publishes the DESIRED_VELOCITY_UPDATE event and will
     * publish the AT_VELOCITY event if needed.
     */
    void newDesiredVelocitySet(const math::Vector2& newVelocity);

    /** Called whenever the desired velocity changes, handles events
     *
     * This publishes the DESIRED_POSITION_UPDATE event and will
     * publish the AT_POSITION event if needed.
     */
    void newDesiredPositionSet(const math::Vector2& newPosition);

    math::Vector2 m_desiredVelocity; // stored in inertial frame
    math::Vector2 m_desiredPosition; // stored in inertial frame
      
    double m_desiredDepth;
    double m_desiredDepthRate;
      
    math::Quaternion m_desiredOrientation;
    math::Vector3 m_desiredAngularRate;

    core::ReadWriteMutex m_stateMutex;
}; // class

} //namespace control
} //namespace ram

#endif //RAM_CONTROL_DESIREDSTATE_H
