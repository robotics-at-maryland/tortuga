# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import helpers

from pyplusplus import module_builder

def generate_vehicle_devices(name, global_ns, local_ns):
    local_ns.include()

def generate_vehicle(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    local_ns.include()

    # Set the call policy so Python doesn't take control of the returned object
    mem_fun = local_ns.class_('Vehicle').member_function('getDevices')
    mem_fun.call_policies = module_builder.call_policies.return_internal_reference()

def generate_code(module_name, files, output_dir, include_files):
    helpers.generate_code(module_name, files, output_dir, include_files,
                          {'vehicle' : generate_vehicle,
                           'vehicle::device' : generate_vehicle_devices})

