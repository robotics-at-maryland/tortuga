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
namespace controltest {

class DesiredState;
typedef boost::shared_ptr<DesiredState> DesiredStatePtr;

class DesiredState : public core::EventPublisher
{
public:
    DesiredState(core::ConfigNode config, 
                 core::EventHubPtr eventHub = core::EventHubPtr());
    virtual ~DesiredState();

    // returned in inertial frame
    math::Vector2 getDesiredVelocity();
    math::Vector2 getDesiredPosition();

    math::Quaternion getDesiredOrientation();
    math::Vector3 getDesiredAngularRate();

    double getDesiredDepth();

    // set in inertial frame
    void setDesiredVelocity(math::Vector2 velocity);
    void setDesiredPosition(math::Vector2 position);

    void setDesiredDepth(double depth);

    void setDesiredOrientation(math::Quaternion orientation);
    void setDesiredAngularRate(math::Vector3 angularRate);

private:
    void init(core::ConfigNode config);

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

    math::Vector2 m_desiredVelocity; // Stored in inertial frame
    math::Vector2 m_desiredPosition; // Stored in inertial frame
      
    double m_desiredDepth;
      
    math::Quaternion m_desiredOrientation;
    math::Vector3 m_desiredAngularRate;

    core::ReadWriteMutex m_stateMutex;
}; // class

} //namespace controltest
} //namespace ram

#endif //RAM_CONTROL_DESIREDSTATE_H
