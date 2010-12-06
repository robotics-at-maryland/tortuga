# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/control/gen_control.py

import os

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
    for cls in classes:
        include_files.update(cls.include_files)
    return ['wrappers/control/include/RegisterFunctions.h'] + list(include_files)

