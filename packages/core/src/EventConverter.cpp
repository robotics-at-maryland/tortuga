/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/core/src/EventFunctor.cpp
 */

// STD Includes
#include <utility>

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "core/include/EventConverter.h"

namespace ram {
namespace core {

static DefaultEventConverter DEFAULT_EVENT_CONVERTER;

// DefaultEventConverter Methods
DefaultEventConverter::~DefaultEventConverter()
{   
}
    
boost::python::object DefaultEventConverter::convert(ram::core::EventPtr event)
{
    return boost::python::object(event);
}


void EventConverter::addEventConverter(EventConverter* converter)
{
    EventConverter::getEventConverterSet()->insert(converter);
}
 	
    
EventConverterSet* EventConverter::getEventConverterSet()
{
    static EventConverterSet eventConverterSet;
    return &eventConverterSet;
}


EventTypeConverterMap* EventConverter::getEventTypeConverterMap()
{
    static EventTypeConverterMap eventConverterMap;
    return &eventConverterMap;
}

boost::python::object EventConverter::convertEvent(ram::core::EventPtr event)
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
            EventConverter::getEventConverterSet();
        
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

} // namespace core
} // namespace ram
