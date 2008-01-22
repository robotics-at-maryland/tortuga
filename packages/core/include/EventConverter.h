/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/core/include/EventConverter.h
 */

#ifndef RAM_CORE_WRAP_EVENTCONVERTER_H_01_08_2008
#define RAM_CORE_WRAP_EVENTCONVERTER_H_01_08_2008

// STD Includes
#include <string>
#include <set>
#include <map>
#include <cstring>

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Event.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

class EventConverter;
typedef std::set<EventConverter*> EventConverterSet;
typedef std::map<ram::core::Event::EventType, EventConverter*>
    EventTypeConverterMap;

class RAM_EXPORT EventConverter
{
public:
    virtual ~EventConverter() {};

    static boost::python::object convertEvent(ram::core::EventPtr event);
    
protected:
    virtual boost::python::object convert(ram::core::EventPtr event) = 0;

    /** Add an event converter to the global list of converters */
    static void addEventConverter(EventConverter* converter);

private:    
    /** Gets the global set of event converters */
    static EventConverterSet* getEventConverterSet();

    /** Gets the global map which maps an event type to the right converter */
    static EventTypeConverterMap* getEventTypeConverterMap();


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
        EventConverter::addEventConverter(this);

        // Register these types here. This is a slight hack, but its really
        // the simplest place to put these
        boost::python::register_ptr_to_python<boost::shared_ptr<T> >();
        boost::python::implicitly_convertible<boost::shared_ptr<T>,
            ram::core::EventPtr >();
    }

protected:    
    virtual boost::python::object convert(ram::core::EventPtr event)
    {
        boost::shared_ptr<T> result;
        
        if ( strcmp(typeid(T).name(), typeid(*event.get()).name()) == 0)
            result = boost::static_pointer_cast<T>(event);
        
        // If the conversion was succesfull return the proper python object
        if (result)
            return boost::python::object(result);

        // If not, return None
        return boost::python::object();
    }
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_WRAP_EVENTCONVERTER_H_01_08_2008
