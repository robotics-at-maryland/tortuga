/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/include/EventFunctor.h
 */

#ifndef RAM_CORE_WRAP_EVENTFUNCTOR_H_10_30_2007
#define RAM_CORE_WRAP_EVENTFUNCTOR_H_10_30_2007

// STD Includes
#include <string>
#include <set>
#include <map>

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Event.h"

class EventConverter;
typedef std::set<EventConverter*> EventConverterSet;
typedef std::map<ram::core::Event::EventType, EventConverter*>
    EventTypeConverterMap;

/** Wraps a called python object, so it can handle events
 *
 *  Then given python object is checked to make sure its calledable before use.
 */
class EventFunctor
{
public:
    EventFunctor(boost::python::object pyObject);
    
    void operator()(ram::core::EventPtr event);

    /** Add an event converter to the global list of converters */
    static void addEventConverter(EventConverter* converter);

    /** The callable python object which is called to handle the event */
    boost::python::object pyFunction;
private:
    /** Gets the global set of event converters */
    static EventConverterSet* getEventConverterSet();

    /** Gets the global map which maps an event type to the right converter */
    static EventTypeConverterMap* getEventTypeConverterMap();

    static boost::python::object convertEvent(ram::core::EventPtr event);
};



#endif // RAM_CORE_WRAP_EVENTFUNCTOR_H_10_30_2007
