/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/src/Subsystem.cpp
 */

// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "core/include/Subsystem.h"
#include "core/include/EventHub.h"

namespace bp = boost::python;

class SubsystemWrapper : public ram::core::Subsystem,
                         public bp::wrapper< ram::core::Subsystem >
{
public:    
    SubsystemWrapper(
        std::string name,
        ram::core::EventHubPtr eventHub = ram::core::EventHubPtr()) :
        Subsystem(name, eventHub),
        bp::wrapper< ram::core::Subsystem >()
    {}

    SubsystemWrapper(std::string name,
                     ram::core::SubsystemList list) :
        Subsystem(name, list),
        bp::wrapper< ram::core::Subsystem >()
    {}

    virtual void background( int interval=-1 ){
        bp::override func_background = this->get_override( "background" );
        func_background( interval );
    }

    virtual bool backgrounded(  ){
        bp::override func_backgrounded = this->get_override( "backgrounded" );
        return func_backgrounded(  );
    }

    virtual void unbackground( bool join=false ){
        bp::override func_unbackground = this->get_override( "unbackground" );
        func_unbackground( join );
    }

    virtual void update( double timestep ){
        bp::override func_update = this->get_override( "update" );
        func_update( timestep );
    }
};

void registerSubsystemClass()
{
    bp::class_<SubsystemWrapper,
        bp::bases<ram::core::IUpdatable, ram::core::EventPublisher>,
        boost::noncopyable >(
            "Subsystem",
            bp::init<std::string, bp::optional<ram::core::EventHubPtr> >(
                (bp::arg("name"),
                 bp::arg("eventHub") = ram::core::EventHubPtr())) )
        .def("getName", &ram::core::Subsystem::getName)
        .add_property("name", &ram::core::Subsystem::getName);

    bp::register_ptr_to_python< ram::core::SubsystemPtr >();
    bp::implicitly_convertible< ram::core::SubsystemPtr, boost::shared_ptr< ram::core::EventPublisher > >();
/*    bp::implicitly_convertible< ram::core::SubsystemPtr, boost::shared_ptr< ram::core::IUpdatable > >();
    bp::register_ptr_to_python< const ram::core::SubsystemPtr >();
    bp::implicitly_convertible< const ram::core::SubsystemPtr, const boost::shared_ptr< ram::core::EventPublisher > >();
    bp::implicitly_convertible< const ram::core::SubsystemPtr, const boost::shared_ptr< ram::core::IUpdatable > >();*/
}
