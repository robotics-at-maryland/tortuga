/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  wrapper/vehicle/test/src/TestIVehicle.cxx
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

struct VehicleFixture
{
    VehicleFixture() :
        main_module(py::import("__main__")),
        main_namespace(main_module.attr("__dict__")),
        eval(boost::bind(py::exec, _1, main_namespace, main_namespace))
    {
        main_namespace["vehicle"] = py::import("ext.vehicle");
    }

    py::object main_module;
    py::object main_namespace;
    boost::function<py::object (py::str)> eval;
};

TEST(VehicleImport)
{
    py::import("ext.vehicle");
}

TEST_FIXTURE(VehicleFixture, Vehicle)
{
    try {
        main_namespace["math"] = py::import("ext.math");

        // A Mock Vehicle in Python, ensures that Python can properly subclass
        // and stand in for a "real" vehicle
        eval("class Vehicle(vehicle.IVehicle):\n"
             "     def __init__(self):\n"
             "          vehicle.IVehicle.__init__(self, 'Test')\n"
             "          self.force = math.Vector3.ZERO\n"
             "          self.torque = math.Vector3.ZERO\n"
             "     def applyForcesAndTorques(self, force, torque):\n"
             "          self.force = force; self.torque = torque\n"
             "veh = Vehicle()");
        
        ram::vehicle::IVehicle* vehicle =
            py::extract<ram::vehicle::IVehicle*>(main_namespace["veh"]);

        // Test applying force
        ram::math::Vector3 force(15, 1237, 12);
        ram::math::Vector3 torque(12, 428, 151);
        vehicle->applyForcesAndTorques(force, torque);

        eval("force = veh.force; torque = veh.torque");
        ram::math::Vector3 result =
            py::extract<ram::math::Vector3>(main_namespace["force"]);
        CHECK_EQUAL(force, result);
        result = py::extract<ram::math::Vector3>(main_namespace["torque"]);
        CHECK_EQUAL(torque, result);

        // Make sure we qualify as a "Subsystem"
        ram::core::Subsystem* subsystem =
            py::extract<ram::core::Subsystem*>(main_namespace["veh"]);
        CHECK_EQUAL(vehicle, subsystem);

        main_namespace["core"] = py::import("ext.core");
        eval("dependents = core.SubsystemList()\n"
             "dependents.append(veh)\n"
             "name = dependents[0].getName()\n"
             "print dependents[0]");

        std::string name = py::extract<std::string>(main_namespace["name"]);
        CHECK_EQUAL("Test", name);
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

int main()
{
    Py_Initialize();
    return UnitTest::RunAllTests();
}
