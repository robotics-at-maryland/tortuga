/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Joseph Lisee <jlisee@cmu.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@cmu.edu>
 * File:  packages/math/src/PropertyImp.cpp
 */



// Project Includes
#include "core/include/Property.h"
#include "core/include/Feature.h"

#ifdef RAM_WITH_CORE

namespace ram {
namespace core {

template <>
Property::PropertyType getPropertyType<math::Vector2>()
{
    return Property::PT_VECTOR2;
}


template <>
Property::PropertyType getPropertyType<math::Vector3>()
{
    return Property::PT_VECTOR3;
}


template <>
Property::PropertyType getPropertyType<math::Quaternion>()
{
    return Property::PT_QUATERNION;
}

template <>
Property::PropertyType getPropertyType<math::Radian>()
{
    return Property::PT_ANGLE;
}

template <>
Property::PropertyType getPropertyType<math::Degree>()
{
    return Property::PT_ANGLE;
}

} // namespace core
} // namespace ram

#endif // RAM_WITH_CORE
