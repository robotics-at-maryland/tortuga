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
#include <boost/assign/list_of.hpp>
#include <boost/filesystem/path.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "logging/include/EventLogger.h"
#include "logging/include/EventPlayer.h"

namespace py = boost::python;
using namespace ram;

static boost::filesystem::path getRefDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "logging" / "test" / "data" / "ref.log";
}

TEST(LoggingImport)
{
    try {
        py::import("ext.logging");
    } catch (py::error_already_set err) {
        PyErr_Print();
        throw err;
    }
}

static std::string PLAYER_CFG =
    "{ 'fileName' : '" + getRefDir().string() + "' }";

struct EventPlayerFixture
{
    EventPlayerFixture()
        : eventHub(new core::EventHub())
        , player(new logging::EventPlayer(
                     core::ConfigNode::fromString(PLAYER_CFG),
                     boost::assign::list_of(eventHub)))
        , main_module(py::import("__main__"))
        , main_namespace(main_module.attr("__dict__"))
        , eval(boost::bind(py::exec, _1, main_namespace, main_namespace))
    {
        // Import our module and inject the player object
        main_namespace["logging"] = py::import("ext.logging");
        main_namespace["player"] = player;
    }

    core::EventHubPtr eventHub;
    logging::EventPlayerPtr player;

    py::object main_module;
    py::object main_namespace;
    boost::function<py::object (py::str)> eval;
};

TEST_FIXTURE(EventPlayerFixture, EventPlayer)
{
    // Check the duration of the player
    eval("duration = player.duration()");
    double duration = py::extract<double>(main_namespace["duration"]);
    CHECK_EQUAL(0, duration);
}

int main()
{
    Py_Initialize();
    // We must register this converter so we can pass a pointer to python
    py::register_ptr_to_python< logging::EventPlayerPtr >();
    return UnitTest::RunAllTests();
}
