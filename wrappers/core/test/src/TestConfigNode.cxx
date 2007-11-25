/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  wrapper/core/test/src/TestConfigNode.cxx
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
#include "core/include/ConfigNode.h"

namespace py = boost::python;

struct DeviceFixture
{
    DeviceFixture() :
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

TEST(DeviceImport)
{
    py::import("ext.core");
}

TEST_FIXTURE(DeviceFixture, Vehicle)
{
    try {
        eval("raw_cfg = {'Bob' : [1, 2, 3], 'A' : 20}\n"
             "cfg = core.ConfigNode.fromString(str(raw_cfg))");

        ram::core::ConfigNode node =
            py::extract<ram::core::ConfigNode>(main_namespace["cfg"]);

        CHECK_EQUAL(2, node["Bob"][1].asInt());
        CHECK_EQUAL(20, node["A"].asInt());
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

int main()
{
    Py_Initialize();
    return UnitTest::RunAllTests();
}
