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
#include "core/include/EventConverter.h"
#include "wrappers/core/include/EventFunctor.h"

namespace bp = boost::python;

EventFunctor::EventFunctor(bp::object pyObject) :
    pyFunction(pyObject)
{
    bool bad = false;
    
    // Check to make sure its a callable object
    if (0 == PyObject_HasAttrString(pyObject.ptr(), "__call__"))
    {
        // If not, lets throw an exception to warn the user
        PyErr_SetString(PyExc_TypeError, "Handler must be a callable object");
        bad = true;
    }
    else
    {
        int expectedArgs = 1;
        
        // Handle function objects vs. functions
        //  * Free python functions have a 'func_code' attributes
        //  * For python function objects, their __call__ method has func_code
        bp::object func_code;
        
        if (0 == PyObject_HasAttrString(pyObject.ptr(), "func_code"))
        {
            // Function object, need to increment count for 'self'
            expectedArgs += 1; 
            func_code = pyObject.attr("__call__").attr("func_code");
        }
        else
        {
            func_code = pyObject.attr("func_code");

            // Handle instancemethod type callable
            if (0 != PyObject_HasAttrString(pyObject.ptr(), "im_self"))
                expectedArgs += 1;
        }

        int argCount = bp::extract<int>(func_code.attr("co_argcount"));
        // 0x4 in the flags means we have a function signature with *args 
        int flags = bp::extract<int>(func_code.attr("co_flags"));

        // To many arguments
        if (argCount > expectedArgs)
        {
            PyErr_SetString(PyExc_TypeError,
                            "Handler has to many args, expected 1");
            bad = true;
        }
        // To few arguments and no "*args" in signature
        else if ((argCount < expectedArgs) && !(flags & 4))
        {
            PyErr_SetString(PyExc_TypeError,
                            "Handler has too few args, expected 1");
            bad = true;
        }
    }

    if (bad)
        bp::throw_error_already_set();
}
    
void EventFunctor::operator()(ram::core::EventPtr event)
{
    pyFunction(ram::core::EventConverter::convertEvent(event));
}
