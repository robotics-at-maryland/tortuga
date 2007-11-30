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

struct EventFunctor {
    EventFunctor(boost::python::object pyObject);
    
    void operator()(ram::core::EventPtr event);
    
    boost::python::object pyFunction;
};

#endif // RAM_CORE_WRAP_EVENTFUNCTOR_H_10_30_2007
