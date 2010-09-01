/// *
//  * Copyright (C) 2007 Robotics at Maryland
//  * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
//  * All rights reserved.
//  *
//  * Author: Joseph Lisee <jlisee@umd.edu>
//  * File:  packages/math/test/src/TestChecks.h
//  */

// // Library Includes
// #include <UnitTest++/UnitTest++.h>

// // Library Includes
// #include <boost/bind.hpp>
// #include <boost/function.hpp>
// #include <boost/shared_ptr.hpp>
// #include <boost/python.hpp>

// // Project Includes
// #include "control/include/IController.h"
// #include "math/include/Vector3.h"

// class MockController : public ram::control::IController
// {
// public:
//     MockController(std::string name) : IController(name) {}

//     virtual void setVelocity(ram::math::Vector2 velocity_) {
//         velocity = velocity_; }

//     virtual ram::math::Vector2 getVelocity() {
//         return ram::math::Vector2::ZERO; }
    
//     virtual void setSpeed(double speed_) { speed = speed_; }

//     virtual void setSidewaysSpeed(double speed) { sidewaysSpeed = speed; }
    
//     virtual void setDepth(double depth_) { depth = depth_; }

//     virtual double getSpeed() { return speed; }

//     virtual double getSidewaysSpeed() { return sidewaysSpeed; }

//     virtual void holdCurrentPosition() {}

//     virtual void setDesiredVelocity(ram::math::Vector2 velocity,
//                                     int frame){}
//     virtual void setDesiredPosition(ram::math::Vector2 position,
//                                     int frame){}
//     virtual void setDesiredPositionAndVelocity(ram::math::Vector2 position,
//                                                ram::math::Vector2 velocity){}

//     virtual ram::math::Vector2 getDesiredVelocity(int frame)
//     {return ram::math::Vector2::ZERO;}
//     virtual ram::math::Vector2 getDesiredPosition(int frame)
//     {return ram::math::Vector2::ZERO;}

//     virtual bool atPosition(){return 0;}
//     virtual bool atVelocity(){return 0;}

//     virtual double getDepth() { return depth; }
    
//     virtual double getEstimatedDepth() { return 0; }
    
//     virtual double getEstimatedDepthDot() { return 0; }

//     virtual void yawVehicle(double degrees) { yaw = degrees; }

//     virtual void pitchVehicle(double degrees) {  }

//     virtual void rollVehicle(double degrees) { }

//     virtual ram::math::Quaternion getDesiredOrientation()
//         { return ram::math::Quaternion(); }
	
// 	virtual void setDesiredOrientation(ram::math::Quaternion)
// 		{ }
    
//     virtual bool atOrientation() { return atorientation; }
    
//     virtual bool atDepth() { return atdepth; }

//     virtual void holdCurrentDepth() {}

//     virtual void holdCurrentHeading() {}
    
//     virtual void setPriority(ram::core::IUpdatable::Priority) {};
//     virtual ram::core::IUpdatable::Priority getPriority() {
//         return ram::core::IUpdatable::NORMAL_PRIORITY;
//     };
//     virtual void setAffinity(size_t) {};
//     virtual int getAffinity() {
//         return -1;
//     }
//     virtual void update(double) {};
//     virtual void background(int) {};
//     virtual void unbackground(bool) {};
//     virtual bool backgrounded() { return false; }

//     ram::math::Vector2 velocity;
//     double speed;
//     double sidewaysSpeed;
//     double depth;
//     double yaw;
//     bool atorientation;
//     bool atdepth;
// };

// TEST(MockController)
// {
//     MockController* mockController = new MockController("Controller");
//     ram::control::IController* controller = mockController;

//     controller->setSpeed(5);
//     CHECK_EQUAL(5, mockController->speed);

//     delete mockController;
// }


// namespace py = boost::python;

// TEST(Emedding)
// {
//     try {
//     py::object main_module = py::import("__main__");
//     py::object main_namespace = main_module.attr("__dict__");
//     py::object ignored = py::exec("result = 5 ** 2", main_namespace,
//                                   main_namespace);
//     int five_squared = py::extract<int>(main_namespace["result"]);

//     CHECK_EQUAL(25, five_squared);
//     } catch(py::error_already_set err) { PyErr_Print(); throw err; }
// }

// TEST(ControlImport)
// {
//     try {
//     py::import("ext.control");
//     } catch(py::error_already_set err) { PyErr_Print(); throw err; }
// }

// TEST(ControllerWrapping)
// {
//     try {
    
//     // Create MockController and wrap it in smart point to allow python to
//     // handle it
//     MockController* mockController = new MockController("Controller");
//     boost::shared_ptr<ram::control::IController> controllerPtr(mockController);

//     // Create out namespace for the python code to operate in
//     py::object main_module = py::import("__main__");
//     py::object main_namespace = main_module.attr("__dict__");

//     // Import our module to test and inject the controller object
//     main_namespace["control"] = py::import("ext.control");
//     main_namespace["controller"] = controllerPtr;

//     // Create simple eval function
//     boost::function<py::object (py::str)> eval =
//         boost::bind(py::exec, _1, main_namespace, main_namespace);

//     // Test speed (if this works everything else should to)
//     eval("controller.setSpeed(3)");
//     CHECK_EQUAL(3, mockController->speed);
    
//     eval("speed = controller.getSpeed()");
//     double speed = py::extract<double>(main_namespace["speed"]);
//     CHECK_EQUAL(3, speed);
    
//     } catch(py::error_already_set err) { PyErr_Print(); throw err; }
// }

int main()
{
//     Py_Initialize();
//     // We must register this contverted so we can pass a pointer to python
//     py::register_ptr_to_python< boost::shared_ptr<ram::control::IController> >();
//     return UnitTest::RunAllTests();
    return 0;
}
