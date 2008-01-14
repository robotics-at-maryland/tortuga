/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/src/SubsystemMaker.cpp
 */

// STD Includes
#include <iostream>
#include <utility>

// Library Includes
#include "boost/python.hpp"
#include "boost/foreach.hpp"

// Project Includes
#include "core/include/SubsystemMaker.h"
#include "core/include/SubsystemConverter.h"

namespace bp = boost::python;

struct SubsystemMakerWrapper : ram::core::SubsystemMaker,
                               bp::wrapper< ram::core::SubsystemMaker >
{

    SubsystemMakerWrapper(std::string key) : 
        ram::core::SubsystemMaker(key),
        bp::wrapper< ram::core::SubsystemMaker >(){
        // constructor
    }

    virtual ~SubsystemMakerWrapper() {};

    virtual ram::core::SubsystemPtr makeObject(
        ram::core::SubsystemMakerParamType params)
    {
        bp::override func_makeObject = this->get_override( "makeObject" );
        bp::list deps;
        
        BOOST_FOREACH(ram::core::SubsystemPtr subsystem, params.second)
        {
            deps.append(
                ram::core::SubsystemConverter::convertSubsystem(subsystem));
        }
        
        return func_makeObject(params.first, deps);
    }

    static boost::python::object newObject(
        ram::core::ConfigNode config,
        ram::core::SubsystemList deps)
    {
        ram::core::SubsystemPtr subsystem =
            ram::core::SubsystemMaker::newObject(std::make_pair(config, deps));
        
        return ram::core::SubsystemConverter::convertSubsystem(subsystem);
    }
};

void registerSubsystemMakerClass(){

    bp::class_< SubsystemMakerWrapper, boost::noncopyable >( "SubsystemMaker", bp::init< std::string >(( bp::arg("key") )) )    
        .def( 
            "newObject"
            , &SubsystemMakerWrapper::newObject
            , ( bp::arg("config"), bp::arg("deps") ) )
        .def( 
            "getRegisteredKeys"
            , &SubsystemMakerWrapper::getRegisteredKeys)    
        .def( 
            "makeObject"
            , bp::pure_virtual( &SubsystemMakerWrapper::makeObject )
            , ( bp::arg("params") ) )
        .staticmethod( "newObject" )
        .staticmethod( "getRegisteredKeys" );
    

}
