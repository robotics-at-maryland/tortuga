# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import wrap

from pygccxml import declarations
from pyplusplus import decl_wrappers
from pyplusplus import messages
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

def make_already_exposed(global_ns, namespace_name, classes, class_decs = None):
    if type(classes) is not list:
        classes = [classes]

    if class_decs is None:
        class_decs = []

    ns = global_ns
    for name in namespace_name.split('::'):
        print 'Going into namespace',name
        ns = ns.namespace(name)

    for class_name in classes:
        print 'Marking class',class_name
        class_ = ns.class_(class_name)
        class_.include()
        class_.already_exposed = True

    for class_dec in class_decs:
        print 'Marking class dec',class_dec
        class_dec = ns.decl(name = 'Vehicle' ,
                            decl_type = declarations.class_declaration_t)
        class_dec.already_exposed = True

def generate_vehicle(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    make_already_exposed(global_ns, 'ram::core', ['IUpdatable'])
    make_already_exposed(global_ns, 'ram::math', ['Vector3', 'Quaternion'])

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
                       {'vehicle' : generate_vehicle,
                        'vehicle::device' : generate_vehicle_device},
                       {'vehicle::device' : insert_code})
