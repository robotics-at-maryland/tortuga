/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/src/SubsystemMaker.cpp
 */


// Library Includes
#include "boost/python.hpp"

// Project Includes
#include "vehicle/include/device/IDeviceMaker.h"

namespace bp = boost::python;

struct IDeviceMakerWrapper : ram::vehicle::device::IDeviceMaker,
                             bp::wrapper< ram::vehicle::device::IDeviceMaker >
{

    IDeviceMakerWrapper(std::string key) : 
        ram::vehicle::device::IDeviceMaker(key),
        bp::wrapper< ram::vehicle::device::IDeviceMaker >(){
        // constructor
    }

    virtual ~IDeviceMakerWrapper() {};

    virtual ram::vehicle::device::IDevicePtr makeObject(
        ram::core::ConfigNode config)
    {
        bp::override func_makeObject = this->get_override( "makeObject" );
        return func_makeObject(config);
    }
};

void registerIDeviceMakerClass()
{
    bp::class_< IDeviceMakerWrapper, boost::noncopyable >(
        "IDeviceMaker", bp::init< std::string >(( bp::arg("key") )) )    
        .def( 
            "newObject"
            , &ram::vehicle::device::IDeviceMaker::newObject
            , ( bp::arg("config") ) )
        .def( 
            "getRegisteredKeys"
            , &ram::vehicle::device::IDeviceMaker::getRegisteredKeys)    
        .def( 
            "makeObject"
            , bp::pure_virtual( &IDeviceMakerWrapper::makeObject )
            , ( bp::arg("params") ) )
        .staticmethod( "newObject" )
        .staticmethod( "getRegisteredKeys" );
}
