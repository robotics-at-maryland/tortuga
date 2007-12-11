/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/core/src/SubsystemList.cpp
 */

#include "boost/python.hpp"
#include "boost/python/suite/indexing/container_suite.hpp"
#include "boost/python/suite/indexing/vector.hpp"

#include "core/include/Subsystem.h"

namespace bp = boost::python;

void registerSubsystemList()
{
    { //::ram::core::SubsystemList
        typedef bp::class_< ram::core::SubsystemList > SubsystemListExposer_t;
        SubsystemListExposer_t subsystemListExposer = SubsystemListExposer_t("SubsystemList");
        bp::scope subsystemListScope(subsystemListExposer);

        unsigned long const methods_mask = bp::indexing::all_methods & 
            ~( bp::indexing::method_sort |  bp::indexing::method_count |  
               bp::indexing::method_contains |  bp::indexing::method_index ) ;
        subsystemListExposer.def( bp::indexing::vector_suite< ram::core::SubsystemList, methods_mask >() );
    }
}
