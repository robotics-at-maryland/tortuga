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

// STD Includes
#include <sstream>

// Library Includes
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/static_assert.hpp>

// Project Includes
#include "core/include/Feature.h"
#include "core/include/Forward.h"

#ifdef RAM_WITH_MATH
#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"
#endif

namespace ram {
namespace core {

/** Base class for all property objects */
class Property
{
public:

    /// The type of a property
    enum PropertyType
    {
        PT_STARTVAL = 0,
        PT_INT,
        PT_DOUBLE,
        PT_BOOL,
//        PT_STRING,
        
#ifdef RAM_WITH_MATH
        PT_VECTOR2,
        PT_VECTOR3,
        PT_QUATERNION,
#endif // RAM_WITH_MATH
        
        PT_END
    };


    /* Construct a property.
        @param name The name of the property
        @param desc A (potentially) long description of the property
        @param pType The type of the property
        @param defaultValue The default value of the property
    */
    Property(const std::string& name, const std::string& desc, 
             PropertyType pType, boost::any defaultValue);

    /* Construct a property.
        @param name The name of the property
        @param desc A (potentially) long description of the property
        @param pType The type of the property
        @param defaultValue The default value of the property
	@param min The minimum value
	@param max The maximum value
    */
    Property(const std::string& name, const std::string& desc, 
             PropertyType pType, boost::any defaultValue, boost::any min,
	     boost::any max);
    
    virtual ~Property()
    {}

    /// Get the name of the property
    const std::string& getName() const { return m_name; }

    /// Get the description of the property
    const std::string& getDescription() const { return m_desc; }

    /// Get the type of the property
    PropertyType getType() const { return m_type; }

    /// Get the default value of this property
    boost::any getDefaultValue() const { return m_default; }
    
    /// True if the property has a min and max
    bool hasMinMax() const { return m_hasMinMax; }

    /// Get the minimum value of the property
    boost::any getMinValue() const { return m_min; }

    /// Get the maximum value of the property
    boost::any getMaxValue() const { return m_max; }

    /// Returns the value of the property as a string
    virtual std::string toString() const = 0;
    
    /**
     * @defgroup Get value as methods
     */
    /* @{ */
        
    /// Get as an integer
    virtual int getAsInt() const {
        int ret; 
        getPropertyImpl(ret, PT_INT);
        return ret;
    }
    
    /// Get as a double
    virtual double getAsDouble() const {
        double ret; 
        getPropertyImpl(ret, PT_DOUBLE);
        return ret;
    }
    
    /// Get a named property value as a boolean
    virtual bool getAsBool() const {
        bool ret; 
        getPropertyImpl(ret, PT_BOOL);
        return ret;
    }

#ifdef RAM_WITH_MATH    
    /// Get a named property value as a Vector2
    virtual math::Vector2 getAsVector2() const {
        math::Vector2 ret; 
        getPropertyImpl(ret, PT_VECTOR2);
        return ret;
    }
    
    /// Get a named property value as a Vector3
    virtual math::Vector3 getAsVector3() const {
        math::Vector3 ret; 
        getPropertyImpl(ret, PT_VECTOR3);
        return ret;
        }

    /// Get a named property value as a quaternion
    virtual math::Quaternion getAsQuat() const {
        math::Quaternion ret; 
        getPropertyImpl(ret, PT_QUATERNION);
        return ret;
    }
#endif // RAM_WITH_MATH
    
    /* }@ */

    /**
     * @defgroup Set value as methods
     */
    /* @{ */

    /// Set property as an integer
    virtual void set(int val) { setPropertyImpl(val, PT_INT); }

    /// Set property as a float
    virtual void set(double val) { setPropertyImpl(val, PT_DOUBLE); }
    
    /// Set property as a boolean
    virtual void set(bool val) { setPropertyImpl(val, PT_BOOL); }

#ifdef RAM_WITH_MATH    
    /// Set property as a Vector2
    virtual void set(const math::Vector2& val)
        { setPropertyImpl(val, PT_VECTOR2); }

    /// Set property as a Vector3
    virtual void set(const math::Vector3& val)
    { setPropertyImpl(val, PT_VECTOR3); }

    /// Set property as a quaternion
    virtual void set(const math::Quaternion& val)
    { setPropertyImpl(val, PT_QUATERNION); }
#endif // RAM_WITH_MATH    

    /* }@ */    

protected:
    /// Return the current value as an Any
    virtual boost::any getValue() const = 0;

