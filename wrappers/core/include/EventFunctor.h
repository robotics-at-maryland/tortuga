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

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Event.h"

/** Wraps a called python object, so it can handle events
 *
 *  Then given python object is checked to make sure its calledable and has
 *  the proper number of arguments on functor creation.
 */
class EventFunctor
{
public:
    EventFunctor(boost::python::object pyObject);
    
    void operator()(ram::core::EventPtr event);

    /** The callable python object which is called to handle the event */
    boost::python::object pyFunction;
};

#endif // RAM_CORE_WRAP_EVENTFUNCTOR_H_10_30_2007
