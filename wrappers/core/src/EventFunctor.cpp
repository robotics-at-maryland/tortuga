/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/src/EventFunctor.cpp
 */

// STD Includes
#include <utility>

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "wrappers/core/include/EventFunctor.h"
#include "wrappers/core/include/EventConverter.h"

static DefaultEventConverter DEFAULT_EVENT_CONVERTER;

EventFunctor::EventFunctor(boost::python::object pyObject) :
    pyFunction(pyObject)
{
    // Check to make sure its a callable object
    if (0 == PyObject_HasAttrString(pyObject.ptr(), "__call__"))
    {
        // If not, lets throw an exception to warn the user
        PyErr_SetString(PyExc_TypeError, "Handler must be a callable object");
        boost::python::throw_error_already_set();
    }
}
    
void EventFunctor::operator()(ram::core::EventPtr event)
{
    pyFunction(EventFunctor::convertEvent(event));
}

void EventFunctor::addEventConverter(EventConverter* converter)
{
    EventFunctor::getEventConverterSet()->insert(converter);
}


EventConverterSet* EventFunctor::getEventConverterSet()
{
    static EventConverterSet eventConverterSet;
    return &eventConverterSet;
}


EventTypeConverterMap* EventFunctor::getEventTypeConverterMap()
{
    static EventTypeConverterMap eventConverterMap;
    return &eventConverterMap;
}

boost::python::object EventFunctor::convertEvent(ram::core::EventPtr event)
{
    EventTypeConverterMap* eventConverterMap = getEventTypeConverterMap();
    
    // Find the proper converter, first look it up by event type
    EventConverter* converter = 0;
    EventTypeConverterMap::iterator iter = eventConverterMap->find(event->type);

    if (eventConverterMap->end() != iter)
    {
        converter = iter->second;
    }
    else
    {
        // Did not find the converter so look it up in our set    
        EventConverterSet* eventConverterSet =
            EventFunctor::getEventConverterSet();
        
        BOOST_FOREACH(EventConverter* conv, (*eventConverterSet))
        {
            // If we don't get None back, the converter succeeded
            if(Py_None != conv->convert(event).ptr())
            {
                converter = conv;
                break;
            }
        }

        // If no converter was found, just use the default
        if (!converter)
            converter = &DEFAULT_EVENT_CONVERTER;
        
        // Record converter it in our map for faster lookup
        eventConverterMap->insert(std::make_pair(event->type, converter));
    }

    
    return converter->convert(event);
}

// DefaultEventConverter Methods

DefaultEventConverter::~DefaultEventConverter()
{   
}
    
boost::python::object DefaultEventConverter::convert(ram::core::EventPtr event)
{
    return boost::python::object(event);
}
