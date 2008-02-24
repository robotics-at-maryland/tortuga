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

// Project Includes
#include "core/include/EventConverter.h"

namespace bp = boost::python;

namespace ram {
namespace core {

static DefaultEventConverter DEFAULT_EVENT_CONVERTER;

// DefaultEventConverter Methods
DefaultEventConverter::DefaultEventConverter()
{
    // Note this only works because we are going for exact matches now
    addEventConverter(typeid(ram::core::Event).name(), this);
}
    
DefaultEventConverter::~DefaultEventConverter()
{   
}
    
boost::python::object DefaultEventConverter::convert(ram::core::EventPtr event)
{
    return boost::python::object(event);
}


void EventConverter::addEventConverter(std::string typeName,
                                       EventConverter* converter)
{
    EventConverter::getEventConverterRegistry()->insert(
        std::make_pair(typeName, converter));
}
 	
    
EventConverterRegistry* EventConverter::getEventConverterRegistry()
{
    static EventConverterRegistry eventConverterRegistry;
    return &eventConverterRegistry;
}


EventTypeConverterMap* EventConverter::getEventTypeConverterMap()
{
    static EventTypeConverterMap eventConverterMap;
    return &eventConverterMap;
}

boost::python::object EventConverter::convertEvent(ram::core::EventPtr event)
{
    // Return None when given a null pointer
    if (!event)
        return boost::python::object();
    
    // If we already have a python object, use default converter
    if (0 != boost::get_deleter<bp::converter::shared_ptr_deleter>(event))
        return DEFAULT_EVENT_CONVERTER.convert(event);

    // Attempt to find the converter based on event type
    EventTypeConverterMap* eventConverterMap = getEventTypeConverterMap();
    EventConverter* converter = 0;
    EventTypeConverterMap::iterator iter = eventConverterMap->find(event->type);

    if (eventConverterMap->end() != iter)
    {
        converter = iter->second;
    }
    else
    {
        // Did not find the converter so look it up by type in our registry
        EventConverterRegistry* registry =
            EventConverter::getEventConverterRegistry();

        std::string eventTypeName(typeid(*(event.get())).name());
        EventConverterRegistry::iterator registryIter =
            registry->find(eventTypeName);

        // We *must* find a converter, or things will break
        assert(registry->end() != registryIter &&
               "Could not find converter for event type in registry");

        converter = registryIter->second;
        
        // Record converter it in our map for faster lookup
        eventConverterMap->insert(std::make_pair(event->type, converter));
    }

    
    return converter->convert(event);
}

} // namespace core
} // namespace ram
