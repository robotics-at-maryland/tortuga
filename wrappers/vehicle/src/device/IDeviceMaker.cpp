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
#include "boost/tuple/tuple.hpp"

// Project Includes
#include "vehicle/include/device/IDeviceMaker.h"
#include "vehicle/include/device/IDeviceConverter.h"

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
        ram::core::ConfigNode config,
        ram::core::EventHubPtr eventHub,
        ram::vehicle::IVehiclePtr vehicle)

    {
        bp::override func_makeObject = this->get_override( "makeObject" );
        return func_makeObject(config, eventHub, vehicle);
    }
    
    virtual ram::vehicle::device::IDevicePtr makeObject(
        ram::vehicle::device::IDeviceMakerParamType params)
    {
        return makeObject(boost::get<0>(params), boost::get<1>(params),
                          boost::get<2>(params));
    }
    
    static boost::python::object newObject(
        ram::core::ConfigNode config,
        ram::core::EventHubPtr eventHub,
        ram::vehicle::IVehiclePtr vehicle)
    {
        ram::vehicle::device::IDevicePtr device =
            ram::vehicle::device::IDeviceMaker::newObject(
                boost::make_tuple(config, eventHub, vehicle));
        
        return ram::vehicle::device::IDeviceConverter::convertObject(device);
    }
};

typedef ram::vehicle::device::IDevicePtr
(IDeviceMakerWrapper::*makeObjectFuncType)
(ram::core::ConfigNode, ram::core::EventHubPtr,  ram::vehicle::IVehiclePtr);

void registerIDeviceMakerClass()
{
    bp::class_< IDeviceMakerWrapper, boost::noncopyable >(
        "IDeviceMaker", bp::init< std::string >(( bp::arg("key") )) )    
        .def( 
            "newObject"
            , &IDeviceMakerWrapper::newObject
            , ( bp::arg("config"), bp::arg("eventHub"), bp::arg("vehicle") ) )
        .def( 
            "getRegisteredKeys"
            , &ram::vehicle::device::IDeviceMaker::getRegisteredKeys)    
        .def( 
            "makeObject"
            , bp::pure_virtual(
                ((makeObjectFuncType)&IDeviceMakerWrapper::makeObject))
            , ( bp::arg("config"), bp::arg("eventHub"), bp::arg("vehicle") ) )
        .staticmethod( "newObject" )
        .staticmethod( "getRegisteredKeys" );
}
