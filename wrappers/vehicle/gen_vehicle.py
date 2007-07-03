# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import wrap

from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

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
    ivehicle_cls = local_ns.class_('IVehicle')
    ivehicle_cls.include()

    # Make things already exposed
    local_ns.typedef('IDevicePtr').already_exposed = True

def expose_device(local_ns, name, remove = True):
    print 'NAME:',name
    device = local_ns.class_(name)
    device.include()

    if remove:
        device.member_function('getVehicle').call_policies = \
            call_policies.return_internal_reference()
        
    local_ns.typedef(name + 'Ptr').include()

    return device

def generate_vehicle_device(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    # Wrap IDevice Class
    expose_device(local_ns, 'IDevice');

    # Wrap the thruster class
    expose_device(local_ns, 'Thruster', False);

    # Wrap IMU class
    device = expose_device(local_ns, 'IMU', False);
    device.include_files.append( "imu/include/imuapi.h" )
    

def insert_code(mb):
    mb.add_registration_code("""
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::IDevice> >();
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::Thruster> >();
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::IMU> >();
    """)

def generate_code(module_name, files, output_dir, include_files,
                  extra_includes = []):
    wrap.generate_code(module_name, files, output_dir, include_files,
                       extra_includes,
                       {'vehicle' : generate_vehicle,
                        'vehicle::device' : generate_vehicle_device},
                       {'vehicle::device' : insert_code})
