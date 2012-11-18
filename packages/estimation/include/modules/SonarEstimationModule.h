 /*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/estimation/include/modules/SonarEstimationModule.h
 */

#ifndef RAM_ESTIMATION_SONARESTIMATIONMODULE_H
#define RAM_ESTIMATION_SONARESTIMATIONMODULE_H

// STD Includes
#include <deque>
// Library Includes

// Project Includes
#include "estimation/include/EstimatedState.h"
#include "estimation/include/EstimationModule.h"
#include "core/include/ConfigNode.h"
#include "core/include/Event.h"
#include "math/include/Vector3.h"
#include "vehicle/include/device/SensorBoard.h"

namespace ram {
namespace estimation {

class SonarEstimationModule : public EstimationModule
{
public:
    SonarEstimationModule(core::ConfigNode config,
                          core::EventHubPtr eventHub,
                          EstimatedStatePtr estState);

    virtual ~SonarEstimationModule() {}

    virtual void update(core::EventPtr event);

private:
    int m_numMeasurements;
    std::deque<math::Vector3> m_pingerLocations;
};

} // namespace estimation
} // namespace ram

#endif
