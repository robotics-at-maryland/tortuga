// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.2.1
// Generated from file `ram.ice'

#ifndef __ram_h__
#define __ram_h__

#include <Ice/LocalObjectF.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/Proxy.h>
#include <Ice/Object.h>
#include <Ice/Outgoing.h>
#include <Ice/Incoming.h>
#include <Ice/Direct.h>
#include <Ice/StreamF.h>
#include <Ice/UndefSysMacros.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 302
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 1
#       error Ice patch level mismatch!
#   endif
#endif

namespace IceProxy
{

namespace ram
{

namespace math
{

class Vector;
bool operator==(const Vector&, const Vector&);
bool operator!=(const Vector&, const Vector&);
bool operator<(const Vector&, const Vector&);
bool operator<=(const Vector&, const Vector&);
bool operator>(const Vector&, const Vector&);
bool operator>=(const Vector&, const Vector&);

class Quaternion;
bool operator==(const Quaternion&, const Quaternion&);
bool operator!=(const Quaternion&, const Quaternion&);
bool operator<(const Quaternion&, const Quaternion&);
bool operator<=(const Quaternion&, const Quaternion&);
bool operator>(const Quaternion&, const Quaternion&);
bool operator>=(const Quaternion&, const Quaternion&);

}

namespace control
{

class Controller;
bool operator==(const Controller&, const Controller&);
bool operator!=(const Controller&, const Controller&);
bool operator<(const Controller&, const Controller&);
bool operator<=(const Controller&, const Controller&);
bool operator>(const Controller&, const Controller&);
bool operator>=(const Controller&, const Controller&);

}

}

}

namespace ram
{

namespace math
{

class Vector;
bool operator==(const Vector&, const Vector&);
bool operator!=(const Vector&, const Vector&);
bool operator<(const Vector&, const Vector&);
bool operator<=(const Vector&, const Vector&);
bool operator>(const Vector&, const Vector&);
bool operator>=(const Vector&, const Vector&);

class Quaternion;
bool operator==(const Quaternion&, const Quaternion&);
bool operator!=(const Quaternion&, const Quaternion&);
bool operator<(const Quaternion&, const Quaternion&);
bool operator<=(const Quaternion&, const Quaternion&);
bool operator>(const Quaternion&, const Quaternion&);
bool operator>=(const Quaternion&, const Quaternion&);

}

namespace control
{

class Controller;
bool operator==(const Controller&, const Controller&);
bool operator!=(const Controller&, const Controller&);
bool operator<(const Controller&, const Controller&);
bool operator<=(const Controller&, const Controller&);
bool operator>(const Controller&, const Controller&);
bool operator>=(const Controller&, const Controller&);

}

}

namespace IceInternal
{

void incRef(::ram::math::Vector*);
void decRef(::ram::math::Vector*);

void incRef(::IceProxy::ram::math::Vector*);
void decRef(::IceProxy::ram::math::Vector*);

void incRef(::ram::math::Quaternion*);
void decRef(::ram::math::Quaternion*);

void incRef(::IceProxy::ram::math::Quaternion*);
void decRef(::IceProxy::ram::math::Quaternion*);

void incRef(::ram::control::Controller*);
void decRef(::ram::control::Controller*);

void incRef(::IceProxy::ram::control::Controller*);
void decRef(::IceProxy::ram::control::Controller*);

}

