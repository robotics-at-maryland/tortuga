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
#ifndef RAM_CORE_PROPERTY_H_04_09_2009
#define RAM_CORE_PROPERTY_H_04_09_2009

#include "OgreMVCPrerequisites.h"
#include "OgreAny.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

namespace ram {
namespace core {


/** Base class for all property objects
 */
class PropertyBase
{
public:

    /// The type of a property
    enum PropertyType
    {
        PT_INT = 0,
        PT_DOUBLE = 1,
        PT_STRING = 2, 
        PT_VECTOR3 = 4,
        PT_BOOL = 7,
        PT_QUATERNION = 8
    };


    /* Construct a property.
        @param name The name of the property
        @param desc A (potentially) long description of the property
        @param pType The type of the property
        @param defaultValue The default value of the property
    */
    PropertyBase(const std::string& name, const std::string& desc, 
		 PropertyType pType, boost::any defaultValue) :
        m_name(name), m_desc(desc), m_type(pType), m_default(defaultValue) 
	{}

    /// Get the name of the property
    const std::string& getName() const { return m_name; }

    /// Get the description of the property
    const std::string& getDescription() const { return m_desc; }

    /// Get the type of the property
    PropertyType getType() const { return m_type; }

    /// Get the default value of this property
    boost::any getDefaultValue() const { return m_default; }

    /**
     * @defgroup Get value as methods
     */
    /* @{ */
    
    /// Return the current value as an Any
    virtual boost::any getValue() const = 0;
    
    /// Get as an integer
    int getAsInt(const std::string& name) const;
    
    /// Get as a double
    float getAsDouble(const std::string& name) const;
    
    /// Get a named property value as a Vector2
    math::Vector2 getAsVector2(const std::string& name) const;
    
    /// Get a named property value as a Vector3
    math::Vector3 getAsVector3(const std::string& name) const;
    
    /// Get a named property value as a boolean
    bool getAsBool(const std::string& name) const;

    /// Get a named property value as a quaternion
    math::Quaternion getAsQuat(const std::string& name) const;
    
    /* }@ */

    /**
     * @defgroup Set value as methods
     */
    /* @{ */

    /// Set property as an integer
    void set(int val);

    /// Set property as a float
    void set(float val);

    /// Set property as a Vector2
    void set(const math::Vector2& val);

    /// Set property as a Vector3
    void set(const math::Vector3& val);

    /// Set property as a boolean
    void set(bool val);

    /// Set property as a quaternion
    void set(const math::Quaternion& val);

    /* }@ */    

protected:
    // no default construction
    Property() {}

    std::string m_name;
    std::string m_desc;
    PropertyType m_type;
    boost::any m_default;
};
    
    /// Map from property name to shared definition
    typedef std::map<std::string, PropertyDef> PropertyDefMap;

    /** Base interface for an instance of a property.
    */
    class PropertyBase
    {
    public:
        /// Constructor
        PropertyBase(PropertyDef* def) : mDef(def) {}
        virtual ~PropertyBase() {}

        /// Get the name of the property
        const std::string& getName() const { return mDef->getName(); }

        /// Get the description of the property
        const std::string& getDescription() const { return mDef->getDescription(); }

        /// Get the type of the property
        PropertyType getType() const { return mDef->getType(); }

        /// Get the default value of this property
        boost::any getDefaultValue() const { return mDef->getDefaultValue(); }




    protected:
        // disallow default construction
        PropertyBase() {}
        PropertyDef* mDef;

    };

    /** Property instance with passthrough calls to a given object. */
    template <typename T>
    class Property : public PropertyBase
    {
    public:
        typedef T value_type;
        typedef boost::function< T (void) > getter_func;
        typedef boost::function< void (T) > setter_func;

        /** Construct a property which is able to directly call a given 
            getter and setter on a specific object instance, via functors.
        */
        Property(PropertyDef* def, getter_func getter, setter_func setter)
            : PropertyBase(def)
            , mGetter(getter)
            , mSetter(setter) 
        {
        }

        /** Set the property value.
        */
        virtual void set(T val)
        {
            mSetter(val);
        }

        virtual T get() const
        {
            return mGetter();
        }

        boost::any getValue() const
        {
            return boost::any(get());
        }

