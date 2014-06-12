# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import buildfiles.wrap as wrap
from buildfiles.wrap import make_already_exposed

from pygccxml import declarations
from pygccxml import declarations as decls_package
from pyplusplus import decl_wrappers
from pyplusplus import messages
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

import pyplusplus.decl_wrappers.decl_wrapper_printer as decl_printer

def expose_device(local_ns, name, register = True):
    print 'NAME:',name
    device = local_ns.class_(name)
    device.include()

    if register:
        wrap.registerConverter(
            'ram::vehicle::device::SpecificIDeviceConverter',
            device, 'vehicle/include/device/IDeviceConverter.h')
    
    
    return device


def generate(module_builder, local_ns, global_ns):
    """
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    
    # Remove all those pesky warnings about only pointed to types!
    for cls in local_ns.decls(decl_type=decls_package.class_declaration_t):
        cls.include()
        cls.already_exposed = True

    module_builder.class_('::ram::core::EventPublisher').already_exposed = True
    module_builder.class_('::ram::core::IUpdatable').already_exposed = True

    wrappedClasses = []

    # Wrap IDevice Class
    IDevice = expose_device(local_ns, 'IDevice', register = False);
    wrappedClasses.append(IDevice)

    # Wrap the thruster class
    IThruster = expose_device(local_ns, 'IThruster')
    wrappedClasses.append(IThruster)

    # Wrap the IDepthSensor class
    IDepthSensor = expose_device(local_ns, 'IDepthSensor')
    wrappedClasses.append(IDepthSensor)

    # Wrap marker interfaces (slightly abuse expose device)
    IVoltageProvider = expose_device(local_ns, 'IVoltageProvider',
                                     register = False)
    wrappedClasses.append(IVoltageProvider)
    ICurrentProvider = expose_device(local_ns, 'ICurrentProvider',
                                     register = False)
    wrappedClasses.append(ICurrentProvider)

    # Wrap the IPowerSource class
    IPowerSource = expose_device(local_ns, 'IPowerSource')
    wrappedClasses.append(IPowerSource)

    # Wrap the ITempSensor class
    ITempSensor = expose_device(local_ns, 'ITempSensor')
    wrappedClasses.append(ITempSensor)

    # Wrap the IIMU class
    IIMU = expose_device(local_ns, 'IIMU')
    wrappedClasses.append(IIMU)

    # Wrap the ISonar class
    ISonar = expose_device(local_ns, 'ISonar')
    wrappedClasses.append(ISonar)

    # Wrap the IPayloadSet class
    IPayloadSet = expose_device(local_ns, 'IPayloadSet')
    wrappedClasses.append(IPayloadSet)

    # Wrap the IVelocitySensor class
    IVelocitySensor = expose_device(local_ns, 'IVelocitySensor')
    wrappedClasses.append(IVelocitySensor)

    # Wrap the IPositionSensor class
    IPositionSensor = expose_device(local_ns, 'IPositionSensor')
    wrappedClasses.append(IPositionSensor)

    # Wrap Events
    events = wrap.expose_events(local_ns)
    if events:
        module_builder.class_('::ram::core::Event').already_exposed = True
        classes += events

    module_builder.add_registration_code("registerIDeviceMakerClass();")
    module_builder.add_registration_code("registerIDevicePtrs();")
    wrap.add_needed_includes(wrappedClasses)

    include_files = set([cls.location.file_name for cls in wrappedClasses])
    for cls in wrappedClasses:
        include_files.update(cls.include_files)
    return ['wrappers/vehicle/include/RegisterFunctions.h',
            'core/include/Event.h'] + list(include_files)
