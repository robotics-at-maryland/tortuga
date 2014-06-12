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
#include <iostream>
#include <cassert>
#include <cstdlib>

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "core/include/Property.h"
#include "core/include/PropertySet.h"

namespace ram {
namespace core {

// ------------------------------------------------------------------------- //
//             T E M P L A T E  S P E C I A L I Z A T I O N S                //
// ------------------------------------------------------------------------- //
    
template <>
void PropertySet::loadValueFromNode<int>(core::ConfigNode config,
                                         core::ConfigNode valueNode,
                                         core::PropertyPtr prop)
{
    int defaultValue = boost::any_cast<int>(prop->getDefaultValue());
    assert(prop->getType() == Property::PT_INT && "Invalid type");
    
    prop->set(valueNode.asInt(defaultValue));
}

template <>
void PropertySet::loadValueFromNode<double>(core::ConfigNode config,
                                            core::ConfigNode valueNode,
                                            core::PropertyPtr prop)
{
    double defaultValue = boost::any_cast<double>(prop->getDefaultValue());
    assert(prop->getType() == Property::PT_DOUBLE && "Invalid type");
    
    prop->set(valueNode.asDouble(defaultValue));
}

template <>
void PropertySet::loadValueFromNode<bool>(core::ConfigNode config,
                                          core::ConfigNode valueNode,
                                          core::PropertyPtr prop)
{
    bool defaultValue = boost::any_cast<bool>(prop->getDefaultValue());
    assert(prop->getType() == Property::PT_BOOL && "Invalid type");

    int intDef = defaultValue ? 1 : 0;
    prop->set((bool)(valueNode.asInt(intDef) == 1));
}

//#ifdef RAM_WITH_MATH
// NOTE: See packages/math/src/PropertySetImp.cpp for the math specializations
//#endif // RAM_WITH_MATH

// ------------------------------------------------------------------------- //
//                    C L A S S   D E F I N I T I O N                        //
// ------------------------------------------------------------------------- //

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


void PropertySet::addPropertiesFromSet(PropertySet* propSet)
{
    BOOST_FOREACH(std::string propName, propSet->getPropertyNames())
        addProperty(propSet->getProperty(propName));
}

void PropertySet::addPropertiesFromSet(PropertySetPtr propSet)
{
    addPropertiesFromSet(propSet.get());
}

bool PropertySet::verifyConfig(core::ConfigNode config, bool assertOnError)
{
    BOOST_FOREACH(std::string nodeName, config.subNodes())
    {
        if (m_propertyMap.find(nodeName) == m_propertyMap.end())
        {
            if (assertOnError && (nodeName != "type") && (nodeName != "name"))
            { 
                std::cerr << "Is not a valid config property \"" << nodeName
                          << "\"" << std::endl;
                assert(false && "Invalid property name");
                exit(1);
            }
            return false;
        }
    }

    return true;
}
    
} // namespace core
} // namespace ram
