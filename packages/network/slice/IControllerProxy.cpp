/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/slice/IControllerProxy.cpp
 */

// Project Includes
#include "core/include/GILock.h"
#include "math/include/Vector2.h"
#include "math/include/Quaternion.h"
#include "control/include/IController.h"

#include "network/include/AdapterMaker.h"
#include "network/slice/IControllerProxy.h"

RAM_NETWORK_REGISTER_PROXY(proxy::control::IControllerProxy, Controller);

static ram::math::Vector2 to_vector2(const proxy::math::Vector2& vector2)
{
    return ram::math::Vector2(vector2.x, vector2.y);
}

static proxy::math::Vector2 from_vector2(const ram::math::Vector2& vector2)
{
    proxy::math::Vector2 proxy;
    proxy.x = vector2.x;
    proxy.y = vector2.y;
    return proxy;
}

static ram::math::Quaternion to_quaternion(
    const proxy::math::Quaternion& quat)
{
    return ram::math::Quaternion(quat.x, quat.y, quat.z, quat.w);
}

static proxy::math::Quaternion from_quaternion(
    const ram::math::Quaternion& quat)
{
    proxy::math::Quaternion proxy;
    proxy.x = quat.x; proxy.y = quat.y;
    proxy.z = quat.z; proxy.w = quat.w;
    return proxy;
}

namespace proxy {
namespace control {

IControllerProxy::IControllerProxy(ram::core::SubsystemPtr impl)
    : m_impl(boost::dynamic_pointer_cast<ram::control::IController>(impl))
{
}

IControllerProxy::~IControllerProxy()
{
}

void IControllerProxy::setVelocity(const math::Vector2& velocity,
                                   const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->setVelocity(to_vector2(velocity));
}

math::Vector2 IControllerProxy::getVelocity(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return from_vector2(m_impl->getVelocity());
}

void IControllerProxy::setSpeed(Ice::Double speed, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->setSpeed(speed);
}

void IControllerProxy::setSidewaysSpeed(Ice::Double speed, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->setSidewaysSpeed(speed);
}

Ice::Double IControllerProxy::getSpeed(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->getSpeed();
}

Ice::Double IControllerProxy::getSidewaysSpeed(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->getSidewaysSpeed();
}

void IControllerProxy::holdCurrentPosition(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->holdCurrentPosition();
}

void IControllerProxy::setDesiredVelocity(const math::Vector2& velocity,
                                          Ice::Int frame, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->setDesiredVelocity(to_vector2(velocity), frame);
}

void IControllerProxy::setDesiredPosition(const math::Vector2& position,
                                          Ice::Int frame, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->setDesiredPosition(to_vector2(position), frame);
}

void IControllerProxy::setDesiredPositionAndVelocity(
    const math::Vector2& position, const math::Vector2& velocity,
    const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->setDesiredPositionAndVelocity(
        to_vector2(position), to_vector2(velocity));
}

math::Vector2 IControllerProxy::getDesiredVelocity(
    Ice::Int frame, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return from_vector2(m_impl->getDesiredVelocity(frame));
}

math::Vector2 IControllerProxy::getDesiredPosition(
    Ice::Int frame, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return from_vector2(m_impl->getDesiredPosition(frame));
}

bool IControllerProxy::atPosition(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->atPosition();
}

bool IControllerProxy::atVelocity(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->atVelocity();
}

void IControllerProxy::yawVehicle(Ice::Double degrees, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->yawVehicle(degrees);
}

void IControllerProxy::pitchVehicle(Ice::Double degrees, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->pitchVehicle(degrees);
}

void IControllerProxy::rollVehicle(Ice::Double degrees, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->rollVehicle(degrees);
}

math::Quaternion IControllerProxy::getDesiredOrientation(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return from_quaternion(m_impl->getDesiredOrientation());
}

void IControllerProxy::setDesiredOrientation(const math::Quaternion& quat,
                                             const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    m_impl->setDesiredOrientation(to_quaternion(quat));
}

bool IControllerProxy::atOrientation(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->atOrientation();
}

void IControllerProxy::setDepth(Ice::Double depth, const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->setDepth(depth);
}

Ice::Double IControllerProxy::getDepth(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->getDepth();
}

Ice::Double IControllerProxy::getEstimatedDepth(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->getEstimatedDepth();
}

Ice::Double IControllerProxy::getEstimatedDepthDot(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->getEstimatedDepthDot();
}

bool IControllerProxy::atDepth(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->atDepth();
}

void IControllerProxy::holdCurrentDepth(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->holdCurrentDepth();
}

void IControllerProxy::holdCurrentHeading(const Ice::Current &)
{
    ram::core::ScopedGILock lock;
    return m_impl->holdCurrentHeading();
}

} // namespace control
} // namespace proxy
