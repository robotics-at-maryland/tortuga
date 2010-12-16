/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/VisionSystemProxy.cpp
 */

// Project Includes
#include "core/include/GILock.h"
#include "network/slice/VisionSystemProxy.h"
#include "vision/include/VisionSystem.h"
#include "network/include/AdapterMaker.h"

RAM_NETWORK_REGISTER_PROXY(proxy::vision::VisionSystemProxy, VisionSystem);

namespace proxy {
namespace vision {

VisionSystemProxy::VisionSystemProxy(ram::core::SubsystemPtr impl)
    : m_impl(boost::dynamic_pointer_cast<ram::vision::VisionSystem>(impl))
{
}

VisionSystemProxy::~VisionSystemProxy()
{
}

void VisionSystemProxy::redLightDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->redLightDetectorOn();
}

void VisionSystemProxy::redLightDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->redLightDetectorOff();
}

void VisionSystemProxy::buoyDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->buoyDetectorOn();
}

void VisionSystemProxy::buoyDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->buoyDetectorOff();
}

void VisionSystemProxy::binDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->binDetectorOn();
}

void VisionSystemProxy::binDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->binDetectorOff();
}

void VisionSystemProxy::pipeLineDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->pipeLineDetectorOn();
}

void VisionSystemProxy::pipeLineDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->pipeLineDetectorOff();
}

void VisionSystemProxy::ductDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->ductDetectorOn();
}

void VisionSystemProxy::ductDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->ductDetectorOff();
}

void VisionSystemProxy::downwardSafeDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->downwardSafeDetectorOn();
}

void VisionSystemProxy::downwardSafeDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->downwardSafeDetectorOff();
}

void VisionSystemProxy::gateDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->gateDetectorOn();
}

void VisionSystemProxy::gateDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->gateDetectorOff();
}

void VisionSystemProxy::targetDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->targetDetectorOn();
}

void VisionSystemProxy::targetDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->targetDetectorOff();
}

void VisionSystemProxy::windowDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->windowDetectorOn();
}

void VisionSystemProxy::windowDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->windowDetectorOff();
}

void VisionSystemProxy::barbedWireDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->barbedWireDetectorOn();
}

void VisionSystemProxy::barbedWireDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->barbedWireDetectorOff();
}

void VisionSystemProxy::hedgeDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->hedgeDetectorOn();
}

void VisionSystemProxy::hedgeDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->hedgeDetectorOff();
}

void VisionSystemProxy::velocityDetectorOn(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->velocityDetectorOn();
}

void VisionSystemProxy::velocityDetectorOff(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->velocityDetectorOff();
}

} // namespace vision
} // namespace proxy
