/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  wrapper/network/test/src/TestNetworkWrapper.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/python.hpp>

namespace py = boost::python;
//using namespace ram;

TEST(NetworkImport)
{
    try {
        py::import("ext.network");
    } catch (py::error_already_set err) {
        PyErr_Print();
        throw err;
    }
}

int main(int argc, char* argv[])
{
    Py_Initialize();
    return UnitTest::RunAllTests();
}
