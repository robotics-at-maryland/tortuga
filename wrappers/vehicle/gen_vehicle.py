# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import wrap

from pyplusplus import module_builder

CONSTRUCT_TEMPLATE = """
#include \"core/include/PythonConfigNodeImp.h\"

::ram::vehicle::device::%(TYPE)sPtr pyconstruct (::ram::vehicle::VehiclePtr vehicle,
                          boost::python::object obj)
{
    ::ram::core::ConfigNode cfg(::ram::core::ConfigNodeImpPtr(new ::ram::core::PythonConfigNodeImp(obj)));
    return ::ram::vehicle::device::%(TYPE)sPtr(new ::ram::vehicle::device::%(TYPE)s(vehicle, cfg));
}
"""

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

def generate_vehicle_device(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    
    #local_ns.class_('Device').include()
    local_ns.class_('IDevice').include()

    # Wrap the thruster class
    thruster = local_ns.class_('Thruster')
    thruster.include()
#    thruster.constructors().exclude()
#    thruster.add_declaration_code(CONSTRUCT_TEMPLATE % {'TYPE' : 'Thruster'})
#    thruster.add_registration_code( 'Thruster_exposer.def( "pyconstruct", &::pyconstruct ); Thruster_exposer.staticmethod("pycrustruct");',
 #                                   works_on_instance = False )


    local_ns.typedef('IDevicePtr').include()
    local_ns.typedef('ThrusterPtr').include()

    # Remove this to prevent mutiple declarations
    # This still causes pure virtual calls, but it can be fixed
    local_ns.class_('Device').member_function('getName').exclude()
    local_ns.class_('Device').member_function('getVehicle').exclude()

  #  local_ns.class_
    

def insert_code(mb):
    mb.add_registration_code("""
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::IDevice> >();
    bp::register_ptr_to_python<boost::shared_ptr<ram::vehicle::device::Thruster> >();
    """)

def generate_code(module_name, files, output_dir, include_files,
                  extra_includes = []):
    wrap.generate_code(module_name, files, output_dir, include_files,
                       extra_includes,
                       {'vehicle' : generate_vehicle,
                        'vehicle::device' : generate_vehicle_device},
                       {'vehicle::device' : insert_code})
