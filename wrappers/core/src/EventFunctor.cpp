/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/include/EventFunctor.h
 */

// Project Includes
#include "include/EventFunctor.h"

EventFunctor::EventFunctor(boost::python::object pyObject) :
    pyFunction(pyObject)
{
    // Check to make sure its a callable object
    if (0 == PyObject_HasAttrString(pyObject.ptr(), "__call__"))
    {
        PyErr_SetString(PyExc_TypeError, "Handler must be a callable object");
        boost::python::throw_error_already_set();
    }
}
    
void EventFunctor::operator()(ram::core::EventPtr event)
{
    pyFunction(event);
}
