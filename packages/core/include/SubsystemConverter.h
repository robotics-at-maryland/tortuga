/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/core/include/SubsystemConverter.h
 */

#ifndef RAM_CORE_SUBSYSTEMCONVERTER_H_01_13_2008
#define RAM_CORE_SUBSYSTEMCONVERTER_H_01_13_2008

// STD Includes
#include <string>
#include <set>

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Subsystem.h"

namespace ram {
namespace core {

class SubsystemConverter;
typedef std::set<SubsystemConverter*> SubsystemConverterSet;

class SubsystemConverter
{
public:
    virtual ~SubsystemConverter() {};
    
    virtual boost::python::object convert(
        ram::core::SubsystemPtr subsystem) = 0;

    static void addSubsystemConverter(SubsystemConverter* converter);

    static boost::python::object convertSubsystem(
        ram::core::SubsystemPtr subsystem);
private:
    /** Gets the global set of subsystem converters */
    static SubsystemConverterSet* getSubsystemConverterSet();
};

class DefaultSubsystemConverter : public SubsystemConverter
{
public:
    virtual ~DefaultSubsystemConverter();
    
    virtual boost::python::object convert(ram::core::SubsystemPtr subsystem);
};

template<class T>
class SpecificSubsystemConverter : public SubsystemConverter
{
public:
    SpecificSubsystemConverter()
    {
        SubsystemConverter::addSubsystemConverter(this);
    }
    
    virtual boost::python::object convert(ram::core::SubsystemPtr subsystem)
    {
        boost::shared_ptr<T> result = boost::dynamic_pointer_cast<T>(subsystem);

        // If the conversion was succesfull return the proper python object
        if (result)
            return boost::python::object(result);

        // If not, return None
        return boost::python::object();
    }
};

} // namespace core
} // namespace ram
    
#endif // RAM_CORE_SUBSYSTEMCONVERTER_H_01_13_2008
