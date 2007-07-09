# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import wrap
from wrap import make_already_exposed

from pygccxml import declarations
from pyplusplus import decl_wrappers
from pyplusplus import messages
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

def expose_device(local_ns, name, remove = True, cast = True):
    print 'NAME:',name
    device = local_ns.class_(name)
    device.include()

    if remove:
        device.member_function('getVehicle').call_policies = \
            call_policies.return_internal_reference()

#    if cast:
#        device.member_function('castTo').call_policies = \
#            call_policies.return_internal_reference()
    
    device.disable_warnings(messages.W1023)

    device.include_files.append( "vehicle/include/Vehicle.h" )
        
    local_ns.typedef(name + 'Ptr').include()

    return device


def generate_vehicle_device(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    # Wrap IDevice Class
    expose_device(local_ns, 'IDevice', cast = False);

    # Wrap the thruster class
    expose_device(local_ns, 'Thruster', False);

    # Wrap IMU class
    IMU = expose_device(local_ns, 'IMU', False);
    IMU.include_files.append( "imu/include/imuapi.h" )

    # Wrap PSU class
    PSU = expose_device(local_ns, 'PSU', False);
    PSU.include_files.append( "carnetix/include/ctxapi.h" )

    # Fix these damn things
    PSU.member_function('getBatteryVoltage').exclude()
    PSU.member_function('getBatteryCurrent').exclude()
    PSU.member_function('getTotalWattage').exclude()
    PSU.member_function('getVoltages').exclude()
    PSU.member_function('getCurrents').exclude()
    PSU.member_function('getWattages').exclude()
    PSU.member_function('getSupplyNames').exclude()

    #local_ns.class_('StringList').exclude()

    make_already_exposed(global_ns, 'ram::pattern', 'Subject')
    make_already_exposed(global_ns, 'ram::core', ['ConfigNode', 'IUpdatable',
                                                  'Updatable'])

def insert_code(mb):
    mb.add_registration_code("""
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::IDevice> >();
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::Thruster> >();
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::IMU> >();
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::PSU> >();
    """)

def generate_code(module_name, files, output_dir, include_files,
                  extra_includes = []):
    wrap.generate_code(module_name, files, output_dir, include_files,
                       extra_includes,
                       {'vehicle::device' : generate_vehicle_device},
                       {'vehicle::device' : insert_code})