namespace ram
{

namespace math
{

typedef ::IceInternal::Handle< ::ram::math::Vector> VectorPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::ram::math::Vector> VectorPrx;

void __write(::IceInternal::BasicStream*, const VectorPrx&);
void __read(::IceInternal::BasicStream*, VectorPrx&);
void __write(::IceInternal::BasicStream*, const VectorPtr&);
void __patch__VectorPtr(void*, ::Ice::ObjectPtr&);

void __addObject(const VectorPtr&, ::IceInternal::GCCountMap&);
bool __usesClasses(const VectorPtr&);
void __decRefUnsafe(const VectorPtr&);
void __clearHandleUnsafe(VectorPtr&);

typedef ::IceInternal::Handle< ::ram::math::Quaternion> QuaternionPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::ram::math::Quaternion> QuaternionPrx;

void __write(::IceInternal::BasicStream*, const QuaternionPrx&);
void __read(::IceInternal::BasicStream*, QuaternionPrx&);
void __write(::IceInternal::BasicStream*, const QuaternionPtr&);
void __patch__QuaternionPtr(void*, ::Ice::ObjectPtr&);

void __addObject(const QuaternionPtr&, ::IceInternal::GCCountMap&);
bool __usesClasses(const QuaternionPtr&);
void __decRefUnsafe(const QuaternionPtr&);
void __clearHandleUnsafe(QuaternionPtr&);

}

namespace control
{

typedef ::IceInternal::Handle< ::ram::control::Controller> ControllerPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::ram::control::Controller> ControllerPrx;

void __write(::IceInternal::BasicStream*, const ControllerPrx&);
void __read(::IceInternal::BasicStream*, ControllerPrx&);
void __write(::IceInternal::BasicStream*, const ControllerPtr&);
void __patch__ControllerPtr(void*, ::Ice::ObjectPtr&);

void __addObject(const ControllerPtr&, ::IceInternal::GCCountMap&);
bool __usesClasses(const ControllerPtr&);
void __decRefUnsafe(const ControllerPtr&);
void __clearHandleUnsafe(ControllerPtr&);

}

}

namespace ram
{

namespace math
{

}

namespace control
{

}

}

namespace IceProxy
{

namespace ram
{

namespace math
{

class Vector : virtual public ::IceProxy::Ice::Object
{
public:

    ::Ice::Double Dot(const ::ram::math::VectorPtr& v)
    {
        return Dot(v, 0);
    }
    ::Ice::Double Dot(const ::ram::math::VectorPtr& v, const ::Ice::Context& __ctx)
    {
        return Dot(v, &__ctx);
    }
    
private:

    ::Ice::Double Dot(const ::ram::math::VectorPtr&, const ::Ice::Context*);
    
public:

    ::Ice::Double Norm()
    {
        return Norm(0);
    }
    ::Ice::Double Norm(const ::Ice::Context& __ctx)
    {
        return Norm(&__ctx);
    }
    
private:

    ::Ice::Double Norm(const ::Ice::Context*);
    
public:

    ::Ice::Double NormSquared()
    {
        return NormSquared(0);
    }
    ::Ice::Double NormSquared(const ::Ice::Context& __ctx)
    {
        return NormSquared(&__ctx);
    }
    
private:

    ::Ice::Double NormSquared(const ::Ice::Context*);
    
public:

    ::ram::math::VectorPtr Normalize()
    {
        return Normalize(0);
    }
    ::ram::math::VectorPtr Normalize(const ::Ice::Context& __ctx)
    {
        return Normalize(&__ctx);
    }
    
private:

    ::ram::math::VectorPtr Normalize(const ::Ice::Context*);
    
public:

    ::ram::math::VectorPtr Add(const ::ram::math::VectorPtr& v)
    {
        return Add(v, 0);
    }
    ::ram::math::VectorPtr Add(const ::ram::math::VectorPtr& v, const ::Ice::Context& __ctx)
    {
        return Add(v, &__ctx);
    }
    
private:

    ::ram::math::VectorPtr Add(const ::ram::math::VectorPtr&, const ::Ice::Context*);
    
public:

    ::ram::math::VectorPtr Subtract(const ::ram::math::VectorPtr& v)
    {
        return Subtract(v, 0);
    }
    ::ram::math::VectorPtr Subtract(const ::ram::math::VectorPtr& v, const ::Ice::Context& __ctx)
    {
        return Subtract(v, &__ctx);
    }
    
private:

    ::ram::math::VectorPtr Subtract(const ::ram::math::VectorPtr&, const ::Ice::Context*);
    
public:

    ::ram::math::VectorPtr Multiply(::Ice::Double s)
    {
        return Multiply(s, 0);
    }
    ::ram::math::VectorPtr Multiply(::Ice::Double s, const ::Ice::Context& __ctx)
    {
        return Multiply(s, &__ctx);
    }
    
private:

    ::ram::math::VectorPtr Multiply(::Ice::Double, const ::Ice::Context*);
    
public:

