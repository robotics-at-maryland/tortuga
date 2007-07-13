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

PythonConfigNodeImp::PythonConfigNodeImp(py::object pyobj,
					 std::string debugPath) :
    m_pyobj(pyobj),
    m_debugPath(debugPath)
{}

PythonConfigNodeImp::PythonConfigNodeImp(std::string pythonString) :
    m_debugPath("ROOT")
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
    } catch(py::error_already_set err) {
        printf("ConfigNode (constructor) Error:\n");
        printf("\tTrying to parse: %s", pythonString.c_str());
        PyErr_Print();

        /// TODO: fix me with real throws and Python to C++ exception handling
        throw err;
    }
}

ConfigNodeImpPtr PythonConfigNodeImp::fromYamlFile(std::string filename)
{
    // Make sure python is initialized
    if (!Py_IsInitialized())
        Py_Initialize();

    try {
        // Create our file object
        py::object file(py::handle<>(PyFile_FromString((char*)filename.c_str(),
                                                       "r")));
        py::object yaml = py::import("yaml");

        return ConfigNodeImpPtr(new PythonConfigNodeImp(yaml.attr("load")(file)));
    } catch(py::error_already_set err) {
        printf("ConfigNode (fromYamlFile) Error:\n");
        PyErr_Print();

        /// TODO: fix me with real throws and Python to C++ exception handling
        throw err;
    }

    return ConfigNodeImpPtr();
}
    
ConfigNode PythonConfigNodeImp::construct(py::object pyobj)
{
    return ConfigNode(ConfigNodeImpPtr(new PythonConfigNodeImp(pyobj)));
}
    
ConfigNodeImpPtr PythonConfigNodeImp::idx(int index)
{
    try {
         std::stringstream ss;
         ss << m_debugPath << "[" << index << "]";

        if ((m_pyobj.ptr() != Py_None) &&
            PyObject_HasAttrString (m_pyobj.ptr(), "__getitem__"))
        {
            return ConfigNodeImpPtr(new PythonConfigNodeImp(m_pyobj[index],
							    ss.str()));
        }
        else
        {
            return ConfigNodeImpPtr(new PythonConfigNodeImp(py::object(),
							    ss.str()));
        }
    } catch(py::error_already_set err) {
        //return ConfigNodeImpPtr(new PythonConfigNodeImp(m_pyobj[index]));
        printf("ConfigNode (Index) Error:\n");
        PyErr_Print();

        throw err;
    }

    return ConfigNodeImpPtr();
}

ConfigNodeImpPtr PythonConfigNodeImp::map(std::string key)
{
    try {
        std::string debugPath(m_debugPath + "." + key);

        if ((m_pyobj.ptr() != Py_None) &&
            PyObject_HasAttrString (m_pyobj.ptr(), "has_key")
            && (m_pyobj.attr("has_key")(key)))
        {
	    return ConfigNodeImpPtr(new PythonConfigNodeImp(m_pyobj[key], debugPath));
        }
        else
        {
            //py::object newObject;
            //m_pyobj[key] = newObject;
            return ConfigNodeImpPtr(new PythonConfigNodeImp(py::object(), debugPath));
        }    
    } catch(py::error_already_set err) {
        printf("ConfigNode (map) Error:\n");
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
        printf("ConfigNode \"%s\"(asString) Error:\n", m_debugPath.c_str());
        PyErr_Print();

        throw err;
    }

    return "ERROR";
}

std::string PythonConfigNodeImp::asString(const std::string& def)
{
    try {
        if (m_pyobj.ptr() == Py_None)
            return def;
        else
            return std::string(py::extract<char*>(py::str(m_pyobj)));
        
    } catch(py::error_already_set err) {
        return def;
    }

    return "ERROR";
}
    
double PythonConfigNodeImp::asDouble()
{
    try {
        return py::extract<double>(m_pyobj);
    } catch(py::error_already_set err ) {
        printf("ConfigNode \"%s\"(asDouble) Error:\n", m_debugPath.c_str());
        PyErr_Print();

        throw err;
    }
    
    return -999999;
}

double PythonConfigNodeImp::asDouble(const double def)
{
    try {
        if (m_pyobj.ptr() == Py_None)
            return def;
        else
            return py::extract<double>(m_pyobj);
    } catch(py::error_already_set err ) {
        return def;
    }
    
    return -999999;
}
    
int PythonConfigNodeImp::asInt()
{
    try {
        return py::extract<int>(m_pyobj);
    } catch(py::error_already_set err ) {
        printf("ConfigNode \"%s\"(asInt) Error:\n", m_debugPath.c_str());
        PyErr_Print();

        throw err;
    }

    return -9999;
}

int PythonConfigNodeImp::asInt(const int def)
{
    try {
        if (m_pyobj.ptr() == Py_None)
            return def;
        else
            return py::extract<int>(m_pyobj);
    } catch(py::error_already_set err ) {
        return def;
    }

    return -9999;
}
    
} // namespace core
} // namespace ram
