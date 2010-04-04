/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/TrackingTranslationalController.h
 */

#ifndef RAM_CONTROL_DESIREDSTATE_H
#define RAM_CONTROL_DESIREDSTATE_H

#include <boost/shared_ptr.hpp>
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

#include "core/include/ReadWriteMutex.h"

#include "vehicle/include/IVehicle.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram{
namespace controltest {

enum Frame {
    INERTIAL,
    BODY
};

class DesiredState;
typedef boost::shared_ptr<DesiredState> DesiredStatePtr;

class DesiredState {
public:
    virtual ~DesiredState();

    static DesiredStatePtr getInstance();

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
    void setDesiredYaw(double degrees);
    void setDesiredPitch(double degrees);
    void setDesiredRoll(double degrees);

private:
    DesiredState();
    DesiredState(DesiredState const&){};

    static DesiredStatePtr m_instance;
    
    math::Vector2 m_desiredVelocity; // Store in inertial frame
    math::Vector2 m_desiredPosition; // Store in inertial frame
      
    double m_desiredDepth;
      
    math::Quaternion m_desiredOrientation;
    math::Vector3 m_desiredAngularRate;

    core::ReadWriteMutex m_stateMutex;
}; // class

} //namespace controltest
} //namespace ram

#endif //RAM_CONTROL_DESIREDSTATE_H
