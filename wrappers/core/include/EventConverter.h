/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/include/EventConverter.h
 */

#ifndef RAM_CORE_WRAP_EVENTCONVERTER_H_01_08_2008
#define RAM_CORE_WRAP_EVENTCONVERTER_H_01_08_2008

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "wrappers/core/include/EventFunctor.h"

class EventConverter
{
public:
    virtual ~EventConverter() {};
    
    virtual boost::python::object convert(ram::core::EventPtr event) = 0;
};

class DefaultEventConverter : public EventConverter
{
public:
    virtual ~DefaultEventConverter();
    
    virtual boost::python::object convert(ram::core::EventPtr event);
};

template<class T>
class SpecificEventConverter : public EventConverter
{
public:
    SpecificEventConverter()
    {
        EventFunctor::addEventConverter(this);
    }
    
    virtual boost::python::object convert(ram::core::EventPtr event)
    {
        boost::shared_ptr<T> result = boost::dynamic_pointer_cast<T>(event);

        // If the conversion was succesfull return the proper python object
        if (result)
            return boost::python::object(result);

        // If not, return None
        return boost::python::object();
    }

//    static registerThis;
};

//#define RAM_CORE_REGISTER_EVENT_CONVERTER(TYPE)  
//static SpecificEventConverter<TYPE>::registerThis DEFAULT_CONVERTER;

#endif // RAM_CORE_WRAP_EVENTCONVERTER_H_01_08_2008
