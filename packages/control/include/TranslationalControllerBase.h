/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/ITranslationalController.h
 */

#ifndef RAM_CONTROL_TRANSLATIONALCONTROLLERBASE_03_15_2009
#define RAM_CONTROL_TRANSLATIONALCONTROLLERBASE_03_15_2009

// Project Includes
#include "control/include/ITranslationalController.h"

#include "control/include/DesiredState.h"
#include "core/include/ConfigNode.h"
#include "core/include/ReadWriteMutex.h"
#include "core/include/EventPublisher.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

/** Defines the interface for controler which controls in plane motion */
class RAM_EXPORT TranslationalControllerBase :
        public ITranslationalControllerImp,
        public core::EventPublisher
{
  public:

    
    TranslationalControllerBase(core::ConfigNode config);
    
    virtual ~TranslationalControllerBase() {}

    virtual math::Vector3 translationalUpdate(
        double timestep,
        estimation::IStateEstimatorPtr estimator,
        control::DesiredStatePtr desiredState);
    
  protected:
    /** Syncs asscess to the shared state */
    core::ReadWriteMutex m_stateMutex;

  private:
    /** Does all initialzation based on the configuration settings */
    void init(core::ConfigNode config);
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_TRANSLATIONALCONTROLLERBASE_03_15_2009