    /// Set the current value as an Any
    virtual void setValue(const boost::any& value) = 0;

private:
    template <typename T>
    void setPropertyImpl(T val, PropertyType typeCheck)
    {
         if (getType() != typeCheck)
             throw std::exception();
         setValue(boost::any(val));
    }

   template <typename T>
   void getPropertyImpl(T& refVal, PropertyType typeCheck) const
   {
       if (getType() != typeCheck)
           throw std::exception();
       refVal = boost::any_cast<T>(getValue());
   }

    
    // no default construction
    Property() {}

    std::string m_name;
    std::string m_desc;
    PropertyType m_type;
    boost::any m_default;
    bool m_hasMinMax;
    boost::any m_min;
    boost::any m_max;
};

/** Helper template that turns type into PropertyType */
template <typename V>
Property::PropertyType getPropertyType()
{
    // Opps, you are using a desired type
    BOOST_STATIC_ASSERT(sizeof(V) == 0);
    return Property::PT_STARTVAL;
}

/** Template specialization for int -> PT_INT */
template <>
Property::PropertyType getPropertyType<int>();

/** Template specialization for int -> PT_DOUBLE */
template <>
Property::PropertyType getPropertyType<double>();

/** Template specialization for int -> PT_BOOL */    
template <>
Property::PropertyType getPropertyType<bool>();

#ifdef RAM_WITH_MATH
/** Template specialization for int -> PT_VECTOR2 */    
template <>
Property::PropertyType getPropertyType<math::Vector2>();

/** Template specialization for int -> PT_VECTOR3 */        
template <>
Property::PropertyType getPropertyType<math::Vector3>();

/** Template specialization for int -> PT_QUATERNION */            
template <>
Property::PropertyType getPropertyType<math::Quaternion>();
#endif // RAM_WITH_MATH

    
/** A property which bases it values on a pointer to a varialbe */
template <typename T>
class VariableProperty : public Property
{
public:
    typedef T ValueType;
    
    /** Construct a property which is able to directly call a given 
     *   getter and setter on a specific object instance, via functors.
     */
    VariableProperty(const std::string& name, const std::string& desc, 
                     boost::any defaultValue, T* valuePtr) :
        Property(name, desc, getPropertyType<T>(), defaultValue),
        m_valuePtr(valuePtr)
    {}

    /** Same as normal constructor except with min/max values*/
    VariableProperty(const std::string& name, const std::string& desc, 
                     boost::any defaultValue, T* valuePtr,
		     boost::any min, boost::any max) :
        Property(name, desc, getPropertyType<T>(), defaultValue, min, max),
        m_valuePtr(valuePtr)
    {}

    virtual std::string toString() const
    {
        std::stringstream ss;
        ss << *m_valuePtr;
        return ss.str();
    }

protected:    
    virtual void setValue(const boost::any& value)
    {
        *m_valuePtr = boost::any_cast<T>(value);
    }
    
    virtual boost::any getValue() const
    {
        return boost::any(*m_valuePtr);
    }
    
private:
    T* m_valuePtr;
};


/** A property which bases its value on getter and setter functions */
template <typename T>
class FunctionProperty : public Property
{
public:
    typedef T ValueType;
    typedef boost::function< T (void) > GetterFunc;
    typedef boost::function< void (T) > SetterFunc;

    /** Construct a property which is able to directly call a given 
        getter and setter on a specific object instance, via functors.
    */
    FunctionProperty(const std::string& name, const std::string& desc, 
                     boost::any defaultValue, GetterFunc getter,
                     SetterFunc setter) :
        Property(name, desc, getPropertyType<T>(), defaultValue), 
        m_getter(getter),
        m_setter(setter) 
        {}

    /** Same as normal constructor except with min and max values*/
    FunctionProperty(const std::string& name, const std::string& desc, 
                     boost::any defaultValue, GetterFunc getter,
                     SetterFunc setter, boost::any min, boost::any max) :
        Property(name, desc, getPropertyType<T>(), defaultValue, min, max), 
        m_getter(getter),
        m_setter(setter) 
        {}

    virtual std::string toString() const
    {
        std::stringstream ss;
        ss << m_getter();
        return ss.str();
    }
    
protected:    
    virtual void setValue(const boost::any& value)
    {
        m_setter(boost::any_cast<T>(value));
    }
    
    virtual boost::any getValue() const
    {
        return boost::any(m_getter());
    }
    
private:
    
    GetterFunc m_getter;
    SetterFunc m_setter;
};
   
} // namespace core
} // namespace ram

#endif // RAM_CORE_PROPERTY_H_04_09_2009
