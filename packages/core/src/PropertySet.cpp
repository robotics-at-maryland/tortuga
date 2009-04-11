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

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "core/include/Property.h"
#include "core/include/PropertySet.h"

namespace ram {
namespace core {

PropertySet::PropertySet()
{
}

PropertySet::~PropertySet()
{
    // Drop all references (they will delete automatically)
    m_propertyMap.clear();
}

void PropertySet::addProperty(PropertyPtr prop)
{
    std::pair<PropertyMap::iterator, bool> retPair = 
        m_propertyMap.insert(PropertyMap::value_type(prop->getName(), prop));
    if (!retPair.second)
        throw std::exception();//"Duplicate property entry!");
}

bool PropertySet::hasProperty(const std::string& name) const
{
    PropertyMap::const_iterator i = m_propertyMap.find(name);
    return i != m_propertyMap.end();
}

PropertyPtr PropertySet::getProperty(const std::string& name) const
{
    PropertyMap::const_iterator i = m_propertyMap.find(name);
    if (i != m_propertyMap.end())
        return i->second;
    else
        throw std::exception();//"Property not found");

}

std::vector<std::string> PropertySet::getPropertyNames()
{
    std::vector<std::string> names;
    BOOST_FOREACH(PropertyMapPair pair, m_propertyMap)
    {
        names.push_back(pair.first);
    }
    
    std::sort(names.begin(), names.end());
    
    return names;
}

} // namespace core
} // namespace ram
