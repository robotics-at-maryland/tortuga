# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import wrap

from pyplusplus import module_builder

def generate_vehicle(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    # Include classes to be wrapped
    local_ns.class_('Vehicle').include()
    local_ns.class_('IVehicle').include()
    
    # Set the call policy so Python doesn't take control of the returned object
    mem_fun = local_ns.class_('IVehicle').member_function('getDevice')
    mem_fun.call_policies = module_builder.call_policies.return_internal_reference()

    mem_fun = local_ns.class_('Vehicle').member_function('getDevice')
    mem_fun.call_policies = module_builder.call_policies.return_internal_reference()

def generate_vehicle_device(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    local_ns.class_('Device').include()
    local_ns.class_('IDevice').include()
    local_ns.class_('Thruster').include()

    # Remove this to prevent mutiple declarations
    # This still causes pure virtual calls, but it can be fixed
    local_ns.class_('Device').member_function('getName').exclude()


def generate_code(module_name, files, output_dir, include_files,
                  extra_includes = []):
    wrap.generate_code(module_name, files, output_dir, include_files,
                       extra_includes,
                       {'vehicle' : generate_vehicle,
                        'vehicle::device' : generate_vehicle_device })

