/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/PythonConfigNodeImp.cpp
 */

// Library Includes
#include <boost/python.hpp>
//#include <Python.h>
namespace py = boost::python;

// Project Includes
#include "core/include/PythonConfigNodeImp.h"

namespace ram {
namespace core {

PythonConfigNodeImp::PythonConfigNodeImp(py::object pyobj) :
    m_pyobj(pyobj)
{
}

PythonConfigNodeImp::PythonConfigNodeImp(std::string pythonString)
{
    // Make sure python is initialized
    if (!Py_IsInitialized())
        Py_Initialize();

    try {
        py::object main_module((py::handle<>(py::borrowed(
             PyImport_AddModule("__main__")))));

        py::object main_namespace = main_module.attr("__dict__");
        
        py::object obj(py::handle<> (PyRun_String(pythonString.c_str(),
                                                  Py_eval_input,
                                                  main_namespace.ptr(),
                                                  main_namespace.ptr())));

        m_pyobj = obj;
    } catch( py::error_already_set ) {
        PyErr_Print();
    }
}

ConfigNode PythonConfigNodeImp::construct(py::object pyobj)
{
    return ConfigNode(ConfigNodeImpPtr(new PythonConfigNodeImp(pyobj)));
}
    
ConfigNodeImpPtr PythonConfigNodeImp::idx(int index)
{
    try {
        return ConfigNodeImpPtr(new PythonConfigNodeImp(m_pyobj[index]));
    } catch(py::error_already_set err) {
        PyErr_Print();

        throw err;
    }

    return ConfigNodeImpPtr();
}

ConfigNodeImpPtr PythonConfigNodeImp::map(std::string key)
{
    try {
        return ConfigNodeImpPtr(new PythonConfigNodeImp(m_pyobj[key]));
    } catch(py::error_already_set err) {
        PyErr_Print();

        throw err;
    }

    return ConfigNodeImpPtr();
}

std::string PythonConfigNodeImp::asString()
{
    try {
        return std::string(py::extract<char*>(py::str(m_pyobj)));
    } catch(py::error_already_set err) {
        PyErr_Print();

        throw err;
    }

    return "ERROR";
}

double PythonConfigNodeImp::asDouble()
{
    try {
        return py::extract<double>(m_pyobj);
    } catch(py::error_already_set err ) {
        PyErr_Print();

        throw err;
    }
    
    return -999999;
}

int PythonConfigNodeImp::asInt()
{
    try {
        return py::extract<int>(m_pyobj);
    } catch(py::error_already_set err ) {
        PyErr_Print();

        throw err;
    }

    return -9999;
}
    
} // namespace core
} // namespace ram
