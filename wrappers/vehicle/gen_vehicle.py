# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/control/gen_vehicle.py

import os

import buildfiles.wrap as wrap
from buildfiles.wrap import make_already_exposed

import pygccxml
from pygccxml import declarations
from pygccxml import declarations as decls_package
from pyplusplus import decl_wrappers
from pyplusplus import messages
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

def generate(module_builder, local_ns, global_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    # Remove all those pesky warnings about only pointed to types!
    for cls in local_ns.decls(decl_type=decls_package.class_declaration_t):
        cls.include()
        cls.already_exposed = True

    wrap.make_already_exposed(global_ns, 'ram::core',
                              ['Subsystem'])

    classes = []

    # Include IVehicle class and handle special cases
    IVehicle = local_ns.class_('IVehicle')
    IVehicle.include()
    classes.append(IVehicle)

    # Remove default wrapper for getDevice
    IVehicle.member_function('getDevice').exclude()

    # Provide the normal one by hand, so C++ users of python subclasses
    # still get the overridden method
    IVehicle.add_wrapper_code("""
    virtual ::ram::vehicle::device::IDevicePtr getDevice( ::std::string name ){
        bp::override func = this->get_override(\"getDevice\");
        return func( name);
    }
    """)

    # Provide a hand made implementation for Python users of C++ IVehicle
    # subclasses, this uses our custom converter to ensure proper IDevice
    # downcasting
    IVehicle.add_declaration_code("""
    boost::python::object pyGetDevice(
        ram::vehicle::IVehicle& veh,
        std::string deviceName)
    {
        ram::vehicle::device::IDevicePtr device =
            veh.getDevice(deviceName);
        return ram::vehicle::device::IDeviceConverter::convertObject(device);
    }
    """)
    IVehicle.add_registration_code("def(\"getDevice\", &::pyGetDevice)",
                                   works_on_instance = True )
    IVehicle.include_files.append('vehicle/include/device/IDeviceConverter.h')

    global_ns.typedef('TempNameList').type.declaration.already_exposed = True
    global_ns.typedef('TempList').include()

    # Fix TempNameList (the include does stick)
    t = global_ns.class_(function =
                         lambda x: x.name.startswith('vector<std::string'))
    t.already_exposed = True

    # Fix overley long std::container names
    #wrap.mangle_container_names(local_ns)

    # Wrap Events
    eventsFound = False
    for cls in local_ns.classes(function= lambda x: x.name.endswith('Event'),
                                allow_empty = True):
        cls.include()
        classes.append(cls)

    if eventsFound:
        wrap.make_already_exposed(global_ns, 'ram::core', ['Event'])
    
    # Added the needed includes
    wrap.add_needed_includes(classes)
    IVehicle.include_files.append(os.environ['RAM_SVN_DIR'] +
                                  '/packages/vehicle/include/device/IDevice.h')

    # Add a castTo
    wrap.registerSubsystemConverter(IVehicle)

    module_builder.add_registration_code("registerIVehiclePtrs();")
    return ['wrappers/vehicle/include/RegisterFunctions.h']
