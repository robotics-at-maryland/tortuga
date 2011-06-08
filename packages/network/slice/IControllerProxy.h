/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/network/slice/IControllerProxy.h
 */

#ifndef RAM_CONTROL_SLICE_ICONTROLLERPROXY_H_12_16_2010
#define RAM_CONTROL_SLICE_ICONTROLLERPROXY_H_12_16_2010

// Library Includes
#include <boost/shared_ptr.hpp>
#include <Ice/Ice.h>

// Project Includes
#include "core/include/Subsystem.h"
#include "control.h"

namespace ram {
namespace control {
class IController;
typedef boost::shared_ptr<IController> IControllerPtr;
} // namespace control
} // namespace ram

namespace proxy {
namespace control {

class IControllerProxy : virtual public IController
{
public:
    IControllerProxy(ram::core::SubsystemPtr impl);

    virtual ~IControllerProxy();

    virtual void setVelocity(const math::Vector2& velocity,
                             const Ice::Current &);
    virtual math::Vector2 getVelocity(const Ice::Current &);

    virtual void setSpeed(Ice::Double speed, const Ice::Current &);
    virtual void setSidewaysSpeed(Ice::Double speed, const Ice::Current &);

    virtual Ice::Double getSpeed(const Ice::Current &);
    virtual Ice::Double getSidewaysSpeed(const Ice::Current &);

    virtual void holdCurrentPosition(const Ice::Current &);

    virtual void setDesiredVelocity(const math::Vector2& velocity,
                                    Ice::Int frame, const Ice::Current &);
    virtual void setDesiredPosition(const math::Vector2& position,
                                    Ice::Int frame, const Ice::Current &);
    virtual void setDesiredPositionAndVelocity(const math::Vector2& position,
                                               const math::Vector2& velocity,
                                               const Ice::Current &);
    virtual math::Vector2 getDesiredVelocity(Ice::Int frame,
                                             const Ice::Current &);
    virtual math::Vector2 getDesiredPosition(Ice::Int frame,
                                             const Ice::Current &);

    virtual bool atPosition(const Ice::Current &);
    virtual bool atVelocity(const Ice::Current &);

    virtual void yawVehicle(Ice::Double degrees, const Ice::Current &);
    virtual void pitchVehicle(Ice::Double degrees, const Ice::Current &);
    virtual void rollVehicle(Ice::Double degrees, const Ice::Current &);

    virtual math::Quaternion getDesiredOrientation(const Ice::Current &);
    virtual void setDesiredOrientation(const math::Quaternion& quat,
                                       const Ice::Current &);
    virtual bool atOrientation(const Ice::Current &);

    virtual void setDepth(Ice::Double depth, const Ice::Current &);
    virtual Ice::Double getDepth(const Ice::Current &);

    virtual Ice::Double getEstimatedDepth(const Ice::Current &);
    virtual Ice::Double getEstimatedDepthDot(const Ice::Current &);
    virtual bool atDepth(const Ice::Current &);
    virtual void holdCurrentDepth(const Ice::Current &);

    virtual void holdCurrentHeading(const Ice::Current &);

private:
    ram::control::IControllerPtr m_impl;
};

} // namespace control
} // namespace proxy

#endif // RAM_CONTROL_SLICE_ICONTROLLERPROXY_H_12_16_2010
