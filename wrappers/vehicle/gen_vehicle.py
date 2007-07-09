# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import wrap
from wrap import make_already_exposed


from pygccxml import declarations
from pygccxml import declarations as decls_package
from pyplusplus import decl_wrappers
from pyplusplus import messages
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

def generate_vehicle(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    # Remove all those pesky warnings about only pointed to types!
    for cls in local_ns.decls(decl_type=decls_package.class_declaration_t):
        cls.include()
        cls.already_exposed = True

    make_already_exposed(global_ns, 'ram::core', ['IUpdatable'])

    # Include vehicle class and handle special cases
    vehicle_cls = local_ns.class_('Vehicle')
    vehicle_cls.include()
    vehicle_cls.member_function('getState').exclude()
    vehicle_cls.member_function('setState').exclude()
    vehicle_cls.member_function('getDevice').alias = '_cpp_getDevice'
    vehicle_cls.member_function('_addDevice').alias = '_cpp_addDevice'
    local_ns.typedef('NameDeviceMap').exclude()
    
    # Handle IVehicle class
    ivehicle_cls = local_ns.class_('IVehicle')
    ivehicle_cls.include()

    # Make things already exposed
    IDevicePtr = local_ns.typedef('IDevicePtr')
    IDevicePtr.include()
    IDevicePtr.already_exposed = True

def generate_code(module_name, files, output_dir, include_files,
                  extra_includes = []):
    wrap.generate_code(module_name, files, output_dir, include_files,
                       extra_includes,
                       {'vehicle' : generate_vehicle})
