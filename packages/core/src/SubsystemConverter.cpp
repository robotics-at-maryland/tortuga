/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/core/src/SubsystemConverter.cpp
 */

// Library Includes
#include <boost/foreach.hpp>

// Project Includes
#include "core/include/SubsystemConverter.h"

namespace bp = boost::python;

namespace ram {
namespace core {

static DefaultSubsystemConverter DEFAULT_SUBSYSTEM_CONVERTER;
    
void SubsystemConverter::addSubsystemConverter(SubsystemConverter* converter)
{
    SubsystemConverter::getSubsystemConverterSet()->insert(converter);
}

SubsystemConverterSet* SubsystemConverter::getSubsystemConverterSet()
{
    static SubsystemConverterSet subsystemConverterSet;
    return &subsystemConverterSet;
}

boost::python::object SubsystemConverter::convertSubsystem(
    ram::core::SubsystemPtr subsystem)
{
    SubsystemConverter* converter = 0;

    // Check to see if we already have a python object
    if (0 != boost::get_deleter<bp::converter::shared_ptr_deleter>(subsystem))
        return DEFAULT_SUBSYSTEM_CONVERTER.convert(subsystem);
    
    SubsystemConverterSet* subsystemConverterSet =
        SubsystemConverter::getSubsystemConverterSet();
    
    BOOST_FOREACH(SubsystemConverter* conv, (*subsystemConverterSet))
    {
        // If we don't get None back, the converter succeeded
        if(Py_None != conv->convert(subsystem).ptr())
        {
            converter = conv;
            break;
        }
    }

    // If no converter was found, just use the default
    if (!converter)
        converter = &DEFAULT_SUBSYSTEM_CONVERTER;
        
    return converter->convert(subsystem);
}

// DefaultEventConverter Methods

DefaultSubsystemConverter::~DefaultSubsystemConverter()
{   
}
    
boost::python::object DefaultSubsystemConverter::convert(
    ram::core::SubsystemPtr subsystem)
{
    return boost::python::object(subsystem);
}

} // namespace core
} // namespace ram
