/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  wrapper/samples/test/src/TestCodeInject.cxx
 */

// STD Includes
#include <cmath>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "vehicle/include/IVehicle.h"

namespace py = boost::python;

struct SampleFixture
{
    SampleFixture() :
        main_module(py::import("__main__")),
        main_namespace(main_module.attr("__dict__")),
        eval(boost::bind(py::exec, _1, main_namespace, main_namespace))
    {
        main_namespace["samples"] = py::import("ext.samples");
    }

    py::object main_module;
    py::object main_namespace;
    boost::function<py::object (py::str)> eval;
};

TEST(SampleImport)
{
    py::import("ext.samples");
}

TEST_FIXTURE(SampleFixture, Vehicle)
{
    try {
        eval("codeI = samples.CodeInject(10)\n"
             "value = codeI.injectedFunc()\n"
             "value2 = codeI.otherInjectedFunc(13)\n"
             "value3 = codeI.getValue()");

        int value = py::extract<int>(main_namespace["value"]);
        
        // Make sure that we can read data properly
        CHECK_EQUAL(10, value);

        value = py::extract<int>(main_namespace["value2"]);
        CHECK_EQUAL(10, value);
        value = py::extract<int>(main_namespace["value3"]);
        CHECK_EQUAL(13, value);
        
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

int main()
{
    Py_Initialize();
    return UnitTest::RunAllTests();
}
