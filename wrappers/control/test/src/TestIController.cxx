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
#include "math/include/Vector3.h"

class MockController : public ram::control::IController
{
public:
    MockController(std::string name) : IController(name) {}

    virtual void changeDepth(double depth,
                             double rate = 0,
                             double accel = 0)
    { 
        m_depth = depth;
        m_depthRate = rate;
        m_depthAccel = accel;
    }

    virtual void translate(ram::math::Vector2 position,
                           ram::math::Vector2 velocity = ram::math::Vector2::ZERO,
                           ram::math::Vector2 accel = ram::math::Vector2::ZERO)
    {
        m_position = position;
        m_velocity = velocity;
        m_accel = accel;
    }

    virtual void rotate(ram::math::Quaternion orientation,
                        ram::math::Vector3 angularRate = ram::math::Vector3::ZERO,
                        ram::math::Vector3 angularAccel = ram::math::Vector3::ZERO)
    {
        m_orientation = orientation;
        m_angularRate = angularRate;
        m_angularAccel = angularAccel;
    }

    virtual void yawVehicle(double degrees, double rate) { yaw = degrees; }
    virtual void pitchVehicle(double degrees, double rate) {  }
    virtual void rollVehicle(double degrees, double rate) { }

    virtual double getDesiredDepth() {return m_depth;}
    virtual double getDesiredDepthRate() {return m_depthRate;}
    virtual double getDesiredDepthAccel() {return m_depthAccel;}

    virtual ram::math::Vector2 getDesiredVelocity() {return m_velocity;}
    virtual ram::math::Vector2 getDesiredPosition() {return m_position;}
    virtual ram::math::Vector2 getDesiredAccel() {return m_accel;}

    virtual ram::math::Quaternion getDesiredOrientation() {return m_orientation;}
    virtual ram::math::Vector3 getDesiredAngularRate() {return m_angularRate;}
    virtual ram::math::Vector3 getDesiredAngularAccel() {return m_angularAccel;}

    virtual bool atPosition(){return m_atPosition;}
    virtual bool atVelocity(){return m_atVelocity;}
    virtual bool atOrientation() { return m_atOrientation; }
    virtual bool atDepth() { return m_atDepth; }

    virtual void holdCurrentPosition() {}
    virtual void holdCurrentDepth() {}
    virtual void holdCurrentHeading() {}
    virtual void holdCurrentOrientation() {}
    
    virtual void setPriority(ram::core::IUpdatable::Priority) {};
    virtual ram::core::IUpdatable::Priority getPriority() {
        return ram::core::IUpdatable::NORMAL_PRIORITY;
    };
    virtual void setAffinity(size_t) {};
    virtual int getAffinity() {
        return -1;
    }
    virtual void update(double) {};
    virtual void background(int) {};
    virtual void unbackground(bool) {};
    virtual bool backgrounded() { return false; }

    double m_depth, m_depthRate, m_depthAccel;
    ram::math::Vector2 m_position, m_velocity, m_accel;
    ram::math::Quaternion m_orientation;
    ram::math::Vector3 m_angularRate, m_angularAccel;

    double yaw;
    bool m_atPosition;
    bool m_atVelocity;
    bool m_atOrientation;
    bool m_atDepth;
};

TEST(MockController)
{
    MockController* mockController = new MockController("Controller");
    ram::control::IController* controller = mockController;

    controller->changeDepth(5, 0);
    CHECK_EQUAL(5, mockController->m_depth);

    delete mockController;
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
    main_namespace["control"] = py::import("ext.control");
    main_namespace["controller"] = controllerPtr;

    // Create simple eval function
    boost::function<py::object (py::str)> eval =
        boost::bind(py::exec, _1, main_namespace, main_namespace);

    // Test speed (if this works everything else should to)
    eval("controller.changeDepth(3, 0)");
    CHECK_EQUAL(3, mockController->m_depth);
    
    eval("depth = controller.getDesiredDepth()");
    double depth = py::extract<double>(main_namespace["depth"]);
    CHECK_EQUAL(3, depth);
    
    } catch(py::error_already_set err) { PyErr_Print(); throw err; }
}

int main()
{
    Py_Initialize();
    // We must register this contverted so we can pass a pointer to python
    py::register_ptr_to_python< boost::shared_ptr<ram::control::IController> >();
    return UnitTest::RunAllTests();
    return 0;
}
