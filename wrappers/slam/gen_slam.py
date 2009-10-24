# Copyright (C) 2009 Maryland Robotics Club
# Copyright (C) 2009 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  wrappers/slam/gen_slam.py

import os

import buildfiles.wrap as wrap

def generate(module_builder, local_ns, global_ns):
    classes = []

    # Mark class from other modules as already exposed
    module_builder.class_('::ram::core::Subsystem').already_exposed = True
    module_builder.class_('::ram::math::Vector2').already_exposed = True
    #Quaternion = module_builder.class_('::ram::math::Quaternion')
    #Quaternion.already_exposed = True
    #Quaternion.constructors().allow_implicit_conversion = False


    # Include slam classes
    ISlam = local_ns.class_('ISlam')
    ISlam.include()
    
    ISlam.include_files.append(os.environ['RAM_SVN_DIR'] +
                                     '/packages/slam/include/ISlam.h')

    # Wrap Events
    eventsFound = False
    for cls in local_ns.classes(function= lambda x: x.name.endswith('Event'),
                                allow_empty = True):
        eventsFound = True
        cls.include()
        classes.append(cls)

    if eventsFound:
        wrap.make_already_exposed(global_ns, 'ram::core', ['Event'])

    # Add a castTo
    wrap.registerSubsystemConverter(ISlam)

    wrap.add_needed_includes(classes)
#    wrap.make_already_exposed(global_ns, 'ram::pattern', 'Subject')
    module_builder.add_registration_code("registerISlamPtrs();")
    return ['wrappers/slam/include/RegisterFunctions.h']

