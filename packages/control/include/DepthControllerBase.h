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

/** class to make implementation of an IDepthController simpler
 *
 *  Makes implementing IDepthControllerImp much easier.
 */
class RAM_EXPORT DepthControllerBase : public IDepthControllerImp,
                                       public core::EventPublisher
{
  public:
    DepthControllerBase(core::ConfigNode config);
    
    virtual ~DepthControllerBase() {}

    virtual math::Vector3 depthUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState);

  protected:
    /** syncs asscess to the shared state */
    core::ReadWriteMutex m_stateMutex;
    
  private:
    /** does all initialzation based on the configuration settings */
    void init(core::ConfigNode config);
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_DEPTHCONTROLLERBASE_03_14_2009
