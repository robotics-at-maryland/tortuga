/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/test/src/TestChecks.h
 */

// STD Includes
#include <cmath>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

namespace py = boost::python;


TEST(MathImport)
{
    py::import("ext.math");
}

TEST(Radian)
{
    try {
        // Create out namespace for the python code to operate in
        py::object main_module = py::import("__main__");
        py::object main_namespace = main_module.attr("__dict__");
        main_namespace["math"] = py::import("ext.math");
        
        // Create simple eval function
        boost::function<py::object (py::str)> eval =
            boost::bind(py::exec, _1, main_namespace, main_namespace);

    
        eval("d = math.Degree(45);\n"
             "deg = d.valueDegrees();\n"
             "rad    = d.valueRadians();\n");
        
        double value = py::extract<double>(main_namespace["deg"]);
        CHECK_EQUAL(45, value);
        value = py::extract<double>(main_namespace["rad"]);
        CHECK_CLOSE(M_PI/4, value, 0.1);
        
        
        eval("import math as m;\n"
             "r = math.Radian(m.pi);\n"
             "deg = r.valueDegrees();\n"
             "rad = r.valueRadians();\n");

    
        value = py::extract<double>(main_namespace["deg"]);
        CHECK_CLOSE(180, value, 0.1);
        value = py::extract<double>(main_namespace["rad"]);
        CHECK_CLOSE(M_PI, value, 0.1);
    } catch(py::error_already_set err) {
        PyErr_Print();
        throw err;
    }
}

int main()
{
    // We must register this contverted so we can pass a pointer to python
//    py::register_ptr_to_python< boost::shared_ptr<ram::control::IController> >();
    Py_Initialize();
    return UnitTest::RunAllTests();
}
