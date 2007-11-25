/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/math/test/src/TestChecks.h
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Library Includes
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>

// Project Includes
#include "control/include/IController.h"

class MockController : public ram::control::IController
{
public:
	MockController(std::string name) : IController(name) {}
	
    virtual void setSpeed(int speed_) { speed = speed_; }

    virtual void setDepth(double depth_) { depth = depth_; }

    virtual int getSpeed() { return speed; }

    virtual double getDepth() { return depth; }

    virtual void yawVehicle(double degrees) { yaw = degrees; }

    virtual bool isOriented() { return oriented; }
    
    virtual bool atDepth() { return atdepth; }


    virtual void update(double) {};
    virtual void background(int) {};
    virtual void unbackground(bool) {};
    virtual bool backgrounded() { return false; }
    
    int speed;
    double depth;
    double yaw;
    bool oriented;
    bool atdepth;
};

TEST(MockController)
{
    MockController* mockController = new MockController("Controller");
    ram::control::IController* controller = mockController;

    controller->setSpeed(5);
    CHECK_EQUAL(5, mockController->speed);
}


namespace py = boost::python;

TEST(Emedding)
{
    try {
    py::object main_module = py::import("__main__");
    py::object main_namespace = main_module.attr("__dict__");
    py::object ignored = py::exec("result = 5 ** 2", main_namespace,
                                  main_namespace);
    int five_squared = py::extract<int>(main_namespace["result"]);

    CHECK_EQUAL(25, five_squared);
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

TEST(ControlImport)
{
    try {
    py::import("ext.core");
    py::import("ext.control");
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

TEST(ControllerWrapping)
{
    try {
    
    // Create MockController and wrap it in smart point to allow python to
    // handle it
    MockController* mockController = new MockController("Controller");
    boost::shared_ptr<ram::control::IController> controllerPtr(mockController);

    // Create out namespace for the python code to operate in
    py::object main_module = py::import("__main__");
    py::object main_namespace = main_module.attr("__dict__");

    // Import our module to test and inject the controller object
    main_namespace["core"] = py::import("ext.core");
    main_namespace["control"] = py::import("ext.control");
    main_namespace["controller"] = controllerPtr;

    // Create simple eval function
    boost::function<py::object (py::str)> eval =
        boost::bind(py::exec, _1, main_namespace, main_namespace);

    // Test speed (if this works everything else should to)
    eval("controller.setSpeed(3)");
    CHECK_EQUAL(3, mockController->speed);
    
    eval("speed = controller.getSpeed()");
    int speed = py::extract<int>(main_namespace["speed"]);
    CHECK_EQUAL(3, speed);
    
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

int main()
{
    // We must register this contverted so we can pass a pointer to python
    py::register_ptr_to_python< boost::shared_ptr<ram::control::IController> >();
    Py_Initialize();
    return UnitTest::RunAllTests();
}
