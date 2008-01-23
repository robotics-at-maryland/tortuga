/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  wrapper/core/test/src/TestEventFunctor.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

// Project Includes
#include "core/include/Event.h"
#include "core/include/EventPublisher.h"
#include "core/include/EventConverter.h"

namespace py = boost::python;

// Test Event Types
struct RegisteredEvent : public ram::core::Event
{
    int value;
};

struct UnRegisteredEvent : public ram::core::Event
{
    int number;
};
    
static ram::core::SpecificEventConverter<RegisteredEvent>
registerRegisteredEvent;

struct EventFunctorFixture
{
    EventFunctorFixture() :
        main_module(py::import("__main__")),
        main_namespace(main_module.attr("__dict__")),
        eval(boost::bind(py::exec, _1, main_namespace, main_namespace))
    {
        main_namespace["core"] = py::import("ext.core");
    }

    py::object main_module;
    py::object main_namespace;
    boost::function<py::object (py::str)> eval;
};


BOOST_PYTHON_MODULE(event_functor)
{
    py::class_<RegisteredEvent, py::bases<ram::core::Event> >(
        "RegisteredEvent")
        .def_readwrite("value", &RegisteredEvent::value);
    py::register_ptr_to_python<boost::shared_ptr<RegisteredEvent> >();
    
/*    py::class_<UnRegisteredEvent, py::bases<ram::core::Event> >(
        "UnRegisteredEvent")
        .def_readwrite("number", &UnRegisteredEvent::number);
        py::register_ptr_to_python<boost::shared_ptr<UnRegisteredEvent> >();*/
}

TEST_FIXTURE(EventFunctorFixture, test)
{
    try {
        // Setup event handler, and publicher
        eval("import event_functor\n"
             "event = None\n"
             "def handler(e):\n"
             "    global event\n"
             "    event = e\n"
             "epub = core.EventPublisher()\n"
             "epub.subscribe('TEST_REG', handler)\n"
             "epub.subscribe('TEST_UNREG', handler)");
        ram::core::EventPublisher* eventPublisher =
            py::extract<ram::core::EventPublisher*>(main_namespace["epub"]);
        
        // Publish an event with a registered converter
        RegisteredEvent* eventReg = new RegisteredEvent();
        eventReg->value = 10;
        
        eventPublisher->publish("TEST_REG",
            ram::core::EventPtr(((ram::core::Event*)eventReg)));
        eval("value = event.value");
        
        int value = py::extract<int>(main_namespace["value"]);
        CHECK_EQUAL(10, value);
        

        // Check the one with the unregistered converter
/*        UnRegisteredEvent* eventUnReg = new UnRegisteredEvent();
        eventUnReg->number = 5;
        
        eventPublisher->publish("TEST_UNREG",
            ram::core::EventPtr(((ram::core::Event*)eventUnReg)));
        eval("attr = hasattr(event, 'number')\n"
             "number = event.number");

        // Converter not needed for some strange reason
        bool hasattr = py::extract<bool>(main_namespace["attr"]);
        CHECK_EQUAL(true, hasattr);

        int number = py::extract<int>(main_namespace["number"]);
        CHECK_EQUAL(5, number);*/

        // They both seem to work, I have no idea why!
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}
