/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/include/EventFunctor.h
 */

#include "include/EventFunctor.h"

EventFunctor::EventFunctor(boost::python::object pyObject) :
    pyFunction(pyObject)
{
}
    
void EventFunctor::operator()(ram::core::EventPtr event)
{
    pyFunction(event);
}