    protected:
        // disallow default construction
        Property() {}

        getter_func mGetter;
        setter_func mSetter;
    };

    /** A simple structure designed just as a holder of property values between
        the instances of objects they might target. There is just enough information
        here to be able to interpret the results accurately but no more.
    */
    struct PropertyValue
    {
        PropertyType propType;
        boost::any val;
    };
    /// Defines a transferable map of properties using wrapped value types (boost::any)
    typedef std::map<std::string, PropertyValue> PropertyValueMap;


    /** Defines a complete set of properties for a single object instance.
    */
    class PropertySet
    {
    public:
        PropertySet();
        ~PropertySet();

        /** Adds a property to this set. 
        @remarks
            The PropertySet is responsible for deleting this object.
        */
        void addProperty(PropertyBase* prop);

        /** Gets the property object for a given property name. 
        @remarks
            Note that this property will need to be cast to a templated property
            compatible with the type you will be setting. You might find the 
            overloaded set and get<type> methods quicker if 
            you already know the type.
        */
        PropertyBase* getProperty(const std::string& name) const;

        /** Reports whether this property set contains a named property. */
        bool hasProperty(const std::string& name) const;

        typedef std::map<std::string, PropertyBase*> PropertyMap;
        typedef Ogre::MapIterator<PropertyMap> PropertyIterator;
        /// Get an iterator over the available properties
        PropertyIterator getPropertyIterator();

        /** Gets an independently usable collection of property values from the
            current state.
        */
        PropertyValueMap getValueMap() const;

        /** Sets the current state from a given value map.
        */
        void setValueMap(const PropertyValueMap& values);

        // Get a named property value as an integer
        int getInt(const std::string& name) const;
        // Get a named property value as a float
        float getFloat(const std::string& name) const;
        // Get a named property value as a string
        std::string getString(const std::string& name) const;
        // Get a named property value as a Vector2
        math::Vector2 getVector2(const std::string& name) const;
        // Get a named property value as a Vector3
        math::Vector3 getVector3(const std::string& name) const;
        // Get a named property value as a Vector4
        math::Vector4 getVector4(const std::string& name) const;
        // Get a named property value as a colour
        math::ColourValue getColour(const std::string& name) const;
        // Get a named property value as a boolean
        bool getBool(const std::string& name) const;
        // Get a named property value as a quaternion
        math::Quaternion getQuat(const std::string& name) const;

        // Set a named property value as an integer
        void set(const std::string& name, int val);
        // Set  a named property value as a float
        void set(const std::string& name, float val);
        // Set a named property value as a string
        void set(const std::string& name, const std::string& val);
        // Set a named property value as a string
        void set(const std::string& name, const char* val);
        // Set a named property value as a Vector2
        void set(const std::string& name, const math::Vector2& val);
        // Set a named property value as a Vector3
        void set(const std::string& name, const math::Vector3& val);
        // Set a named property value as a Vector4
        void set(const std::string& name, const math::Vector4& val);
        // Set a named property value as a colour
        void set(const std::string& name, const math::ColourValue& val);
        // Set a named property value as a boolean
        void set(const std::string& name, bool val);
        // Set a named property value as a quaternion
        void set(const std::string& name, const math::Quaternion& val);

        /** Save-to-stream method.
        */
        void save(std::ostream& stream);

        /** Load-from-stream method.
        */
        void load(std::istream& stream);


    protected:
        PropertyMap mPropertyMap;

        /// Set a named property value, internal implementation (type match required)
        template <typename T>
        void setPropertyImpl(const std::string& name, T val, PropertyType typeCheck)
        {
            PropertyBase* baseProp = getProperty(name);
            if (baseProp->getType() != typeCheck)
                throw std::exception("Type not correct");
            static_cast<Property<T>*>(baseProp)->set(val);
        }

        /// Set a named property value, internal implementation (type match required)
        template <typename T>
        void getPropertyImpl(const std::string& name, T& refVal, PropertyType typeCheck) const
        {
            PropertyBase* baseProp = getProperty(name);
            if (baseProp->getType() != typeCheck)
                throw std::exception("Type not correct");
            refVal = static_cast<Property<T>*>(baseProp)->get();
        }

    };

   
}

#endif 
