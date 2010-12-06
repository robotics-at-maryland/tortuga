/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  wrapper/logging/test/src/TestLogging.cxx
 */

#ifdef RAM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif

// STD Includes
#include <cstdlib>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Library Includes
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/python.hpp>
#include <boost/filesystem/path.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "logging/include/EventLogger.h"
#include "logging/include/EventPlayer.h"

namespace py = boost::python;
using ram::logging::EventPlayer;

TEST(LoggingImport)
{
    try {
        py::import("ext.logging");
    } catch (py::error_already_set err) {
        PyErr_Print();
        throw err;
    }
}

int main()
{
    Py_Initialize();
    // We must register this converter so we can pass a pointer to python
    py::register_ptr_to_python< boost::shared_ptr<EventPlayer> >();
    return UnitTest::RunAllTests();
}
