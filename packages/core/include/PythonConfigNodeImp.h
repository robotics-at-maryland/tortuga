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

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/ConfigNodeImp.h"

namespace ram {
namespace core {

/**
 * Implements the ConfigNodeImp based upon a python dict
 *
 */
class PythonConfigNodeImp : public ConfigNodeImp
{
public:
    /** A config node from the given python dict like object */
    PythonConfigNodeImp(boost::python::object m_pyobj);
    /** Create a python config node from the given python string */
    PythonConfigNodeImp(std::string pythonString);

    virtual ~PythonConfigNodeImp() {};
    
    /** Grab a section of the config like an array */
    virtual ConfigNodeImpPtr idx(int index);

    /** Grab a sub node with the same name */
    virtual ConfigNodeImpPtr map(std::string key);

    /** Convert the node to a string value */
    virtual std::string asString();

    /** Convert the node to a double */
    virtual double asDouble();

    /** Convert the node to an int */
    virtual int asInt();

private:
    boost::python::object m_pyobj;
};

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_PYTHONCONFIGNODEIMP_06_27_2007
