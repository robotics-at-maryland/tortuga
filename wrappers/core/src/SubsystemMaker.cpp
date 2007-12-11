/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/src/SubsystemMaker.cpp
 */


#include "boost/python.hpp"
#include "core/include/SubsystemMaker.h"
#include "iostream"
#include <utility>

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

    virtual ram::core::SubsystemPtr makeObject(ram::core::SubsystemMakerParamType params)
    {
        bp::override func_makeObject = this->get_override( "makeObject" );
        return func_makeObject(params.first, params.second);
    }

    static ram::core::SubsystemPtr newObject(
        ram::core::ConfigNode config,
        ram::core::SubsystemList deps)
    {
        return ram::core::SubsystemMaker::newObject(std::make_pair(config, deps));
    }
};

void registerSubsystemMakerClass(){

    bp::class_< SubsystemMakerWrapper, boost::noncopyable >( "SubsystemMaker", bp::init< std::string >(( bp::arg("key") )) )    
        .def( 
            "newObject"
            , &SubsystemMakerWrapper::newObject
            , ( bp::arg("config"), bp::arg("deps") ) )    
        .def( 
            "makeObject"
            , bp::pure_virtual( &SubsystemMakerWrapper::makeObject )
            , ( bp::arg("params") ) )    
        .staticmethod( "newObject" );

}
