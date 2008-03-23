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

#include <ram.h>
#include <Ice/LocalException.h>
#include <Ice/ObjectFactory.h>
#include <Ice/BasicStream.h>
#include <Ice/Object.h>
#include <IceUtil/Iterator.h>
#include <IceUtil/ScopedArray.h>

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

static const ::std::string __ram__math__Vector__Dot_name = "Dot";

static const ::std::string __ram__math__Vector__Norm_name = "Norm";

static const ::std::string __ram__math__Vector__NormSquared_name = "NormSquared";

static const ::std::string __ram__math__Vector__Normalize_name = "Normalize";

static const ::std::string __ram__math__Vector__Add_name = "Add";

static const ::std::string __ram__math__Vector__Subtract_name = "Subtract";

static const ::std::string __ram__math__Vector__Multiply_name = "Multiply";

static const ::std::string __ram__math__Vector__Negate_name = "Negate";

static const ::std::string __ram__math__Vector__CrossProduct_name = "CrossProduct";

static const ::std::string __ram__math__Quaternion__Dot_name = "Dot";

static const ::std::string __ram__math__Quaternion__Norm_name = "Norm";

static const ::std::string __ram__math__Quaternion__NormSquared_name = "NormSquared";

static const ::std::string __ram__math__Quaternion__Normalize_name = "Normalize";

static const ::std::string __ram__math__Quaternion__Add_name = "Add";

static const ::std::string __ram__math__Quaternion__Subtract_name = "Subtract";

static const ::std::string __ram__math__Quaternion__MultiplyQuaternion_name = "MultiplyQuaternion";

static const ::std::string __ram__math__Quaternion__MultiplyScalar_name = "MultiplyScalar";

static const ::std::string __ram__math__Quaternion__Inverse_name = "Inverse";

static const ::std::string __ram__math__Quaternion__Negate_name = "Negate";

static const ::std::string __ram__math__Quaternion__MultiplyVector_name = "MultiplyVector";

static const ::std::string __ram__control__Controller__setSpeed_name = "setSpeed";

static const ::std::string __ram__control__Controller__setDepth_name = "setDepth";

static const ::std::string __ram__control__Controller__getSpeed_name = "getSpeed";

static const ::std::string __ram__control__Controller__getDepth_name = "getDepth";

static const ::std::string __ram__control__Controller__getEstimatedDepth_name = "getEstimatedDepth";

static const ::std::string __ram__control__Controller__getEstimatedDepthDot_name = "getEstimatedDepthDot";

static const ::std::string __ram__control__Controller__yawVehicle_name = "yawVehicle";

static const ::std::string __ram__control__Controller__pitchVehicle_name = "pitchVehicle";

static const ::std::string __ram__control__Controller__rollVehicle_name = "rollVehicle";

static const ::std::string __ram__control__Controller__getDesiredOrientation_name = "getDesiredOrientation";

static const ::std::string __ram__control__Controller__atDepth_name = "atDepth";

static const ::std::string __ram__control__Controller__atOrientation_name = "atOrientation";

void
IceInternal::incRef(::ram::math::Vector* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::ram::math::Vector* p)
{
    p->__decRef();
}

void
IceInternal::incRef(::IceProxy::ram::math::Vector* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::IceProxy::ram::math::Vector* p)
{
    p->__decRef();
}

void
IceInternal::incRef(::ram::math::Quaternion* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::ram::math::Quaternion* p)
{
    p->__decRef();
}

void
IceInternal::incRef(::IceProxy::ram::math::Quaternion* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::IceProxy::ram::math::Quaternion* p)
{
    p->__decRef();
}

void
IceInternal::incRef(::ram::control::Controller* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::ram::control::Controller* p)
{
    p->__decRef();
}

void
IceInternal::incRef(::IceProxy::ram::control::Controller* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::IceProxy::ram::control::Controller* p)
{
    p->__decRef();
}

void
ram::math::__write(::IceInternal::BasicStream* __os, const ::ram::math::VectorPrx& v)
{
    __os->write(::Ice::ObjectPrx(v));
}

void
ram::math::__read(::IceInternal::BasicStream* __is, ::ram::math::VectorPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::ram::math::Vector;
        v->__copyFrom(proxy);
    }
}

void
ram::math::__write(::IceInternal::BasicStream* __os, const ::ram::math::VectorPtr& v)
{
    __os->write(::Ice::ObjectPtr(v));
}

void
ram::math::__write(::IceInternal::BasicStream* __os, const ::ram::math::QuaternionPrx& v)
{
    __os->write(::Ice::ObjectPrx(v));
}

void
ram::math::__read(::IceInternal::BasicStream* __is, ::ram::math::QuaternionPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::ram::math::Quaternion;
        v->__copyFrom(proxy);
    }
}

void
ram::math::__write(::IceInternal::BasicStream* __os, const ::ram::math::QuaternionPtr& v)
{
    __os->write(::Ice::ObjectPtr(v));
}

void
ram::control::__write(::IceInternal::BasicStream* __os, const ::ram::control::ControllerPrx& v)
{
    __os->write(::Ice::ObjectPrx(v));
}

void
ram::control::__read(::IceInternal::BasicStream* __is, ::ram::control::ControllerPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::ram::control::Controller;
        v->__copyFrom(proxy);
    }
}

void
ram::control::__write(::IceInternal::BasicStream* __os, const ::ram::control::ControllerPtr& v)
{
    __os->write(::Ice::ObjectPtr(v));
}

void
ram::math::__addObject(const VectorPtr& p, ::IceInternal::GCCountMap& c)
{
    p->__addObject(c);
}

bool
ram::math::__usesClasses(const VectorPtr& p)
{
    return p->__usesClasses();
}

void
ram::math::__decRefUnsafe(const VectorPtr& p)
{
    p->__decRefUnsafe();
}

