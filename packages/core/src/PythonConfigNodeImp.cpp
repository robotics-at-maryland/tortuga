/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/PythonConfigNodeImp.cpp
 */

#include <iostream>
#include <vector>

// Library Includea
//#include <Python.h>
#include "boost/foreach.hpp"

// Project Includes
#ifdef RAM_WINDOWS
#pragma warning( disable : 4121 )
#endif 
#include "core/include/PythonConfigNodeImp.h"


namespace py = boost::python;

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
        py::object main_module((py::handle<>(py::borrowed(
             PyImport_AddModule("__main__")))));

        py::object main_namespace = main_module.attr("__dict__");
//        py::object yaml = py::import("yaml");
//        main_namespace["yaml"] = yaml;

        std::stringstream ss;
        ss << "import yaml\nconfig = yaml.load(file(\"" << filename << "\"))";
        
        py::handle<> ignored(PyRun_String(ss.str().c_str(),
                                          Py_file_input,
                                          main_namespace.ptr(),
                                          main_namespace.ptr()));
        py::object obj(main_namespace["config"]);
    
        return ConfigNodeImpPtr(new PythonConfigNodeImp(obj));
    } catch(py::error_already_set err) {
        printf("ConfigNode (fromYamlFile) Error:\n");
        PyErr_Print();

        /// TODO: fix me with real throws and Python to C++ exception handling
        throw err;
    }

    // return ConfigNodeImpPtr();
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

    // return ConfigNodeImpPtr();
}

ConfigNodeImpPtr PythonConfigNodeImp::map(std::string key)
{
    try {
        std::string debugPath(m_debugPath + "." + key);

        if ((m_pyobj.ptr() != Py_None) &&
            PyObject_HasAttrString (m_pyobj.ptr(), "has_key"))
        {
            // Run the includes if needed
            includeIfNeeded(m_pyobj);
            if (m_pyobj.attr("has_key")(key))
            {
                return ConfigNodeImpPtr(new PythonConfigNodeImp(m_pyobj[key], debugPath));
            }
        }

        // If we got here, we haven't found it
        return ConfigNodeImpPtr(new PythonConfigNodeImp(py::object(), debugPath));
    } catch(py::error_already_set err) {
        printf("ConfigNode (map) Error:\n");
        PyErr_Print();

        throw err;
    }

    // return ConfigNodeImpPtr();
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

    // return "ERROR";
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

    // return "ERROR";
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
    
    // return -999999;
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
    
    // return -999999;
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

    // return -9999;
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

    // return -9999;
}

NodeNameList PythonConfigNodeImp::subNodes()
{
    try {
        // Make sure to pull in any nodes needed for includes
        includeIfNeeded(m_pyobj);

        // Get the python list that is the subnode names
        NodeNameList subnodes;
        py::object nodes(m_pyobj.attr("keys")());
        size_t size = py::len(nodes);
        
        for (size_t i = 0; i < size; ++i)
        {
            std::string nodeName = py::extract<std::string>(nodes[i]);

            // Only include values that aren't our special "INCLUDE" and
            // "INCLUDED_LOADED" values
            if ((nodeName != "INCLUDE") && (nodeName != "INCLUDE_LOADED"))
                subnodes.insert(nodeName);
        }
        return subnodes;
    } catch(py::error_already_set err ) {
        printf("ConfigNode \"%s\"(subNodes) Error:\n", m_debugPath.c_str());
        PyErr_Print();

        throw err;
    }
}

size_t PythonConfigNodeImp::size()
{
    try {
        return py::len(m_pyobj);
    } catch(py::error_already_set err ) {
        printf("ConfigNode \"%s\"(size) Error:\n", m_debugPath.c_str());
        PyErr_Print();

        throw err;
    }
}
    
void PythonConfigNodeImp::set(std::string key, std::string str)
{
    try {
        m_pyobj[key] = py::str(str);
    } catch(py::error_already_set err) {
        printf("ConfigNode (set) at: %s with key: %s Error:\n",
	       m_debugPath.c_str(), key.c_str());
        PyErr_Print();

        throw err;
    }
}

struct null_deleter
{
    void operator()(void const *) const
    {
    }
};


std::string PythonConfigNodeImp::toString()
{
    try {
        // Force includes of all nodes
        std::vector<ConfigNodeImpPtr> nodes;
        ConfigNodeImpPtr thisPtr(this, null_deleter());
        nodes.push_back(thisPtr);
        
        do {
            // Pull a node off the list
            ConfigNodeImpPtr currentNode = nodes.back();
            nodes.pop_back();

            // Get all of its subnodes
            PythonConfigNodeImp* imp =
                (PythonConfigNodeImp*)currentNode.get();
            if (PyObject_HasAttrString(imp->m_pyobj.ptr(), "has_key"))
            {
                NodeNameList subNodeNames = currentNode->subNodes();

                // Add then all to the list
                BOOST_FOREACH(std::string nodeName, subNodeNames)
                    nodes.push_back(currentNode->map(nodeName));
            }
            
        } while (nodes.size() > 0);
        
        return py::extract<std::string>(py::str(m_pyobj));
    } catch(py::error_already_set err) {
        printf("ConfigNode (toString) Error:\n");
        PyErr_Print();

        throw err;
    }
}

void PythonConfigNodeImp::includeIfNeeded(boost::python::object pyObj)
{
    try {
        // Only include if there is an include tag and we haven't already done
        // an include
        if ((!pyObj.attr("has_key")("INCLUDE_LOADED")) &&
            (pyObj.attr("has_key")("INCLUDE")))
        {
            py::object main_module((py::handle<>(py::borrowed(
                PyImport_AddModule("__main__")))));

            py::object main_namespace = main_module.attr("__dict__");
            main_namespace["node"] = pyObj;
        
            std::stringstream ss;
            ss << "import yaml, os, os.path\n"
                // All paths are resolved from the root of the SVN dir
               << "basePath = os.environ['RAM_SVN_DIR']\n"
               << "filePath = node['INCLUDE'].replace('/', os.sep)\n"
               << "fullPath = os.path.join(basePath, filePath)\n"
               << "cfg = yaml.load(file(os.path.normpath(fullPath)))\n"
                // Place all loaded item into the key
               << "for key, val in cfg.iteritems():\n"
               << "    node[key] = val\n"
                // Mark it already loaded
               << "node['INCLUDE_LOADED'] = True";
        
            py::object obj(py::handle<> (PyRun_String(ss.str().c_str(),
                                                      Py_file_input,
                                                      main_namespace.ptr(),
                                                      main_namespace.ptr())));
        }
    } catch(py::error_already_set err) {
        printf("Error during include:\n");
        PyErr_Print();

        throw err;
    }

}

} // namespace core
} // namespace ram
