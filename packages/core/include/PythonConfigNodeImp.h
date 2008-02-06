/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/ConfigNodeImp.h
 */

#ifndef RAM_CORE_PYTHONCONFIGNODEIMP_06_27_2007
#define RAM_CORE_PYTHONCONFIGNODEIMP_06_27_2007

#include "core/include/Platform.h"

// Library Includes
// disable this warning, because the boost code is not confromat at /W4
#if RAM_COMPILER == RAM_COMPILER_MSVC
#   pragma warning( push )
#   pragma warning( disable : 4640 4244 )
#endif
#include <boost/python.hpp>
#if RAM_COMPILER == RAM_COMPILER_MSVC
#   pragma warning( pop )
#endif

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/ConfigNodeImp.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

/**
 * Implements the ConfigNodeImp based upon a python dict
 *
 */
class RAM_EXPORT PythonConfigNodeImp : public ConfigNodeImp
{
public:
    /** A config node from the given python dict like object */
    PythonConfigNodeImp(boost::python::object pyobj, 
			std::string debugPath = "ROOT");
    /** Create a python config node from the given python string */
    PythonConfigNodeImp(std::string pythonString);

    /** Allows easier wrapping when joined with the shared pointers */
    ConfigNode construct(boost::python::object pyobj);
    
    virtual ~PythonConfigNodeImp() {};
    
    /** Grab a section of the config like an array */
    virtual ConfigNodeImpPtr idx(int index);

    /** Grab a sub node with the same name */
    virtual ConfigNodeImpPtr map(std::string key);

    /** Convert the node to a string value */
    virtual std::string asString();

    /** Attempts conversion to string, if it fails return def */
    virtual std::string asString(const std::string& def);
    
    /** Convert the node to a double */
    virtual double asDouble();

    /** Attempts conversion to string, if it fails return def */
    virtual double asDouble(const double def);

    /** Convert the node to an int */
    virtual int asInt();

    /** Attempts conversion to int, if it fails return def */
    virtual int asInt(const int def);

    /** Returns the list of sub nodes of the current config node */
    virtual NodeNameList subNodes();

    /** The number of elements in an array or map node */
    virtual size_t size();
    
    /** Map a key to a given value */
    virtual void set(std::string key, std::string str);
    
    /** Open a file using the python Yaml parser */
    static ConfigNodeImpPtr fromYamlFile(std::string filename);

    virtual std::string toString();
private:
    boost::python::object m_pyobj;

    std::string m_debugPath;
};

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_PYTHONCONFIGNODEIMP_06_27_2007