    ::ram::math::VectorPtr Negate()
    {
        return Negate(0);
    }
    ::ram::math::VectorPtr Negate(const ::Ice::Context& __ctx)
    {
        return Negate(&__ctx);
    }
    
private:

    ::ram::math::VectorPtr Negate(const ::Ice::Context*);
    
public:

    ::ram::math::VectorPtr CrossProduct(const ::ram::math::VectorPtr& v)
    {
        return CrossProduct(v, 0);
    }
    ::ram::math::VectorPtr CrossProduct(const ::ram::math::VectorPtr& v, const ::Ice::Context& __ctx)
    {
        return CrossProduct(v, &__ctx);
    }
    
private:

    ::ram::math::VectorPtr CrossProduct(const ::ram::math::VectorPtr&, const ::Ice::Context*);
    
public:
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
};

class Quaternion : virtual public ::IceProxy::Ice::Object
{
public:

    ::Ice::Double Dot(const ::ram::math::QuaternionPtr& q)
    {
        return Dot(q, 0);
    }
    ::Ice::Double Dot(const ::ram::math::QuaternionPtr& q, const ::Ice::Context& __ctx)
    {
        return Dot(q, &__ctx);
    }
    
private:

    ::Ice::Double Dot(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);
    
public:

    ::Ice::Double Norm()
    {
        return Norm(0);
    }
    ::Ice::Double Norm(const ::Ice::Context& __ctx)
    {
        return Norm(&__ctx);
    }
    
private:

    ::Ice::Double Norm(const ::Ice::Context*);
    
public:

    ::Ice::Double NormSquared()
    {
        return NormSquared(0);
    }
    ::Ice::Double NormSquared(const ::Ice::Context& __ctx)
    {
        return NormSquared(&__ctx);
    }
    
private:

    ::Ice::Double NormSquared(const ::Ice::Context*);
    
public:

    ::ram::math::QuaternionPtr Normalize()
    {
        return Normalize(0);
    }
    ::ram::math::QuaternionPtr Normalize(const ::Ice::Context& __ctx)
    {
        return Normalize(&__ctx);
    }
    
private:

    ::ram::math::QuaternionPtr Normalize(const ::Ice::Context*);
    
public:

    ::ram::math::QuaternionPtr Add(const ::ram::math::QuaternionPtr& q)
    {
        return Add(q, 0);
    }
    ::ram::math::QuaternionPtr Add(const ::ram::math::QuaternionPtr& q, const ::Ice::Context& __ctx)
    {
        return Add(q, &__ctx);
    }
    
private:

    ::ram::math::QuaternionPtr Add(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);
    
public:

    ::ram::math::QuaternionPtr Subtract(const ::ram::math::QuaternionPtr& q)
    {
        return Subtract(q, 0);
    }
    ::ram::math::QuaternionPtr Subtract(const ::ram::math::QuaternionPtr& q, const ::Ice::Context& __ctx)
    {
        return Subtract(q, &__ctx);
    }
    
private:

    ::ram::math::QuaternionPtr Subtract(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);
    
public:

    ::ram::math::QuaternionPtr MultiplyQuaternion(const ::ram::math::QuaternionPtr& q)
    {
        return MultiplyQuaternion(q, 0);
    }
    ::ram::math::QuaternionPtr MultiplyQuaternion(const ::ram::math::QuaternionPtr& q, const ::Ice::Context& __ctx)
    {
        return MultiplyQuaternion(q, &__ctx);
    }
    
private:

    ::ram::math::QuaternionPtr MultiplyQuaternion(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);
    
public:

    ::ram::math::QuaternionPtr MultiplyScalar(::Ice::Double s)
    {
        return MultiplyScalar(s, 0);
    }
    ::ram::math::QuaternionPtr MultiplyScalar(::Ice::Double s, const ::Ice::Context& __ctx)
    {
        return MultiplyScalar(s, &__ctx);
    }
    
private:

    ::ram::math::QuaternionPtr MultiplyScalar(::Ice::Double, const ::Ice::Context*);
    
public:

    ::ram::math::QuaternionPtr Inverse()
    {
        return Inverse(0);
    }
    ::ram::math::QuaternionPtr Inverse(const ::Ice::Context& __ctx)
    {
        return Inverse(&__ctx);
    }
    
private:

