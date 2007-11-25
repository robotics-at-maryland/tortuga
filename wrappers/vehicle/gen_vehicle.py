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

def generate(local_ns, global_ns):
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

    # Include IVehicle class and handle special cases
    IVehicle = local_ns.class_('IVehicle')
    IVehicle.include()

    global_ns.typedef('TempNameList').include()
    global_ns.typedef('TempList').include()

    # Fix TempNameList (the include does stick)
    t = global_ns.class_(function =
                         lambda x: x.name.startswith('vector<std::string'))
    t.alias = 'TempNameList'

    # Fix overley long std::container names
    #wrap.mangle_container_names(local_ns)

    # Need to tell Boost.Python what the ownership policy for the raw pointer
    IVehicle.member_function('getDevice').call_policies = \
        call_policies.return_internal_reference()

    # Added the needed includes
    wrap.add_needed_includes([IVehicle])
    IVehicle.include_files.append(os.environ['RAM_SVN_DIR'] +
                                  '/packages/vehicle/include/device/IDevice.h')


