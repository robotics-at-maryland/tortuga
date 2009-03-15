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

namespace ram {
namespace control {

DepthControllerBase::DepthControllerBase(core::ConfigNode config) :
    m_desiredDepth(0),
    m_currentDepth(0)
{
    init(config);
}

void DepthControllerBase::setDepth(double depth)
{
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_desiredDepth = depth;
}

double DepthControllerBase::getDepth()
{
    core::ReadWriteMutex::ScopedReadLock lock(m_stateMutex);
    return m_desiredDepth;
}
    
bool DepthControllerBase::atDepth()
{
    double difference = fabs(m_currentDepth - m_desiredDepth);
    return difference <= m_depthThreshold;
}

math::Vector3 DepthControllerBase::depthUpdate(double timestep, double depth,
                                               math::Quaternion orienation)
{ 
    core::ReadWriteMutex::ScopedWriteLock lock(m_stateMutex);
    m_currentDepth = depth;

    return math::Vector3::ZERO;
}

void DepthControllerBase::init(core::ConfigNode config)
{
    m_depthThreshold =
        config["depthThreshold"].asDouble(DEPTH_TOLERANCE);
}
    
} // namespace control
} // namespace ram