    ::ram::math::QuaternionPtr Inverse(const ::Ice::Context*);
    
public:

    ::ram::math::QuaternionPtr Negate()
    {
        return Negate(0);
    }
    ::ram::math::QuaternionPtr Negate(const ::Ice::Context& __ctx)
    {
        return Negate(&__ctx);
    }
    
private:

    ::ram::math::QuaternionPtr Negate(const ::Ice::Context*);
    
public:

    ::ram::math::VectorPtr MultiplyVector(const ::ram::math::VectorPtr& v)
    {
        return MultiplyVector(v, 0);
    }
    ::ram::math::VectorPtr MultiplyVector(const ::ram::math::VectorPtr& v, const ::Ice::Context& __ctx)
    {
        return MultiplyVector(v, &__ctx);
    }
    
private:

    ::ram::math::VectorPtr MultiplyVector(const ::ram::math::VectorPtr&, const ::Ice::Context*);
    
public:
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
};

}

namespace control
{

class Controller : virtual public ::IceProxy::Ice::Object
{
public:

    void setSpeed(::Ice::Double speed)
    {
        setSpeed(speed, 0);
    }
    void setSpeed(::Ice::Double speed, const ::Ice::Context& __ctx)
    {
        setSpeed(speed, &__ctx);
    }
    
private:

    void setSpeed(::Ice::Double, const ::Ice::Context*);
    
public:

    void setDepth(::Ice::Double depth)
    {
        setDepth(depth, 0);
    }
    void setDepth(::Ice::Double depth, const ::Ice::Context& __ctx)
    {
        setDepth(depth, &__ctx);
    }
    
private:

    void setDepth(::Ice::Double, const ::Ice::Context*);
    
public:

    ::Ice::Double getSpeed()
    {
        return getSpeed(0);
    }
    ::Ice::Double getSpeed(const ::Ice::Context& __ctx)
    {
        return getSpeed(&__ctx);
    }
    
private:

    ::Ice::Double getSpeed(const ::Ice::Context*);
    
public:

    ::Ice::Double getDepth()
    {
        return getDepth(0);
    }
    ::Ice::Double getDepth(const ::Ice::Context& __ctx)
    {
        return getDepth(&__ctx);
    }
    
private:

    ::Ice::Double getDepth(const ::Ice::Context*);
    
public:

    ::Ice::Double getEstimatedDepth()
    {
        return getEstimatedDepth(0);
    }
    ::Ice::Double getEstimatedDepth(const ::Ice::Context& __ctx)
    {
        return getEstimatedDepth(&__ctx);
    }
    
private:

    ::Ice::Double getEstimatedDepth(const ::Ice::Context*);
    
public:

    ::Ice::Double getEstimatedDepthDot()
    {
        return getEstimatedDepthDot(0);
    }
    ::Ice::Double getEstimatedDepthDot(const ::Ice::Context& __ctx)
    {
        return getEstimatedDepthDot(&__ctx);
    }
    
private:

    ::Ice::Double getEstimatedDepthDot(const ::Ice::Context*);
    
public:

    void yawVehicle(::Ice::Double degrees)
    {
        yawVehicle(degrees, 0);
    }
    void yawVehicle(::Ice::Double degrees, const ::Ice::Context& __ctx)
    {
        yawVehicle(degrees, &__ctx);
    }
    
private:

    void yawVehicle(::Ice::Double, const ::Ice::Context*);
    
public:

    void pitchVehicle(::Ice::Double degrees)
    {
        pitchVehicle(degrees, 0);
    }
    void pitchVehicle(::Ice::Double degrees, const ::Ice::Context& __ctx)
    {
        pitchVehicle(degrees, &__ctx);
    }
    
private:

    void pitchVehicle(::Ice::Double, const ::Ice::Context*);
    
public:

    void rollVehicle(::Ice::Double degrees)
    {
        rollVehicle(degrees, 0);
    }
    void rollVehicle(::Ice::Double degrees, const ::Ice::Context& __ctx)
    {
        rollVehicle(degrees, &__ctx);
    }
    
private:

    void rollVehicle(::Ice::Double, const ::Ice::Context*);
    
public:

