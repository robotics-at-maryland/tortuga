/*
-----------------------------------------------------------------------------
This source file is part of wxOgreMVC

Copyright (c) 2007 Torus Knot Software Ltd, all rights reserved

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

-----------------------------------------------------------------------------
*/

// STD Includes
#include <cassert>

// Project Includes
#include "core/include/Property.h"

namespace ram {
namespace core {

Property::Property(const std::string& name, const std::string& desc, 
                   PropertyType pType, boost::any defaultValue) :
    m_name(name), m_desc(desc), m_type(pType), m_default(defaultValue) 
{
    assert((PT_STARTVAL < pType) && (pType < PT_END) &&
           "Invalid property type");
}

    
template <>
Property::PropertyType getPropertyType<int>()
{
    return Property::PT_INT;
}

template <>
Property::PropertyType getPropertyType<double>()
{
    return Property::PT_DOUBLE;
}


template <>
Property::PropertyType getPropertyType<bool>()
{
    return Property::PT_BOOL;
}

    
#ifdef RAM_WITH_MATH

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

    
#endif // RAM_WITH_MATH
    
    
} // namespace core
} // namespace ram
