/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/src/EventHub.cpp
 */

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/EventHub.h"
#include "core/include/EventPublisher.h"
#include "core/include/EventConnection.h"
#include "wrappers/core/include/EventFunctor.h"

namespace bp = boost::python;

// Wrapper functions allow use of python function objects
ram::core::EventConnectionPtr subscribe(ram::core::EventHub& epub,
                                        std::string type,
                                        ram::core::EventPublisher* publisher,
                                        boost::python::object pyFunction)
{
    return epub.subscribe(type, publisher, EventFunctor(pyFunction));
}

ram::core::EventConnectionPtr subscribeToType(ram::core::EventHub& epub,
                                              std::string type,
                                              boost::python::object pyFunction)
{
    return epub.subscribeToType(type, EventFunctor(pyFunction));
}

ram::core::EventConnectionPtr subscribeToAll(ram::core::EventHub& epub,
                                             boost::python::object pyFunction)
{
    return epub.subscribeToAll(EventFunctor(pyFunction));
}


void registerEventHubClass()
{
    bp::class_<ram::core::EventHub,
        bp::bases<ram::core::Subsystem> >("EventHub")
        .def(bp::init<ram::core::ConfigNode,
             bp::optional<ram::core::SubsystemList> >())
        .def("subscribe", &subscribe,
             (bp::arg("type"), bp::arg("publisher"), bp::arg("handler")))    
        .def("subscribeToType", &subscribeToType,
             (bp::arg("type"), bp::arg("handler")))
        .def("subscribeToAll", &::subscribeToAll, (bp::arg("handler")));

    bp::register_ptr_to_python<ram::core::EventConnectionPtr>();
}
