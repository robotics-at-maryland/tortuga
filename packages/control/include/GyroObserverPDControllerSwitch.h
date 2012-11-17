/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/control/include/GyroObserverPDControllerSwitch.h
 */

#ifndef RAM_CONTROL_GYROOBSERVERPDCONTROLLERSWITCH_H
#define RAM_CONTROL_GYROOBSERVERPDCONTROLLERSWITCH_H

#include "control/include/RotationalControllerBase.h"
#include "math/include/MatrixN.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#include "math/include/Matrix3.h"

namespace ram {
namespace control {

class GyroObserverPDControllerSwitch : public RotationalControllerBase
{
public:
    GyroObserverPDControllerSwitch(ram::core::ConfigNode config);
    virtual ~GyroObserverPDControllerSwitch() {}

    virtual math::Vector3 rotationalUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState);

    /* Public accessor functions for testing purposes */
    math::Vector3 get_bHat() {return m_bHatOld;}
    math::Vector3 get_dbHat() {return m_dbHatOld;}
    math::Vector3 get_wHat() {return m_wHatOld;}
    math::Quaternion get_qHat() {return m_qHatOld;}
    math::Quaternion get_dqHat() {return m_dqHatOld;}

private:
    /** constrain var between min and max.  enhance readability */
    void clip(double &var, double min, double max);

    double m_dtMin;
    double m_dtMax;

    double m_gyroObsGain;
    double m_angularPGain;
    double m_angularDGain;
    
    int m_gyroPDType;

    math::Vector3 m_bHatOld;
    math::Vector3 m_dbHatOld;
    math::Vector3 m_wHatOld;
    math::Quaternion m_qHatOld;
    math::Quaternion m_dqHatOld;

    math::Matrix3 m_inertia;
};

} // namespace control
} // namespace ram

#endif // RAM_CONTROL_GYROOBSERVERPDCONTROLLERSWITCH_H