    ::ram::math::QuaternionPtr getDesiredOrientation()
    {
        return getDesiredOrientation(0);
    }
    ::ram::math::QuaternionPtr getDesiredOrientation(const ::Ice::Context& __ctx)
    {
        return getDesiredOrientation(&__ctx);
    }
    
private:

    ::ram::math::QuaternionPtr getDesiredOrientation(const ::Ice::Context*);
    
public:

    bool atDepth()
    {
        return atDepth(0);
    }
    bool atDepth(const ::Ice::Context& __ctx)
    {
        return atDepth(&__ctx);
    }
    
private:

    bool atDepth(const ::Ice::Context*);
    
public:

    bool atOrientation()
    {
        return atOrientation(0);
    }
    bool atOrientation(const ::Ice::Context& __ctx)
    {
        return atOrientation(&__ctx);
    }
    
private:

    bool atOrientation(const ::Ice::Context*);
    
public:
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
};

}

}

}

namespace IceDelegate
{

namespace ram
{

namespace math
{

class Vector : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual ::Ice::Double Dot(const ::ram::math::VectorPtr&, const ::Ice::Context*) = 0;

    virtual ::Ice::Double Norm(const ::Ice::Context*) = 0;

    virtual ::Ice::Double NormSquared(const ::Ice::Context*) = 0;

    virtual ::ram::math::VectorPtr Normalize(const ::Ice::Context*) = 0;

    virtual ::ram::math::VectorPtr Add(const ::ram::math::VectorPtr&, const ::Ice::Context*) = 0;

    virtual ::ram::math::VectorPtr Subtract(const ::ram::math::VectorPtr&, const ::Ice::Context*) = 0;

    virtual ::ram::math::VectorPtr Multiply(::Ice::Double, const ::Ice::Context*) = 0;

    virtual ::ram::math::VectorPtr Negate(const ::Ice::Context*) = 0;

    virtual ::ram::math::VectorPtr CrossProduct(const ::ram::math::VectorPtr&, const ::Ice::Context*) = 0;
};

class Quaternion : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual ::Ice::Double Dot(const ::ram::math::QuaternionPtr&, const ::Ice::Context*) = 0;

    virtual ::Ice::Double Norm(const ::Ice::Context*) = 0;

    virtual ::Ice::Double NormSquared(const ::Ice::Context*) = 0;

    virtual ::ram::math::QuaternionPtr Normalize(const ::Ice::Context*) = 0;

    virtual ::ram::math::QuaternionPtr Add(const ::ram::math::QuaternionPtr&, const ::Ice::Context*) = 0;

    virtual ::ram::math::QuaternionPtr Subtract(const ::ram::math::QuaternionPtr&, const ::Ice::Context*) = 0;

    virtual ::ram::math::QuaternionPtr MultiplyQuaternion(const ::ram::math::QuaternionPtr&, const ::Ice::Context*) = 0;

    virtual ::ram::math::QuaternionPtr MultiplyScalar(::Ice::Double, const ::Ice::Context*) = 0;

    virtual ::ram::math::QuaternionPtr Inverse(const ::Ice::Context*) = 0;

    virtual ::ram::math::QuaternionPtr Negate(const ::Ice::Context*) = 0;

    virtual ::ram::math::VectorPtr MultiplyVector(const ::ram::math::VectorPtr&, const ::Ice::Context*) = 0;
};

}

namespace control
{

class Controller : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void setSpeed(::Ice::Double, const ::Ice::Context*) = 0;

    virtual void setDepth(::Ice::Double, const ::Ice::Context*) = 0;

    virtual ::Ice::Double getSpeed(const ::Ice::Context*) = 0;

    virtual ::Ice::Double getDepth(const ::Ice::Context*) = 0;

    virtual ::Ice::Double getEstimatedDepth(const ::Ice::Context*) = 0;

    virtual ::Ice::Double getEstimatedDepthDot(const ::Ice::Context*) = 0;

    virtual void yawVehicle(::Ice::Double, const ::Ice::Context*) = 0;

    virtual void pitchVehicle(::Ice::Double, const ::Ice::Context*) = 0;

    virtual void rollVehicle(::Ice::Double, const ::Ice::Context*) = 0;

