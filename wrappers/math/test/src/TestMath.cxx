/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  wrappers/math/test/src/TestMath.cxx
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

struct MathFixture
{
    MathFixture() :
        main_module(py::import("__main__")),
        main_namespace(main_module.attr("__dict__")),
        eval(boost::bind(py::exec, _1, main_namespace, main_namespace))
    {
        main_namespace["math"] = py::import("ext.math");
    }

    py::object main_module;
    py::object main_namespace;
    boost::function<py::object (py::str)> eval;
};

TEST(MathImport)
{
    py::import("ext.math");
}

TEST_FIXTURE(MathFixture, Radian)
{
    try {
        eval("d = math.Degree(45)\n"
             "deg = d.valueDegrees()\n"
             "rad    = d.valueRadians()\n");
        
        double value = py::extract<double>(main_namespace["deg"]);
        CHECK_EQUAL(45, value);
        value = py::extract<double>(main_namespace["rad"]);
        CHECK_CLOSE(M_PI/4, value, 0.1);
        
        
        eval("import math as m\n"
             "r = math.Radian(m.pi)\n"
             "deg = r.valueDegrees()\n"
             "rad = r.valueRadians()\n");
    
        value = py::extract<double>(main_namespace["deg"]);
        CHECK_CLOSE(180, value, 0.1);
        value = py::extract<double>(main_namespace["rad"]);
        CHECK_CLOSE(M_PI, value, 0.1);


    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

TEST_FIXTURE(MathFixture, Matrix3)
{
    try  {
        eval("M = math.Matrix3(1,2,3,4,5,6,7,8,9)\n"
             "elem21 = M[2][1]");
        double value = py::extract<double>(main_namespace["elem21"]);
        CHECK_EQUAL(8, value);
        
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

TEST_FIXTURE(MathFixture, __str__)
{
    try {
        eval("vset = str(math.Vector3(1,2,3))");

        std::string str = py::extract<std::string>(main_namespace["vset"]);
        CHECK_EQUAL("Vector3(1, 2, 3)", str);
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

int main()
{
    // We must register this contverted so we can pass a pointer to python
//    py::register_ptr_to_python< boost::shared_ptr<ram::control::IController> >();
    Py_Initialize();
    return UnitTest::RunAllTests();
}
