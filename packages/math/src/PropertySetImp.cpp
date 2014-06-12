/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Joseph Lisee <jlisee@cmu.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@cmu.edu>
 * File:  packages/math/src/PropertyImp.cpp
 */


// STD Includes
#include <cassert>

// Project Includes
#include "core/include/Property.h"
#include "core/include/PropertySet.h"
#include "core/include/Feature.h"

#ifdef RAM_WITH_CORE

namespace ram {
namespace core {

template <>
   void PropertySet::loadValueFromNode<math::Radian>(core::ConfigNode config,
						  core::ConfigNode valueNode,
						  core::PropertyPtr prop)
{
    math::Radian defaultValue(
        boost::any_cast<math::Radian>(prop->getDefaultValue()));
    assert(prop->getType() == Property::PT_ANGLE && "Invalid type");

    math::Degree degreeVal(valueNode.asDouble(defaultValue.valueDegrees()));
    prop->set(math::Radian(degreeVal));
}

template <>
void PropertySet::loadValueFromNode<math::Degree>(core::ConfigNode config,
						  core::ConfigNode valueNode,
						  core::PropertyPtr prop)
{
    math::Degree defaultValue(
        boost::any_cast<math::Degree>(prop->getDefaultValue()));
    assert(prop->getType() == Property::PT_ANGLE && "Invalid type");
    
    prop->set(math::Degree(valueNode.asDouble(defaultValue.valueDegrees())));
}

template <>
void PropertySet::loadValueFromNode<math::Vector2>(core::ConfigNode config,
                                                   core::ConfigNode valueNode,
                                                   core::PropertyPtr prop)
{
    math::Vector2 defaultValue =
        boost::any_cast<math::Vector2>(prop->getDefaultValue());
    assert(prop->getType() == Property::PT_VECTOR2 && "Invalid type");

    if (config.exists(prop->getName()))
    {
        assert(valueNode.size() == 2 &&
               "Improper size for Vector2 config");
        prop->set(math::Vector2(valueNode[0].asDouble(),
                                valueNode[1].asDouble()));
    }
}

template <>
void PropertySet::loadValueFromNode<math::Vector3>(core::ConfigNode config,
                                                   core::ConfigNode valueNode,
                                                   core::PropertyPtr prop)
{
    math::Vector3 defaultValue =
        boost::any_cast<math::Vector3>(prop->getDefaultValue());
    assert(prop->getType() == Property::PT_VECTOR3 && "Invalid type");

    if (config.exists(prop->getName()))
    {
        assert(valueNode.size() == 3 &&
               "Improper size for Vector3 config");
        prop->set(math::Vector3(valueNode[0].asDouble(),
                                valueNode[1].asDouble(),
                                valueNode[2].asDouble()));
    }
}

template <>
void PropertySet::loadValueFromNode<math::Quaternion>(core::ConfigNode config,
                                                   core::ConfigNode valueNode,
                                                   core::PropertyPtr prop)
{
    math::Quaternion defaultValue =
        boost::any_cast<math::Quaternion>(prop->getDefaultValue());
    assert(prop->getType() == Property::PT_QUATERNION && "Invalid type");

    if (config.exists(prop->getName()))
    {
        assert(valueNode.size() == 4 &&
               "Improper size for Quaternion config");
        prop->set(math::Quaternion(valueNode[0].asDouble(),
                                   valueNode[1].asDouble(),
                                   valueNode[2].asDouble(),
                                   valueNode[3].asDouble()));
    }
}

} // namespace core
} // namespace ram    

#endif // RAM_WITH_CORE
