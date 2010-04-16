/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/TrackingTranslationalController.h
 */
 
#ifndef RAM_CONTROL_TRACKINGTRANSLATIONALCONTROLLER_H
#define RAM_CONTROL_TRACKINGTRANSLATIONALCONTROLLER_H

// Project Includes
#include "control/include/TranslationalControllerBase.h"

namespace ram {
namespace control {

/** A translational controller that is able to track a position, velocity, or both*/
class TrackingTranslationalController : public TranslationalControllerBase
{
public:
    TrackingTranslationalController(ram::core::ConfigNode config);
        
    virtual math::Vector3 translationalUpdate(double timestep,
                                              math::Vector3 linearAcceleration,
                                              math::Quaternion orientation,
                                              math::Vector2 position,
                                              math::Vector2 velocity,
                                              controltest::DesiredStatePtr desiredState);

private:
    void setControllerMode(ControlMode::ModeType mode);

    math::Vector2 m_prevPositionError;
    math::Vector2 m_prevVelocityError;

    math::Vector2 m_iPositionError;
    math::Vector2 m_iVelocityError;

    double x1kp, x1ki, x1kd;
    double x2kp, x2ki, x2kd;

};

} // namespace control 
} // namespace ram

#endif // RAM_CONTROL_TRACKINGTRANSLATIONALCONTROLLER_H