void
ram::math::__clearHandleUnsafe(VectorPtr& p)
{
    p.__clearHandleUnsafe();
}

void
ram::math::__addObject(const QuaternionPtr& p, ::IceInternal::GCCountMap& c)
{
    p->__addObject(c);
}

bool
ram::math::__usesClasses(const QuaternionPtr& p)
{
    return p->__usesClasses();
}

void
ram::math::__decRefUnsafe(const QuaternionPtr& p)
{
    p->__decRefUnsafe();
}

void
ram::math::__clearHandleUnsafe(QuaternionPtr& p)
{
    p.__clearHandleUnsafe();
}

void
ram::control::__addObject(const ControllerPtr& p, ::IceInternal::GCCountMap& c)
{
    p->__addObject(c);
}

bool
ram::control::__usesClasses(const ControllerPtr& p)
{
    return p->__usesClasses();
}

void
ram::control::__decRefUnsafe(const ControllerPtr& p)
{
    p->__decRefUnsafe();
}

void
ram::control::__clearHandleUnsafe(ControllerPtr& p)
{
    p.__clearHandleUnsafe();
}

::Ice::Double
IceProxy::ram::math::Vector::Dot(const ::ram::math::VectorPtr& v, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Vector__Dot_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Vector* __del = dynamic_cast< ::IceDelegate::ram::math::Vector*>(__delBase.get());
            return __del->Dot(v, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::Ice::Double
IceProxy::ram::math::Vector::Norm(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Vector__Norm_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Vector* __del = dynamic_cast< ::IceDelegate::ram::math::Vector*>(__delBase.get());
            return __del->Norm(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::Ice::Double
IceProxy::ram::math::Vector::NormSquared(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Vector__NormSquared_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Vector* __del = dynamic_cast< ::IceDelegate::ram::math::Vector*>(__delBase.get());
            return __del->NormSquared(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::VectorPtr
IceProxy::ram::math::Vector::Normalize(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Vector__Normalize_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Vector* __del = dynamic_cast< ::IceDelegate::ram::math::Vector*>(__delBase.get());
            return __del->Normalize(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::VectorPtr
IceProxy::ram::math::Vector::Add(const ::ram::math::VectorPtr& v, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Vector__Add_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Vector* __del = dynamic_cast< ::IceDelegate::ram::math::Vector*>(__delBase.get());
            return __del->Add(v, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::VectorPtr
IceProxy::ram::math::Vector::Subtract(const ::ram::math::VectorPtr& v, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Vector__Subtract_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Vector* __del = dynamic_cast< ::IceDelegate::ram::math::Vector*>(__delBase.get());
            return __del->Subtract(v, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::VectorPtr
IceProxy::ram::math::Vector::Multiply(::Ice::Double s, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Vector__Multiply_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Vector* __del = dynamic_cast< ::IceDelegate::ram::math::Vector*>(__delBase.get());
            return __del->Multiply(s, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::VectorPtr
IceProxy::ram::math::Vector::Negate(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Vector__Negate_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Vector* __del = dynamic_cast< ::IceDelegate::ram::math::Vector*>(__delBase.get());
            return __del->Negate(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::VectorPtr
IceProxy::ram::math::Vector::CrossProduct(const ::ram::math::VectorPtr& v, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Vector__CrossProduct_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Vector* __del = dynamic_cast< ::IceDelegate::ram::math::Vector*>(__delBase.get());
            return __del->CrossProduct(v, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

const ::std::string&
IceProxy::ram::math::Vector::ice_staticId()
{
    return ::ram::math::Vector::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::ram::math::Vector::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::ram::math::Vector);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::ram::math::Vector::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::ram::math::Vector);
}

bool
IceProxy::ram::math::operator==(const ::IceProxy::ram::math::Vector& l, const ::IceProxy::ram::math::Vector& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) == static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::ram::math::operator!=(const ::IceProxy::ram::math::Vector& l, const ::IceProxy::ram::math::Vector& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) != static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::ram::math::operator<(const ::IceProxy::ram::math::Vector& l, const ::IceProxy::ram::math::Vector& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) < static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::ram::math::operator<=(const ::IceProxy::ram::math::Vector& l, const ::IceProxy::ram::math::Vector& r)
{
    return l < r || l == r;
}

bool
IceProxy::ram::math::operator>(const ::IceProxy::ram::math::Vector& l, const ::IceProxy::ram::math::Vector& r)
{
    return !(l < r) && !(l == r);
}

bool
IceProxy::ram::math::operator>=(const ::IceProxy::ram::math::Vector& l, const ::IceProxy::ram::math::Vector& r)
{
    return !(l < r);
}

::Ice::Double
IceProxy::ram::math::Quaternion::Dot(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__Dot_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->Dot(q, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::Ice::Double
IceProxy::ram::math::Quaternion::Norm(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__Norm_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->Norm(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::Ice::Double
IceProxy::ram::math::Quaternion::NormSquared(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__NormSquared_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->NormSquared(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::QuaternionPtr
IceProxy::ram::math::Quaternion::Normalize(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__Normalize_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->Normalize(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::QuaternionPtr
IceProxy::ram::math::Quaternion::Add(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__Add_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->Add(q, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::QuaternionPtr
IceProxy::ram::math::Quaternion::Subtract(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__Subtract_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->Subtract(q, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::QuaternionPtr
IceProxy::ram::math::Quaternion::MultiplyQuaternion(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__MultiplyQuaternion_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->MultiplyQuaternion(q, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::QuaternionPtr
IceProxy::ram::math::Quaternion::MultiplyScalar(::Ice::Double s, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__MultiplyScalar_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->MultiplyScalar(s, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::QuaternionPtr
IceProxy::ram::math::Quaternion::Inverse(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__Inverse_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->Inverse(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::QuaternionPtr
IceProxy::ram::math::Quaternion::Negate(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__Negate_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->Negate(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::VectorPtr
IceProxy::ram::math::Quaternion::MultiplyVector(const ::ram::math::VectorPtr& v, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__math__Quaternion__MultiplyVector_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::math::Quaternion* __del = dynamic_cast< ::IceDelegate::ram::math::Quaternion*>(__delBase.get());
            return __del->MultiplyVector(v, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

const ::std::string&
IceProxy::ram::math::Quaternion::ice_staticId()
{
    return ::ram::math::Quaternion::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::ram::math::Quaternion::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::ram::math::Quaternion);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::ram::math::Quaternion::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::ram::math::Quaternion);
}

bool
IceProxy::ram::math::operator==(const ::IceProxy::ram::math::Quaternion& l, const ::IceProxy::ram::math::Quaternion& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) == static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::ram::math::operator!=(const ::IceProxy::ram::math::Quaternion& l, const ::IceProxy::ram::math::Quaternion& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) != static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::ram::math::operator<(const ::IceProxy::ram::math::Quaternion& l, const ::IceProxy::ram::math::Quaternion& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) < static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::ram::math::operator<=(const ::IceProxy::ram::math::Quaternion& l, const ::IceProxy::ram::math::Quaternion& r)
{
    return l < r || l == r;
}

bool
IceProxy::ram::math::operator>(const ::IceProxy::ram::math::Quaternion& l, const ::IceProxy::ram::math::Quaternion& r)
{
    return !(l < r) && !(l == r);
}

bool
IceProxy::ram::math::operator>=(const ::IceProxy::ram::math::Quaternion& l, const ::IceProxy::ram::math::Quaternion& r)
{
    return !(l < r);
}

void
IceProxy::ram::control::Controller::setSpeed(::Ice::Double speed, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            __del->setSpeed(speed, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

void
IceProxy::ram::control::Controller::setDepth(::Ice::Double depth, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            __del->setDepth(depth, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::Ice::Double
IceProxy::ram::control::Controller::getSpeed(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__control__Controller__getSpeed_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            return __del->getSpeed(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::Ice::Double
IceProxy::ram::control::Controller::getDepth(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__control__Controller__getDepth_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            return __del->getDepth(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::Ice::Double
IceProxy::ram::control::Controller::getEstimatedDepth(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__control__Controller__getEstimatedDepth_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            return __del->getEstimatedDepth(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::Ice::Double
IceProxy::ram::control::Controller::getEstimatedDepthDot(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__control__Controller__getEstimatedDepthDot_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            return __del->getEstimatedDepthDot(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

void
IceProxy::ram::control::Controller::yawVehicle(::Ice::Double degrees, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            __del->yawVehicle(degrees, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

void
IceProxy::ram::control::Controller::pitchVehicle(::Ice::Double degrees, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            __del->pitchVehicle(degrees, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

void
IceProxy::ram::control::Controller::rollVehicle(::Ice::Double degrees, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            __del->rollVehicle(degrees, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::ram::math::QuaternionPtr
IceProxy::ram::control::Controller::getDesiredOrientation(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__control__Controller__getDesiredOrientation_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            return __del->getDesiredOrientation(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

bool
IceProxy::ram::control::Controller::atDepth(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__control__Controller__atDepth_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            return __del->atDepth(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

bool
IceProxy::ram::control::Controller::atOrientation(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__ram__control__Controller__atOrientation_name);
            __delBase = __getDelegate();
            ::IceDelegate::ram::control::Controller* __del = dynamic_cast< ::IceDelegate::ram::control::Controller*>(__delBase.get());
            return __del->atOrientation(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

const ::std::string&
IceProxy::ram::control::Controller::ice_staticId()
{
    return ::ram::control::Controller::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::ram::control::Controller::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::ram::control::Controller);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::ram::control::Controller::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::ram::control::Controller);
}

bool
IceProxy::ram::control::operator==(const ::IceProxy::ram::control::Controller& l, const ::IceProxy::ram::control::Controller& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) == static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::ram::control::operator!=(const ::IceProxy::ram::control::Controller& l, const ::IceProxy::ram::control::Controller& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) != static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::ram::control::operator<(const ::IceProxy::ram::control::Controller& l, const ::IceProxy::ram::control::Controller& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) < static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::ram::control::operator<=(const ::IceProxy::ram::control::Controller& l, const ::IceProxy::ram::control::Controller& r)
{
    return l < r || l == r;
}

bool
IceProxy::ram::control::operator>(const ::IceProxy::ram::control::Controller& l, const ::IceProxy::ram::control::Controller& r)
{
    return !(l < r) && !(l == r);
}

bool
IceProxy::ram::control::operator>=(const ::IceProxy::ram::control::Controller& l, const ::IceProxy::ram::control::Controller& r)
{
    return !(l < r);
}

::Ice::Double
IceDelegateM::ram::math::Vector::Dot(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Vector__Dot_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::ram::math::__write(__os, v);
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateM::ram::math::Vector::Norm(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Vector__Norm_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateM::ram::math::Vector::NormSquared(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Vector__NormSquared_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::VectorPtr
IceDelegateM::ram::math::Vector::Normalize(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Vector__Normalize_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::VectorPtr __ret;
        __is->read(::ram::math::__patch__VectorPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::VectorPtr
IceDelegateM::ram::math::Vector::Add(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Vector__Add_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::ram::math::__write(__os, v);
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::VectorPtr __ret;
        __is->read(::ram::math::__patch__VectorPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::VectorPtr
IceDelegateM::ram::math::Vector::Subtract(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Vector__Subtract_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::ram::math::__write(__os, v);
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::VectorPtr __ret;
        __is->read(::ram::math::__patch__VectorPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::VectorPtr
IceDelegateM::ram::math::Vector::Multiply(::Ice::Double s, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Vector__Multiply_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(s);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::VectorPtr __ret;
        __is->read(::ram::math::__patch__VectorPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::VectorPtr
IceDelegateM::ram::math::Vector::Negate(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Vector__Negate_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::VectorPtr __ret;
        __is->read(::ram::math::__patch__VectorPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::VectorPtr
IceDelegateM::ram::math::Vector::CrossProduct(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Vector__CrossProduct_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::ram::math::__write(__os, v);
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::VectorPtr __ret;
        __is->read(::ram::math::__patch__VectorPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateM::ram::math::Quaternion::Dot(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__Dot_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::ram::math::__write(__os, q);
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateM::ram::math::Quaternion::Norm(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__Norm_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateM::ram::math::Quaternion::NormSquared(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__NormSquared_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::QuaternionPtr
IceDelegateM::ram::math::Quaternion::Normalize(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__Normalize_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::QuaternionPtr __ret;
        __is->read(::ram::math::__patch__QuaternionPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::QuaternionPtr
IceDelegateM::ram::math::Quaternion::Add(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__Add_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::ram::math::__write(__os, q);
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::QuaternionPtr __ret;
        __is->read(::ram::math::__patch__QuaternionPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::QuaternionPtr
IceDelegateM::ram::math::Quaternion::Subtract(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__Subtract_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::ram::math::__write(__os, q);
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::QuaternionPtr __ret;
        __is->read(::ram::math::__patch__QuaternionPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::QuaternionPtr
IceDelegateM::ram::math::Quaternion::MultiplyQuaternion(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__MultiplyQuaternion_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::ram::math::__write(__os, q);
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::QuaternionPtr __ret;
        __is->read(::ram::math::__patch__QuaternionPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::QuaternionPtr
IceDelegateM::ram::math::Quaternion::MultiplyScalar(::Ice::Double s, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__MultiplyScalar_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(s);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::QuaternionPtr __ret;
        __is->read(::ram::math::__patch__QuaternionPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::QuaternionPtr
IceDelegateM::ram::math::Quaternion::Inverse(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__Inverse_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::QuaternionPtr __ret;
        __is->read(::ram::math::__patch__QuaternionPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::QuaternionPtr
IceDelegateM::ram::math::Quaternion::Negate(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__Negate_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::QuaternionPtr __ret;
        __is->read(::ram::math::__patch__QuaternionPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::VectorPtr
IceDelegateM::ram::math::Quaternion::MultiplyVector(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__math__Quaternion__MultiplyVector_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::ram::math::__write(__os, v);
        __os->writePendingObjects();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::VectorPtr __ret;
        __is->read(::ram::math::__patch__VectorPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::ram::control::Controller::setSpeed(::Ice::Double speed, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__setSpeed_name, ::Ice::Normal, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(speed);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::ram::control::Controller::setDepth(::Ice::Double depth, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__setDepth_name, ::Ice::Normal, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(depth);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateM::ram::control::Controller::getSpeed(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__getSpeed_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateM::ram::control::Controller::getDepth(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__getDepth_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateM::ram::control::Controller::getEstimatedDepth(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__getEstimatedDepth_name, ::Ice::Normal, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateM::ram::control::Controller::getEstimatedDepthDot(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__getEstimatedDepthDot_name, ::Ice::Normal, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::Ice::Double __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::ram::control::Controller::yawVehicle(::Ice::Double degrees, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__yawVehicle_name, ::Ice::Normal, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(degrees);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::ram::control::Controller::pitchVehicle(::Ice::Double degrees, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__pitchVehicle_name, ::Ice::Normal, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(degrees);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::ram::control::Controller::rollVehicle(::Ice::Double degrees, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__rollVehicle_name, ::Ice::Normal, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(degrees);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::ram::math::QuaternionPtr
IceDelegateM::ram::control::Controller::getDesiredOrientation(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__getDesiredOrientation_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::ram::math::QuaternionPtr __ret;
        __is->read(::ram::math::__patch__QuaternionPtr, &__ret);
        __is->readPendingObjects();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

bool
IceDelegateM::ram::control::Controller::atDepth(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__atDepth_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        bool __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

bool
IceDelegateM::ram::control::Controller::atOrientation(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __ram__control__Controller__atOrientation_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        bool __ret;
        __is->read(__ret);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::Ice::Double
IceDelegateD::ram::math::Vector::Dot(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Vector__Dot_name, ::Ice::Idempotent, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Vector* __servant = dynamic_cast< ::ram::math::Vector*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Dot(v, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::Ice::Double
IceDelegateD::ram::math::Vector::Norm(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Vector__Norm_name, ::Ice::Idempotent, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Vector* __servant = dynamic_cast< ::ram::math::Vector*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Norm(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::Ice::Double
IceDelegateD::ram::math::Vector::NormSquared(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Vector__NormSquared_name, ::Ice::Idempotent, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Vector* __servant = dynamic_cast< ::ram::math::Vector*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->NormSquared(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::VectorPtr
IceDelegateD::ram::math::Vector::Normalize(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Vector__Normalize_name, ::Ice::Idempotent, __context);
    ::ram::math::VectorPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Vector* __servant = dynamic_cast< ::ram::math::Vector*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Normalize(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::VectorPtr
IceDelegateD::ram::math::Vector::Add(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Vector__Add_name, ::Ice::Idempotent, __context);
    ::ram::math::VectorPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Vector* __servant = dynamic_cast< ::ram::math::Vector*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Add(v, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::VectorPtr
IceDelegateD::ram::math::Vector::Subtract(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Vector__Subtract_name, ::Ice::Idempotent, __context);
    ::ram::math::VectorPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Vector* __servant = dynamic_cast< ::ram::math::Vector*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Subtract(v, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::VectorPtr
IceDelegateD::ram::math::Vector::Multiply(::Ice::Double s, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Vector__Multiply_name, ::Ice::Idempotent, __context);
    ::ram::math::VectorPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Vector* __servant = dynamic_cast< ::ram::math::Vector*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Multiply(s, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::VectorPtr
IceDelegateD::ram::math::Vector::Negate(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Vector__Negate_name, ::Ice::Idempotent, __context);
    ::ram::math::VectorPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Vector* __servant = dynamic_cast< ::ram::math::Vector*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Negate(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::VectorPtr
IceDelegateD::ram::math::Vector::CrossProduct(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Vector__CrossProduct_name, ::Ice::Idempotent, __context);
    ::ram::math::VectorPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Vector* __servant = dynamic_cast< ::ram::math::Vector*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->CrossProduct(v, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::Ice::Double
IceDelegateD::ram::math::Quaternion::Dot(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__Dot_name, ::Ice::Idempotent, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Dot(q, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::Ice::Double
IceDelegateD::ram::math::Quaternion::Norm(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__Norm_name, ::Ice::Idempotent, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Norm(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::Ice::Double
IceDelegateD::ram::math::Quaternion::NormSquared(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__NormSquared_name, ::Ice::Idempotent, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->NormSquared(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::QuaternionPtr
IceDelegateD::ram::math::Quaternion::Normalize(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__Normalize_name, ::Ice::Idempotent, __context);
    ::ram::math::QuaternionPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Normalize(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::QuaternionPtr
IceDelegateD::ram::math::Quaternion::Add(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__Add_name, ::Ice::Idempotent, __context);
    ::ram::math::QuaternionPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Add(q, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::QuaternionPtr
IceDelegateD::ram::math::Quaternion::Subtract(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__Subtract_name, ::Ice::Idempotent, __context);
    ::ram::math::QuaternionPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Subtract(q, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::QuaternionPtr
IceDelegateD::ram::math::Quaternion::MultiplyQuaternion(const ::ram::math::QuaternionPtr& q, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__MultiplyQuaternion_name, ::Ice::Idempotent, __context);
    ::ram::math::QuaternionPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->MultiplyQuaternion(q, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::QuaternionPtr
IceDelegateD::ram::math::Quaternion::MultiplyScalar(::Ice::Double s, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__MultiplyScalar_name, ::Ice::Idempotent, __context);
    ::ram::math::QuaternionPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->MultiplyScalar(s, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::QuaternionPtr
IceDelegateD::ram::math::Quaternion::Inverse(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__Inverse_name, ::Ice::Idempotent, __context);
    ::ram::math::QuaternionPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Inverse(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::QuaternionPtr
IceDelegateD::ram::math::Quaternion::Negate(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__Negate_name, ::Ice::Idempotent, __context);
    ::ram::math::QuaternionPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->Negate(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::ram::math::VectorPtr
IceDelegateD::ram::math::Quaternion::MultiplyVector(const ::ram::math::VectorPtr& v, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__math__Quaternion__MultiplyVector_name, ::Ice::Idempotent, __context);
    ::ram::math::VectorPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::math::Quaternion* __servant = dynamic_cast< ::ram::math::Quaternion*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->MultiplyVector(v, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

void
IceDelegateD::ram::control::Controller::setSpeed(::Ice::Double speed, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__setSpeed_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __servant->setSpeed(speed, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return;
}

void
IceDelegateD::ram::control::Controller::setDepth(::Ice::Double depth, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__setDepth_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __servant->setDepth(depth, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return;
}

::Ice::Double
IceDelegateD::ram::control::Controller::getSpeed(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__getSpeed_name, ::Ice::Idempotent, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->getSpeed(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::Ice::Double
IceDelegateD::ram::control::Controller::getDepth(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__getDepth_name, ::Ice::Idempotent, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->getDepth(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::Ice::Double
IceDelegateD::ram::control::Controller::getEstimatedDepth(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__getEstimatedDepth_name, ::Ice::Normal, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->getEstimatedDepth(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

::Ice::Double
IceDelegateD::ram::control::Controller::getEstimatedDepthDot(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__getEstimatedDepthDot_name, ::Ice::Normal, __context);
    ::Ice::Double __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->getEstimatedDepthDot(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

void
IceDelegateD::ram::control::Controller::yawVehicle(::Ice::Double degrees, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__yawVehicle_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __servant->yawVehicle(degrees, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return;
}

void
IceDelegateD::ram::control::Controller::pitchVehicle(::Ice::Double degrees, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__pitchVehicle_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __servant->pitchVehicle(degrees, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return;
}

void
IceDelegateD::ram::control::Controller::rollVehicle(::Ice::Double degrees, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__rollVehicle_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __servant->rollVehicle(degrees, __current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return;
}

::ram::math::QuaternionPtr
IceDelegateD::ram::control::Controller::getDesiredOrientation(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__getDesiredOrientation_name, ::Ice::Idempotent, __context);
    ::ram::math::QuaternionPtr __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->getDesiredOrientation(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

bool
IceDelegateD::ram::control::Controller::atDepth(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__atDepth_name, ::Ice::Idempotent, __context);
    bool __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->atDepth(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

bool
IceDelegateD::ram::control::Controller::atOrientation(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __ram__control__Controller__atOrientation_name, ::Ice::Idempotent, __context);
    bool __ret;
    try
    {
        ::IceInternal::Direct __direct(__current);
        ::ram::control::Controller* __servant = dynamic_cast< ::ram::control::Controller*>(__direct.servant().get());
        if(!__servant)
        {
            __direct.destroy();
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, __current.id, __current.facet, __current.operation);
        }
        try
        {
            __ret = __servant->atOrientation(__current);
        }
        catch(const ::Ice::UserException&)
        {
            __direct.destroy();
            throw;
        }
        catch(const ::std::exception& __ex)
        {
            __direct.destroy();
            ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
        }
        catch(...)
        {
            __direct.destroy();
            throw ::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception");
        }
        __direct.destroy();
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwUnknownWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __ret;
}

ram::math::Vector::Vector(::Ice::Double __ice_x, ::Ice::Double __ice_y, ::Ice::Double __ice_z) :
    x(__ice_x),
    y(__ice_y),
    z(__ice_z)
{
}

::Ice::ObjectPtr
ram::math::Vector::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __ram__math__Vector_ids[2] =
{
    "::Ice::Object",
    "::ram::math::Vector"
};

bool
ram::math::Vector::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__ram__math__Vector_ids, __ram__math__Vector_ids + 2, _s);
}

::std::vector< ::std::string>
ram::math::Vector::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__ram__math__Vector_ids[0], &__ram__math__Vector_ids[2]);
}

const ::std::string&
ram::math::Vector::ice_id(const ::Ice::Current&) const
{
    return __ram__math__Vector_ids[1];
}

const ::std::string&
ram::math::Vector::ice_staticId()
{
    return __ram__math__Vector_ids[1];
}

::IceInternal::DispatchStatus
ram::math::Vector::___Dot(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::VectorPtr v;
    __is->read(::ram::math::__patch__VectorPtr, &v);
    __is->readPendingObjects();
    ::Ice::Double __ret = Dot(v, __current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Vector::___Norm(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double __ret = Norm(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Vector::___NormSquared(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double __ret = NormSquared(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Vector::___Normalize(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::VectorPtr __ret = Normalize(__current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Vector::___Add(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::VectorPtr v;
    __is->read(::ram::math::__patch__VectorPtr, &v);
    __is->readPendingObjects();
    ::ram::math::VectorPtr __ret = Add(v, __current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Vector::___Subtract(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::VectorPtr v;
    __is->read(::ram::math::__patch__VectorPtr, &v);
    __is->readPendingObjects();
    ::ram::math::VectorPtr __ret = Subtract(v, __current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Vector::___Multiply(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double s;
    __is->read(s);
    ::ram::math::VectorPtr __ret = Multiply(s, __current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Vector::___Negate(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::VectorPtr __ret = Negate(__current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Vector::___CrossProduct(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::VectorPtr v;
    __is->read(::ram::math::__patch__VectorPtr, &v);
    __is->readPendingObjects();
    ::ram::math::VectorPtr __ret = CrossProduct(v, __current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

static ::std::string __ram__math__Vector_all[] =
{
    "Add",
    "CrossProduct",
    "Dot",
    "Multiply",
    "Negate",
    "Norm",
    "NormSquared",
    "Normalize",
    "Subtract",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping"
};

::IceInternal::DispatchStatus
ram::math::Vector::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__ram__math__Vector_all, __ram__math__Vector_all + 13, current.operation);
    if(r.first == r.second)
    {
        return ::IceInternal::DispatchOperationNotExist;
    }

    switch(r.first - __ram__math__Vector_all)
    {
        case 0:
        {
            return ___Add(in, current);
        }
        case 1:
        {
            return ___CrossProduct(in, current);
        }
        case 2:
        {
            return ___Dot(in, current);
        }
        case 3:
        {
            return ___Multiply(in, current);
        }
        case 4:
        {
            return ___Negate(in, current);
        }
        case 5:
        {
            return ___Norm(in, current);
        }
        case 6:
        {
            return ___NormSquared(in, current);
        }
        case 7:
        {
            return ___Normalize(in, current);
        }
        case 8:
        {
            return ___Subtract(in, current);
        }
        case 9:
        {
            return ___ice_id(in, current);
        }
        case 10:
        {
            return ___ice_ids(in, current);
        }
        case 11:
        {
            return ___ice_isA(in, current);
        }
        case 12:
        {
            return ___ice_ping(in, current);
        }
    }

    assert(false);
    return ::IceInternal::DispatchOperationNotExist;
}

void
ram::math::Vector::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->write(x);
    __os->write(y);
    __os->write(z);
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
ram::math::Vector::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->read(x);
    __is->read(y);
    __is->read(z);
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
ram::math::Vector::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type ram::math::Vector was not generated with stream support";
    throw ex;
}

void
ram::math::Vector::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type ram::math::Vector was not generated with stream support";
    throw ex;
}

void 
ram::math::__patch__VectorPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::ram::math::VectorPtr* p = static_cast< ::ram::math::VectorPtr*>(__addr);
    assert(p);
    *p = ::ram::math::VectorPtr::dynamicCast(v);
    if(v && !*p)
    {
        ::Ice::UnexpectedObjectException e(__FILE__, __LINE__);
        e.type = v->ice_id();
        e.expectedType = ::ram::math::Vector::ice_staticId();
        throw e;
    }
}

bool
ram::math::operator==(const ::ram::math::Vector& l, const ::ram::math::Vector& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
ram::math::operator!=(const ::ram::math::Vector& l, const ::ram::math::Vector& r)
{
    return static_cast<const ::Ice::Object&>(l) != static_cast<const ::Ice::Object&>(r);
}

bool
ram::math::operator<(const ::ram::math::Vector& l, const ::ram::math::Vector& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

bool
ram::math::operator<=(const ::ram::math::Vector& l, const ::ram::math::Vector& r)
{
    return l < r || l == r;
}

bool
ram::math::operator>(const ::ram::math::Vector& l, const ::ram::math::Vector& r)
{
    return !(l < r) && !(l == r);
}

bool
ram::math::operator>=(const ::ram::math::Vector& l, const ::ram::math::Vector& r)
{
    return !(l < r);
}

ram::math::Quaternion::Quaternion(::Ice::Double __ice_w, ::Ice::Double __ice_x, ::Ice::Double __ice_y, ::Ice::Double __ice_z) :
    w(__ice_w),
    x(__ice_x),
    y(__ice_y),
    z(__ice_z)
{
}

::Ice::ObjectPtr
ram::math::Quaternion::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __ram__math__Quaternion_ids[2] =
{
    "::Ice::Object",
    "::ram::math::Quaternion"
};

bool
ram::math::Quaternion::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__ram__math__Quaternion_ids, __ram__math__Quaternion_ids + 2, _s);
}

::std::vector< ::std::string>
ram::math::Quaternion::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__ram__math__Quaternion_ids[0], &__ram__math__Quaternion_ids[2]);
}

const ::std::string&
ram::math::Quaternion::ice_id(const ::Ice::Current&) const
{
    return __ram__math__Quaternion_ids[1];
}

const ::std::string&
ram::math::Quaternion::ice_staticId()
{
    return __ram__math__Quaternion_ids[1];
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___Dot(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::QuaternionPtr q;
    __is->read(::ram::math::__patch__QuaternionPtr, &q);
    __is->readPendingObjects();
    ::Ice::Double __ret = Dot(q, __current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___Norm(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double __ret = Norm(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___NormSquared(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double __ret = NormSquared(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___Normalize(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::QuaternionPtr __ret = Normalize(__current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___Add(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::QuaternionPtr q;
    __is->read(::ram::math::__patch__QuaternionPtr, &q);
    __is->readPendingObjects();
    ::ram::math::QuaternionPtr __ret = Add(q, __current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___Subtract(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::QuaternionPtr q;
    __is->read(::ram::math::__patch__QuaternionPtr, &q);
    __is->readPendingObjects();
    ::ram::math::QuaternionPtr __ret = Subtract(q, __current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___MultiplyQuaternion(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::QuaternionPtr q;
    __is->read(::ram::math::__patch__QuaternionPtr, &q);
    __is->readPendingObjects();
    ::ram::math::QuaternionPtr __ret = MultiplyQuaternion(q, __current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___MultiplyScalar(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double s;
    __is->read(s);
    ::ram::math::QuaternionPtr __ret = MultiplyScalar(s, __current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___Inverse(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::QuaternionPtr __ret = Inverse(__current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___Negate(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::QuaternionPtr __ret = Negate(__current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::math::Quaternion::___MultiplyVector(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::VectorPtr v;
    __is->read(::ram::math::__patch__VectorPtr, &v);
    __is->readPendingObjects();
    ::ram::math::VectorPtr __ret = MultiplyVector(v, __current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

static ::std::string __ram__math__Quaternion_all[] =
{
    "Add",
    "Dot",
    "Inverse",
    "MultiplyQuaternion",
    "MultiplyScalar",
    "MultiplyVector",
    "Negate",
    "Norm",
    "NormSquared",
    "Normalize",
    "Subtract",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping"
};

::IceInternal::DispatchStatus
ram::math::Quaternion::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__ram__math__Quaternion_all, __ram__math__Quaternion_all + 15, current.operation);
    if(r.first == r.second)
    {
        return ::IceInternal::DispatchOperationNotExist;
    }

    switch(r.first - __ram__math__Quaternion_all)
    {
        case 0:
        {
            return ___Add(in, current);
        }
        case 1:
        {
            return ___Dot(in, current);
        }
        case 2:
        {
            return ___Inverse(in, current);
        }
        case 3:
        {
            return ___MultiplyQuaternion(in, current);
        }
        case 4:
        {
            return ___MultiplyScalar(in, current);
        }
        case 5:
        {
            return ___MultiplyVector(in, current);
        }
        case 6:
        {
            return ___Negate(in, current);
        }
        case 7:
        {
            return ___Norm(in, current);
        }
        case 8:
        {
            return ___NormSquared(in, current);
        }
        case 9:
        {
            return ___Normalize(in, current);
        }
        case 10:
        {
            return ___Subtract(in, current);
        }
        case 11:
        {
            return ___ice_id(in, current);
        }
        case 12:
        {
            return ___ice_ids(in, current);
        }
        case 13:
        {
            return ___ice_isA(in, current);
        }
        case 14:
        {
            return ___ice_ping(in, current);
        }
    }

    assert(false);
    return ::IceInternal::DispatchOperationNotExist;
}

void
ram::math::Quaternion::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->write(w);
    __os->write(x);
    __os->write(y);
    __os->write(z);
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
ram::math::Quaternion::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->read(w);
    __is->read(x);
    __is->read(y);
    __is->read(z);
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
ram::math::Quaternion::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type ram::math::Quaternion was not generated with stream support";
    throw ex;
}

void
ram::math::Quaternion::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type ram::math::Quaternion was not generated with stream support";
    throw ex;
}

void 
ram::math::__patch__QuaternionPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::ram::math::QuaternionPtr* p = static_cast< ::ram::math::QuaternionPtr*>(__addr);
    assert(p);
    *p = ::ram::math::QuaternionPtr::dynamicCast(v);
    if(v && !*p)
    {
        ::Ice::UnexpectedObjectException e(__FILE__, __LINE__);
        e.type = v->ice_id();
        e.expectedType = ::ram::math::Quaternion::ice_staticId();
        throw e;
    }
}

bool
ram::math::operator==(const ::ram::math::Quaternion& l, const ::ram::math::Quaternion& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
ram::math::operator!=(const ::ram::math::Quaternion& l, const ::ram::math::Quaternion& r)
{
    return static_cast<const ::Ice::Object&>(l) != static_cast<const ::Ice::Object&>(r);
}

bool
ram::math::operator<(const ::ram::math::Quaternion& l, const ::ram::math::Quaternion& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

bool
ram::math::operator<=(const ::ram::math::Quaternion& l, const ::ram::math::Quaternion& r)
{
    return l < r || l == r;
}

bool
ram::math::operator>(const ::ram::math::Quaternion& l, const ::ram::math::Quaternion& r)
{
    return !(l < r) && !(l == r);
}

bool
ram::math::operator>=(const ::ram::math::Quaternion& l, const ::ram::math::Quaternion& r)
{
    return !(l < r);
}

::Ice::ObjectPtr
ram::control::Controller::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __ram__control__Controller_ids[2] =
{
    "::Ice::Object",
    "::ram::control::Controller"
};

bool
ram::control::Controller::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__ram__control__Controller_ids, __ram__control__Controller_ids + 2, _s);
}

::std::vector< ::std::string>
ram::control::Controller::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__ram__control__Controller_ids[0], &__ram__control__Controller_ids[2]);
}

const ::std::string&
ram::control::Controller::ice_id(const ::Ice::Current&) const
{
    return __ram__control__Controller_ids[1];
}

const ::std::string&
ram::control::Controller::ice_staticId()
{
    return __ram__control__Controller_ids[1];
}

::IceInternal::DispatchStatus
ram::control::Controller::___setSpeed(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::Ice::Double speed;
    __is->read(speed);
    setSpeed(speed, __current);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___setDepth(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::Ice::Double depth;
    __is->read(depth);
    setDepth(depth, __current);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___getSpeed(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double __ret = getSpeed(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___getDepth(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double __ret = getDepth(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___getEstimatedDepth(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double __ret = getEstimatedDepth(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___getEstimatedDepthDot(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::Ice::Double __ret = getEstimatedDepthDot(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___yawVehicle(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::Ice::Double degrees;
    __is->read(degrees);
    yawVehicle(degrees, __current);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___pitchVehicle(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::Ice::Double degrees;
    __is->read(degrees);
    pitchVehicle(degrees, __current);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___rollVehicle(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::Ice::Double degrees;
    __is->read(degrees);
    rollVehicle(degrees, __current);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___getDesiredOrientation(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    ::ram::math::QuaternionPtr __ret = getDesiredOrientation(__current);
    ::ram::math::__write(__os, __ret);
    __os->writePendingObjects();
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___atDepth(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    bool __ret = atDepth(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
ram::control::Controller::___atOrientation(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __os = __inS.os();
    bool __ret = atOrientation(__current);
    __os->write(__ret);
    return ::IceInternal::DispatchOK;
}

static ::std::string __ram__control__Controller_all[] =
{
    "atDepth",
    "atOrientation",
    "getDepth",
    "getDesiredOrientation",
    "getEstimatedDepth",
    "getEstimatedDepthDot",
    "getSpeed",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "pitchVehicle",
    "rollVehicle",
    "setDepth",
    "setSpeed",
    "yawVehicle"
};

::IceInternal::DispatchStatus
ram::control::Controller::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__ram__control__Controller_all, __ram__control__Controller_all + 16, current.operation);
    if(r.first == r.second)
    {
        return ::IceInternal::DispatchOperationNotExist;
    }

    switch(r.first - __ram__control__Controller_all)
    {
        case 0:
        {
            return ___atDepth(in, current);
        }
        case 1:
        {
            return ___atOrientation(in, current);
        }
        case 2:
        {
            return ___getDepth(in, current);
        }
        case 3:
        {
            return ___getDesiredOrientation(in, current);
        }
        case 4:
        {
            return ___getEstimatedDepth(in, current);
        }
        case 5:
        {
            return ___getEstimatedDepthDot(in, current);
        }
        case 6:
        {
            return ___getSpeed(in, current);
        }
        case 7:
        {
            return ___ice_id(in, current);
        }
        case 8:
        {
            return ___ice_ids(in, current);
        }
        case 9:
        {
            return ___ice_isA(in, current);
        }
        case 10:
        {
            return ___ice_ping(in, current);
        }
        case 11:
        {
            return ___pitchVehicle(in, current);
        }
        case 12:
        {
            return ___rollVehicle(in, current);
        }
        case 13:
        {
            return ___setDepth(in, current);
        }
        case 14:
        {
            return ___setSpeed(in, current);
        }
        case 15:
        {
            return ___yawVehicle(in, current);
        }
    }

    assert(false);
    return ::IceInternal::DispatchOperationNotExist;
}

void
ram::control::Controller::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
ram::control::Controller::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
ram::control::Controller::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type ram::control::Controller was not generated with stream support";
    throw ex;
}

void
ram::control::Controller::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type ram::control::Controller was not generated with stream support";
    throw ex;
}

void 
ram::control::__patch__ControllerPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::ram::control::ControllerPtr* p = static_cast< ::ram::control::ControllerPtr*>(__addr);
    assert(p);
    *p = ::ram::control::ControllerPtr::dynamicCast(v);
    if(v && !*p)
    {
        ::Ice::UnexpectedObjectException e(__FILE__, __LINE__);
        e.type = v->ice_id();
        e.expectedType = ::ram::control::Controller::ice_staticId();
        throw e;
    }
}

bool
ram::control::operator==(const ::ram::control::Controller& l, const ::ram::control::Controller& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
ram::control::operator!=(const ::ram::control::Controller& l, const ::ram::control::Controller& r)
{
    return static_cast<const ::Ice::Object&>(l) != static_cast<const ::Ice::Object&>(r);
}

bool
ram::control::operator<(const ::ram::control::Controller& l, const ::ram::control::Controller& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

bool
ram::control::operator<=(const ::ram::control::Controller& l, const ::ram::control::Controller& r)
{
    return l < r || l == r;
}

bool
ram::control::operator>(const ::ram::control::Controller& l, const ::ram::control::Controller& r)
{
    return !(l < r) && !(l == r);
}

bool
ram::control::operator>=(const ::ram::control::Controller& l, const ::ram::control::Controller& r)
{
    return !(l < r);
}
