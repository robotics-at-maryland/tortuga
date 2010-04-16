/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/src/DepthControllerBase.h
 */

// STD Includes
#include <cmath>

// Project includes
#include "control/include/DepthControllerBase.h"
#include "control/include/ControllerBase.h"
#include "math/include/Helpers.h"
#include "math/include/Events.h"
#include "core/include/EventHub.h"

namespace ram {
namespace control {

DepthControllerBase::DepthControllerBase(core::ConfigNode config) :
    m_desiredDepth(0),
    m_currentDepth(0)
{
    init(config);
}

// void DepthControllerBase::setDepth(double depth)
// {
//     {
//         core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
//         if(depth < 0)
//             depth = 0;
//         m_desiredDepth = depth;
//     }

// }

// double DepthControllerBase::getDepth()
// {
//     core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
//     return m_desiredDepth;
// }
    
// bool DepthControllerBase::atDepth()
// {
//     double currentDepth, desiredDepth;
//     {
//         core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
//         currentDepth = m_currentDepth;
//         desiredDepth = m_desiredDepth;
//     }
//     double difference = fabs(currentDepth - desiredDepth);
//     return difference <= m_depthThreshold;
// }

// void DepthControllerBase::holdCurrentDepth()
// {
//     double depth;
//     {
//         core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
//         depth = m_currentDepth;
//     }
//     setDepth(depth);
// }
    
math::Vector3 DepthControllerBase::depthUpdate(double timestep, double depth,
                                               math::Quaternion orientation,
                                               controltest::DesiredStatePtr desiredState)
{ 
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_currentDepth = depth;
    m_currentOrientation = orientation;

    return math::Vector3::ZERO;
}

void DepthControllerBase::init(core::ConfigNode config)
{
    m_depthThreshold =
        config["depthThreshold"].asDouble(DEPTH_TOLERANCE);
}

} // namespace control
} // namespace ram