    virtual ::ram::math::QuaternionPtr getDesiredOrientation(const ::Ice::Context*) = 0;

    virtual bool atDepth(const ::Ice::Context*) = 0;

    virtual bool atOrientation(const ::Ice::Context*) = 0;
};

}

}

}

namespace IceDelegateM
{

namespace ram
{

namespace math
{

class Vector : virtual public ::IceDelegate::ram::math::Vector,
               virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual ::Ice::Double Dot(const ::ram::math::VectorPtr&, const ::Ice::Context*);

    virtual ::Ice::Double Norm(const ::Ice::Context*);

    virtual ::Ice::Double NormSquared(const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Normalize(const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Add(const ::ram::math::VectorPtr&, const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Subtract(const ::ram::math::VectorPtr&, const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Multiply(::Ice::Double, const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Negate(const ::Ice::Context*);

    virtual ::ram::math::VectorPtr CrossProduct(const ::ram::math::VectorPtr&, const ::Ice::Context*);
};

class Quaternion : virtual public ::IceDelegate::ram::math::Quaternion,
                   virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual ::Ice::Double Dot(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);

    virtual ::Ice::Double Norm(const ::Ice::Context*);

    virtual ::Ice::Double NormSquared(const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Normalize(const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Add(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Subtract(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr MultiplyQuaternion(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr MultiplyScalar(::Ice::Double, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Inverse(const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Negate(const ::Ice::Context*);

    virtual ::ram::math::VectorPtr MultiplyVector(const ::ram::math::VectorPtr&, const ::Ice::Context*);
};

}

namespace control
{

class Controller : virtual public ::IceDelegate::ram::control::Controller,
                   virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void setSpeed(::Ice::Double, const ::Ice::Context*);

    virtual void setDepth(::Ice::Double, const ::Ice::Context*);

    virtual ::Ice::Double getSpeed(const ::Ice::Context*);

    virtual ::Ice::Double getDepth(const ::Ice::Context*);

    virtual ::Ice::Double getEstimatedDepth(const ::Ice::Context*);

    virtual ::Ice::Double getEstimatedDepthDot(const ::Ice::Context*);

    virtual void yawVehicle(::Ice::Double, const ::Ice::Context*);

    virtual void pitchVehicle(::Ice::Double, const ::Ice::Context*);

    virtual void rollVehicle(::Ice::Double, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr getDesiredOrientation(const ::Ice::Context*);

    virtual bool atDepth(const ::Ice::Context*);

    virtual bool atOrientation(const ::Ice::Context*);
};

}

}

}

namespace IceDelegateD
{

namespace ram
{

namespace math
{

class Vector : virtual public ::IceDelegate::ram::math::Vector,
               virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual ::Ice::Double Dot(const ::ram::math::VectorPtr&, const ::Ice::Context*);

    virtual ::Ice::Double Norm(const ::Ice::Context*);

    virtual ::Ice::Double NormSquared(const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Normalize(const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Add(const ::ram::math::VectorPtr&, const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Subtract(const ::ram::math::VectorPtr&, const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Multiply(::Ice::Double, const ::Ice::Context*);

    virtual ::ram::math::VectorPtr Negate(const ::Ice::Context*);

    virtual ::ram::math::VectorPtr CrossProduct(const ::ram::math::VectorPtr&, const ::Ice::Context*);
};

class Quaternion : virtual public ::IceDelegate::ram::math::Quaternion,
                   virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual ::Ice::Double Dot(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);

    virtual ::Ice::Double Norm(const ::Ice::Context*);

    virtual ::Ice::Double NormSquared(const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Normalize(const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Add(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Subtract(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr MultiplyQuaternion(const ::ram::math::QuaternionPtr&, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr MultiplyScalar(::Ice::Double, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Inverse(const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr Negate(const ::Ice::Context*);

    virtual ::ram::math::VectorPtr MultiplyVector(const ::ram::math::VectorPtr&, const ::Ice::Context*);
};

}

namespace control
{

class Controller : virtual public ::IceDelegate::ram::control::Controller,
                   virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void setSpeed(::Ice::Double, const ::Ice::Context*);

    virtual void setDepth(::Ice::Double, const ::Ice::Context*);

    virtual ::Ice::Double getSpeed(const ::Ice::Context*);

    virtual ::Ice::Double getDepth(const ::Ice::Context*);

    virtual ::Ice::Double getEstimatedDepth(const ::Ice::Context*);

    virtual ::Ice::Double getEstimatedDepthDot(const ::Ice::Context*);

    virtual void yawVehicle(::Ice::Double, const ::Ice::Context*);

    virtual void pitchVehicle(::Ice::Double, const ::Ice::Context*);

    virtual void rollVehicle(::Ice::Double, const ::Ice::Context*);

    virtual ::ram::math::QuaternionPtr getDesiredOrientation(const ::Ice::Context*);

    virtual bool atDepth(const ::Ice::Context*);

    virtual bool atOrientation(const ::Ice::Context*);
};

}

}

}

namespace ram
{

namespace math
{

class Vector : virtual public ::Ice::Object
{
public:

    typedef VectorPrx ProxyType;
    typedef VectorPtr PointerType;
    
    Vector() {}
    Vector(::Ice::Double, ::Ice::Double, ::Ice::Double);
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();


    virtual ::Ice::Double Dot(const ::ram::math::VectorPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Dot(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::Double Norm(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Norm(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::Double NormSquared(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___NormSquared(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::VectorPtr Normalize(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Normalize(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::VectorPtr Add(const ::ram::math::VectorPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Add(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::VectorPtr Subtract(const ::ram::math::VectorPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Subtract(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::VectorPtr Multiply(::Ice::Double, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Multiply(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::VectorPtr Negate(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Negate(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::VectorPtr CrossProduct(const ::ram::math::VectorPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___CrossProduct(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::IceInternal::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);

    ::Ice::Double x;

    ::Ice::Double y;

    ::Ice::Double z;
};

void __patch__VectorPtr(void*, ::Ice::ObjectPtr&);

class Quaternion : virtual public ::Ice::Object
{
public:

    typedef QuaternionPrx ProxyType;
    typedef QuaternionPtr PointerType;
    
    Quaternion() {}
    Quaternion(::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double);
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();


    virtual ::Ice::Double Dot(const ::ram::math::QuaternionPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Dot(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::Double Norm(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Norm(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::Double NormSquared(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___NormSquared(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::QuaternionPtr Normalize(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Normalize(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::QuaternionPtr Add(const ::ram::math::QuaternionPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Add(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::QuaternionPtr Subtract(const ::ram::math::QuaternionPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Subtract(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::QuaternionPtr MultiplyQuaternion(const ::ram::math::QuaternionPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___MultiplyQuaternion(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::QuaternionPtr MultiplyScalar(::Ice::Double, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___MultiplyScalar(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::QuaternionPtr Inverse(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Inverse(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::QuaternionPtr Negate(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___Negate(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::VectorPtr MultiplyVector(const ::ram::math::VectorPtr&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___MultiplyVector(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::IceInternal::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);

    ::Ice::Double w;

    ::Ice::Double x;

    ::Ice::Double y;

    ::Ice::Double z;
};

void __patch__QuaternionPtr(void*, ::Ice::ObjectPtr&);

}

namespace control
{

class Controller : virtual public ::Ice::Object
{
public:

    typedef ControllerPrx ProxyType;
    typedef ControllerPtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void setSpeed(::Ice::Double, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___setSpeed(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void setDepth(::Ice::Double, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___setDepth(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::Double getSpeed(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___getSpeed(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::Double getDepth(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___getDepth(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::Double getEstimatedDepth(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___getEstimatedDepth(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::Double getEstimatedDepthDot(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___getEstimatedDepthDot(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void yawVehicle(::Ice::Double, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___yawVehicle(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void pitchVehicle(::Ice::Double, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___pitchVehicle(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void rollVehicle(::Ice::Double, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___rollVehicle(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::ram::math::QuaternionPtr getDesiredOrientation(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___getDesiredOrientation(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual bool atDepth(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___atDepth(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual bool atOrientation(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___atOrientation(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::IceInternal::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

void __patch__ControllerPtr(void*, ::Ice::ObjectPtr&);

}

}

#endif
