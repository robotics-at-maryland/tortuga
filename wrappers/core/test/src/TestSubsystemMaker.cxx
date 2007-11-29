/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  wrapper/core/test/src/TestSubsystemMaker.cxx
 */

#include <iostream>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

// Project Includes
#include "core/include/SubsystemMaker.h"
#include "core/test/include/MockSubsystem.h"

namespace py = boost::python;

// arguments are (type name, registered name)
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(MockSubsystem, MockSubsystem);

struct SubsystemMakerFixture
{
    SubsystemMakerFixture() :
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

TEST(SubsystemMaker)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'name' : 'Mock', 'type' : 'MockSubsystem' }"));
    ram::core::SubsystemList deps;
    deps.insert(ram::core::SubsystemPtr());
    
    ram::core::SubsystemPtr subsystem = //ram::core::makeMock();
        ram::core::SubsystemMaker::newObject(std::make_pair(config, deps));
    MockSubsystem* mockSubsystem =
        dynamic_cast<MockSubsystem*>(subsystem.get());
    
    CHECK(mockSubsystem);
    CHECK_EQUAL("Mock", subsystem->getName());
    CHECK_EQUAL(1u, mockSubsystem->dependents.size());
}

TEST_FIXTURE(SubsystemMakerFixture, newObject)
{
    try {
    eval("cfg = { 'name' : 'Mock', 'type' : 'MockSubsystem' }\n"
         "subsystem = core.SubsystemMaker.newObject(cfg, [])");

    ram::core::SubsystemPtr subsystem =
        py::extract<ram::core::SubsystemPtr>(main_namespace["subsystem"]);
    MockSubsystem* mockSubsystem =
        dynamic_cast<MockSubsystem*>(subsystem.get());
    
    CHECK(mockSubsystem);
    CHECK_EQUAL("Mock", subsystem->getName());
    CHECK_EQUAL(0u, mockSubsystem->dependents.size());

   } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}
