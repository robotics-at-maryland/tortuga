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
        bp::object func_defaults;
        
        if (0 == PyObject_HasAttrString(pyObject.ptr(), "func_code"))
        {
            // Function object, need to increment count for 'self'
            expectedArgs += 1; 
            func_code = pyObject.attr("__call__").attr("func_code");
            func_defaults = pyObject.attr("__call__").attr("func_defaults");
        }
        else
        {
            func_code = pyObject.attr("func_code");
            func_defaults = pyObject.attr("func_defaults");

            // Handle instancemethod type callable
            if (0 != PyObject_HasAttrString(pyObject.ptr(), "im_self"))
                expectedArgs += 1;
        }

        // Determine the number of default arguments
        int defaultArgCount = 0;
        if (Py_None != func_defaults.ptr())
            defaultArgCount = bp::len(func_defaults);
        
        int maxArgCount = bp::extract<int>(func_code.attr("co_argcount"));
        // 0x4 in the flags means we have a function signature with *args 
        int flags = bp::extract<int>(func_code.attr("co_flags"));

        // Argument count with default arguments considered
        int minArgCount = maxArgCount - defaultArgCount;
        
        // To many arguments
        if (minArgCount > expectedArgs)
        {
            PyErr_SetString(PyExc_TypeError,
                            "Handler has to many args, expected 1");
            bad = true;
        }

        // Note: 0x08 if flags means **kwargs in signature
        
        // To few arguments and no "*args" in signature
        else if ((maxArgCount < expectedArgs) && !(flags & 4))
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
