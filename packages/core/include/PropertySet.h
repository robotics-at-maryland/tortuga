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

#ifndef RAM_CORE_PROPERTYSET_H_04_10_2009
#define RAM_CORE_PROPERTYSET_H_04_10_2009

// STD Includes
#include <string>
#include <vector>
#include <map>

// Project Includes
#include "core/include/Property.h"

namespace ram {
namespace core {
    
/** Defines a complete set of properties for a single object instance. */
class PropertySet
{
public:
    PropertySet();
    ~PropertySet();

    /** Adds a property to this set. 
     *    @remarks
     *         The PropertySet is responsible for deleting this object.
     */
    void addProperty(PropertyPtr prop);

    /** Gets the property object for a given property name. 
     *    @remarks
     *        Note that this property will need to be cast to a templated
     *        property compatible with the type you will be setting. You might
     *        find the overloaded set and get<type> methods quicker if 
     *        you already know the type.
     */
    PropertyPtr getProperty(const std::string& name) const;

    /** Reports whether this property set contains a named property. */
    bool hasProperty(const std::string& name) const;

    /** Returns a list of all property names in alphabetical order*/
    std::vector<std::string> getPropertyNames();
    
protected:
    typedef std::map<std::string, PropertyPtr> PropertyMap;
    typedef PropertyMap::value_type PropertyMapPair;
    
    PropertyMap m_propertyMap;
};

   
} // namespace core
} // namespace ram

#endif // RAM_CORE_PROPERTYSET_H_04_10_2009
