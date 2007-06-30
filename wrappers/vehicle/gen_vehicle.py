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

    # Include vehicle class and handle special cases
    vehicle_cls = local_ns.class_('Vehicle')
    vehicle_cls.include()
    vehicle_cls.member_function('getState').exclude()
    vehicle_cls.member_function('setState').exclude()
    local_ns.typedef('NameDeviceMap').exclude()

    # Handle IVehicle class
    local_ns.class_('IVehicle').include()

    # Make things already exposed
    local_ns.typedef('IDevicePtr').already_exposed = True

def generate_vehicle_device(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    
    local_ns.class_('Device').include()
    local_ns.class_('IDevice').include()
    local_ns.class_('Thruster').include()

    local_ns.typedef('DevicePtr').include()
    local_ns.typedef('IDevicePtr').include()
    local_ns.typedef('ThrusterPtr').include()

    # Remove this to prevent mutiple declarations
    # This still causes pure virtual calls, but it can be fixed
    local_ns.class_('Device').member_function('getName').exclude()


def insert_code(mb):
    mb.add_registration_code("""
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::IDevice> >();
    """)

def generate_code(module_name, files, output_dir, include_files,
                  extra_includes = []):
    wrap.generate_code(module_name, files, output_dir, include_files,
                       extra_includes,
                       {'vehicle' : generate_vehicle,
                        'vehicle::device' : generate_vehicle_device,
                        'insert_code' : insert_code})
