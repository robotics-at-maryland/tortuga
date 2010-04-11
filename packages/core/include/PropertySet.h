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
#include "core/include/ConfigNode.h"
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

    /** Create the property object for you, which uses the given ptr */
    template <typename T>
    void addProperty(const std::string& name, const std::string& desc, 
                     T defaultValue, T* valuePtr);

    /** Create the property object for you, which uses the getter and setter */
    template <typename T>
    void addProperty(const std::string& name, const std::string& desc, 
                     T defaultValue,
                     typename FunctionProperty<T>::GetterFunc getter,
                     typename FunctionProperty<T>::SetterFunc setter);

    /** Create the property object for you, load from the config */
    template <typename T>
    void addProperty(core::ConfigNode config, bool requireInConfig,
                     const std::string& name, const std::string& desc, 
                     T defaultValue, T* valuePtr);

    /** Create the property object for you, load from the config with min/max*/
    template <typename T>
    void addProperty(core::ConfigNode config, bool requireInConfig,
                     const std::string& name, const std::string& desc, 
                     T defaultValue, T* valuePtr, T min, T max);

    /** Create the property object for you, load from the config */
    template <typename T>
    void addProperty(core::ConfigNode config, bool requireInConfig,
                     const std::string& name, const std::string& desc,
                     T defaultValue,
                     typename FunctionProperty<T>::GetterFunc getter,
                     typename FunctionProperty<T>::SetterFunc setter);


    /** Create the property object for you, load from the config with min/max */
    template <typename T>
    void addProperty(core::ConfigNode config, bool requireInConfig,
                     const std::string& name, const std::string& desc,
                     T defaultValue,
                     typename FunctionProperty<T>::GetterFunc getter,
                     typename FunctionProperty<T>::SetterFunc setter,
                     T min, T max);
   
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

    /** Adds all the properties in the given set to this set */
    void addPropertiesFromSet(PropertySet* propSet);

    void addPropertiesFromSet(PropertySetPtr propSet);

    /** Verify the config not only contains keys for the current properties
     *
     *  @param assertOnError
     *      Causes the function to assert on error, leading to a program crash
     *      if the config is invalid.
     *
     *  @return
     *      True if the keys are valid
     */
    bool verifyConfig(core::ConfigNode config, bool assertOnError = false);
    
protected:
    /** Loads the value into the given property */
    template <typename T>
    void loadValueFromConfig(core::ConfigNode config, core::PropertyPtr prop,
                             bool requireInConfig);


    /** Specialized by type to do the actual loading */
    template <typename T>
    void loadValueFromNode(core::ConfigNode config, core::ConfigNode valueNode,
                           core::PropertyPtr prop);

    
    typedef std::map<std::string, PropertyPtr> PropertyMap;
    typedef PropertyMap::value_type PropertyMapPair;
    
    PropertyMap m_propertyMap;
};

   
} // namespace core
} // namespace ram

// Include our template definitions
#include "core/include/PropertySetDetail.h"

#endif // RAM_CORE_PROPERTYSET_H_04_10_2009
