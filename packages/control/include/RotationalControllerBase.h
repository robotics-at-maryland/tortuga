/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/RotationalControllerBase.h
 */

#ifndef RAM_CONTROL_ROTATIONALCONTROLLERBASE_03_02_2009
#define RAM_CONTROL_ROTATIONALCONTROLLERBASE_03_02_2009

// Project Includes
#include "control/include/IRotationalController.h"

#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

/** Implements every method except rotationalUpdate
 *
 *  Makes implementing IRotationalControllerImp much easier.
 */
class RAM_EXPORT RotationalControllerBase : public IRotationalControllerImp
{
public:
    RotationalControllerBase(core::ConfigNode config);
    
    virtual ~RotationalControllerBase() {}
    
    virtual void yawVehicle(double degrees);

    virtual void pitchVehicle(double degrees);

    virtual void rollVehicle(double degrees);

    virtual math::Quaternion getDesiredOrientation();
    
    virtual void setDesiredOrientation(math::Quaternion);
    
    virtual bool atOrientation();

    virtual void holdCurrentHeading();

    /** Does housing keeping work, called first in every override
     *
     *  When this method is overridden to implement your controller, call this
     *  method first with the same arguments you are given.
     */
    virtual math::Vector3 rotationalUpdate(double timestep,
                                           math::Quaternion orientation,
                                           math::Vector3 angularRate);
    
private:
    /** Does all initialzation based on the configuration settings */
    void init(core::ConfigNode config);
    
    /** When we are within this limit we send of the atOrientation is true */
    double m_orientationThreshold;

    /** Syncs asscess to the desired state */
    core::ReadWriteMutex m_stateMutex;

    /** The current desired orientation */
    math::Quaternion m_desiredOrientation;

    /** The orientation from the last update command */
    math::Quaternion m_currentOrientation;
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_ROTATIONALCONTROLLERBASE_03_02_2009
