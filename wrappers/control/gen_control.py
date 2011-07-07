# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
# All rights reserved.
#
# Author: Jonathan Wonders <jwonders@umd.edu>
# File:  wrappers/estimation/gen_control.py

import os
from pyplusplus.module_builder import call_policies
from pygccxml import declarations
import buildfiles.wrap as wrap

def generate(module_builder, local_ns, global_ns):
    classes = []

    # Mark class from other modules as already exposed
    module_builder.class_('::ram::core::Subsystem').already_exposed = True
    module_builder.class_('::ram::math::Vector3').already_exposed = True
    Quaternion = module_builder.class_('::ram::math::Quaternion')
    Quaternion.already_exposed = True
    Quaternion.constructors().allow_implicit_conversion = False


    # Include controller classes
    IController = local_ns.class_('IController')
    IController.include()
    
    IController.include_files.append(os.environ['RAM_SVN_DIR'] +
                                     '/packages/control/include/IController.h')
    classes.append(IController)

    # Include desired state class
    DesiredState = global_ns.class_('DesiredState')
    DesiredState.include()
    DesiredState.include_files.append(os.environ['RAM_SVN_DIR'] +
                                     '/packages/control/include/DesiredState.h')
    classes.append(DesiredState)

    yawVehicleHelper = local_ns.free_function('yawVehicleHelper')
    pitchVehicleHelper = local_ns.free_function('pitchVehicleHelper')
    rollVehicleHelper = local_ns.free_function('rollVehicleHelper')
    holdCurrentHeadingHelper = local_ns.free_function('holdCurrentHeadingHelper')

    yawVehicleHelper.include()
    pitchVehicleHelper.include()
    rollVehicleHelper.include()
    holdCurrentHeadingHelper.include()

    # Wrap Events
    events = wrap.expose_events(local_ns)

    if events:
        wrap.make_already_exposed(global_ns, 'ram::core', ['Event'])
        classes += events

    # Add a castTo
    wrap.registerSubsystemConverter(IController)

    wrap.add_needed_includes(classes)
#    wrap.make_already_exposed(global_ns, 'ram::pattern', 'Subject')
    module_builder.add_registration_code("registerIControllerPtrs();")

    include_files = set([cls.location.file_name for cls in classes])
    include_files.add('packages/control/include/Helpers.h')
    for cls in classes:
        include_files.update(cls.include_files)
    return ['wrappers/control/include/RegisterFunctions.h'] + list(include_files)

