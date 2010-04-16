/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/IDepthController.h
 */

#ifndef RAM_CONTROL_DEPTHCONTROLLERBASE_03_14_2009
#define RAM_CONTROL_DEPTHCONTROLLERBASE_03_14_2009

// Project Includes
#include "control/include/IDepthController.h"

#include "control/include/DesiredState.h"
#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

/** Class to make implementation of an IDepthController simpler
 *
 *  Makes implementing IDepthControllerImp much easier.
 */
class RAM_EXPORT DepthControllerBase : public IDepthControllerImp
{
public:
    DepthControllerBase(core::ConfigNode config);
    
    virtual ~DepthControllerBase() {}

    // virtual void setDepth(double depth);

    // virtual double getDepth();

    // /** This is basically ignored, most likely will be removed */
    // virtual double getEstimatedDepth() { return 0.0; }
    
    // /** This is basically ignored, most likely will be removed*/
    // virtual double getEstimatedDepthDot() { return 0.0; }
    
    // virtual bool atDepth();

    // virtual void holdCurrentDepth();

    /** Does housing keeping work, should be called first in every override
     *
     *  When this method is overridden to implement your controller, call this
     *  method first with the same arguments you are given.
     */
    virtual math::Vector3 depthUpdate(double timestep, double depth,
                                      math::Quaternion orienation,
                                      controltest::DesiredStatePtr desiredState);
protected:
    /** When we are within this limit atDepth returns */
    double m_depthThreshold;

    /** Syncs asscess to the shared state */
    core::ReadWriteMutex m_stateMutex;

    /** The current desired depth */
    double m_desiredDepth;

    /** The depth from the last update command */
    double m_currentDepth;

    math::Quaternion m_currentOrientation;

    bool m_atDepth;

    bool m_atOrientation;

 private:
    /** Does all initialzation based on the configuration settings */
    void init(core::ConfigNode config);
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_DEPTHCONTROLLERBASE_03_14_2009
